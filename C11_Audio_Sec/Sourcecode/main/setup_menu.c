#include "macro.h"
#include "key_task.h"
#include "setup_menu.h"
#include "setup_func.h"
#include "keydefine.h"
#include "vfd_disp.h"
#include "message.h"
#include "task.h"
#include "c11_msg_handler.h"
#include "debug.h"
#include "key_tab.h"
#include "radio_task.h"

#include "..\driver\volume.h"
#include "..\driver\vfddriver.h"
#include "..\driver\eeprom.h"
#include "..\driver\channel.h"
#include "..\cs495300\dspdrv.h" 
#include "..\cs495300\DSP_API.h"
#include "..\cs495300\SignalGenerator.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
#define _DEBUG_MENU
#endif

#define UNIVERSAL_PWD 	0x1100
#define DEFAULT_PWD 	0x0000
#define ENTER_OLD_PWD 	0
#define ENTER_NEW_PWD 1
#define REENTER_NEW_PWD 2
#define CHECK_SETUP_PWD  3

#define FLASH_START 0
#define FLASH_CONT 	1


#define FLASH_CHARACTER '_'

static TASK_ID bak_id;

uchar InNameChanging = 0;
uchar FlashPosition = 0;

uchar InPwdInput = 0;
uchar PwdCounter = 0;
uchar PwdInputState = CHECK_SETUP_PWD;
unsigned short current_pwd = 0;
unsigned short new_pwd = 0;
unsigned short user_pwd = 0;

unsigned char Movie_Music_Speaker_Flag=0; 

unsigned short ReadPwd()
{
	unsigned short rtn_value = 0;
	rtn_value = EepromReadByte(PASSWORD_H);
	rtn_value <<= 8;
	rtn_value |= EepromReadByte(PASSWORD_L);
	return rtn_value;
}

void WritePwd(unsigned short pwd)
{
	EepromWriteByte(PASSWORD_L,pwd & 0xFF);
	EepromWriteByte(PASSWORD_H,(pwd >> 8) & 0xFF);
}

//
//调用主菜单的子菜单中action 函数读取EEPROM中的值,并返回读到的值
//
char read_curr_option(MENU_ITEM *pItem)
{
	char rtn_value;
	
	if(pItem->pAction)
		{
		rtn_value = (*pItem->pAction)(READ_VALUE_MODE, 0, pItem->uPara); /*pItem->uPara 为子菜单当前选项数*/

		if(rtn_value >= pItem->uOptionNum) /*如果溢出，则调入默认值*/
			rtn_value = (*pItem->pAction)(LOAD_DEFAULT_MODE, 0, pItem->uPara);
		return rtn_value;
		}
	else
		return 0;
}

void write_curr_option(MENU_ITEM *pItem, char index)
{
	if(pItem->pAction)
		(*pItem->pAction)(WRITE_VALUE_MODE, index, pItem->uPara); 
}

//
// 调用主菜单的子菜单中action 函数用于功能的实现
//
uchar execute_curr_option(MENU_ITEM *pItem, unsigned char value)
{
	if(pItem->pAction)
		return (*pItem->pAction)(EXECUTE_MODE, value, pItem->uPara); 
	return 0;
}

char load_default_option(MENU_ITEM *pItem)
{
	if(pItem->pAction)
		return (*pItem->pAction)(LOAD_DEFAULT_MODE, 0, pItem->uPara);
	else
		return 0;
}

unsigned char Inquire_restrict_option(MENU_ITEM *pItem)
{
	BYTE tmp;
	if(pItem->pAction)
		tmp = (*pItem->pAction)(INQUIRE_RESTRICT, 0, pItem->uPara);
	else
		tmp = 0;
	return tmp;
}

char MenuLoadCurOption(MENU_ITEM * pCurItem)
{
	char rtn_option_index;

	switch(pCurItem->uType)
		{
		case GENERAL_ITEM_TYPE:
		case STRING_ITEM_TYPE:
		case RESTRICTED_ITEM_TYPE:
		case DELAY_DISTANCE_ITEM_TYPE:
		case VOLUME_ITEM_TYPE:
		case AV_DELAY_ITEM_TYPE:
		case BALANCE_ITEM_TYPE:
			rtn_option_index = read_curr_option(pCurItem);
			break;
		case GOTO_PAGE_TYPE:
			rtn_option_index = 0;
			break;
		default:
			rtn_option_index = -1; 
			break;
		}
	return rtn_option_index;
}   

MENU_PAGE* MenuFindPage(unsigned char uID)
{
	unsigned char tmp;
	for(tmp = 0; tmp < MENUPAGELIST_SIZE; tmp++)
		{
		if(MenuPageList[tmp].uID == uID)
			return (MENU_PAGE*)(MenuPageList+tmp);
		}
	return NULL;
}

void MenuCaptionShow(unsigned char x, char *pCap,unsigned char update)
{
#ifdef _DEBUG_MENU
	while(x)
		{
		debug_printf(" ");
		--x;
		}
	debug_printf(pCap);
	debug_printf("\n");
#endif
	if(update)
		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,pCap);
	else
		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_NONE,pCap);
}

void MenuHeaderShow(MENU_PAGE * pCurPage)
{
	if(pCurPage->pCap)
		{
#ifdef _DEBUG_MENU
		debug_printf("____________________\n");
		debug_printf(pCurPage->pCap);
		debug_printf("\n");
#endif
		//VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,pCurPage->pCap); 
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,pCurPage->pCap); //Change by cjm 2009-04-08
		}
}

void VfdShowMenuOption(const char *format,...)
{
	char temp_buffer[20];
	uchar length = 0;

	va_list arg_ptr;

	va_start (arg_ptr, format); // format string 
	vsprintf (temp_buffer, format, arg_ptr);
	length = strlen(temp_buffer);

#ifdef _DEBUG_MENU
	debug_printf("temp_buffer:%s\n",temp_buffer);
	debug_printf("length:%d\n",length);
#endif

	VfdDisplay(VFD_MAX_POSITION - length,VFD_UPDATE_LINE_ALL,"%s", temp_buffer);
}

extern MENU_PAGE * pMenuCurPage;	 
void MenuItemShow(MENU_PAGE * pCurPage,char pos)
{
	static unsigned char i;
	unsigned char index;
	MENU_ITEM * pTemp;
	short * short_table;
	float * float_table;
	float temp_value;
	unsigned char temp;
	unsigned char temp1;
	

	if(pCurPage->uType == OPTION_TYPE)
		{
		for(i = 0; i < (pCurPage->uItemNum); ++i) 
			{
			if(i == pos) 
				{
				switch(pCurPage->pItemArray[i].uType)
					{
					case NONE_OPTION_ITEM_TYPE:
					case GOTO_PAGE_TYPE:
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,1);
						break;
					case GENERAL_ITEM_TYPE:	
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,0);				
						if(pCurPage->pItemArray[i].uOptionNum > 0)
							{								
							pTemp = pCurPage->pItemArray + i;	
							
							//add by cjm 2009-03-31 
							if(pMenuCurPage->uID==SET_ANALOG_IN_PAGE)  //Set Analog In Page
								{
								if(i<=7) //Input1-Input8
									{
									temp= EepromReadByte(ASSIGNED_INPUT1+i);
									if(i==0) //Input1
										{
										if(temp==0) //XLR Digital
											VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"Not Available");
										else //XLR Analog
											{
											index = read_curr_option(pTemp);
											VfdShowMenuOption("%s",(pTemp->pOptionArray)[index].pCap); //显示设置的值	
											}
										}
									else //Input2-Input8
										{
										if((temp==0)||(temp==1)) //Toslink Coaxial
											VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"Not Available");
										else //Analog
											{
											index = read_curr_option(pTemp);
											VfdShowMenuOption("%s",(pTemp->pOptionArray)[index].pCap); //显示设置的值	
											}					
										}
									}
								else //7.1Input Tuner
									{
									index = read_curr_option(pTemp);
									VfdShowMenuOption("%s",(pTemp->pOptionArray)[index].pCap); //显示设置的值	
									}
								}
							//add by cjm 2009-03-31  
							else
								{
								index = read_curr_option(pTemp);
								VfdShowMenuOption("%s",(pTemp->pOptionArray)[index].pCap); //显示设置的值		
								}						
							}					
						break;
					case DELAY_DISTANCE_ITEM_TYPE:
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,0);
						pTemp = pCurPage->pItemArray + i;
						index = read_curr_option(pTemp);
						temp_value = index;
						VfdShowMenuOption("%3.1fft %2.1fM",temp_value,temp_value/METER2FEET);
						break;
					case STRING_ITEM_TYPE:
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,0);						
						//pTemp = pCurPage->pItemArray + i;
						//index = read_curr_option(pTemp);	//由于STRING_ITEM_TYPE已经保存在对应的数组中，所以显示的时候不用另外read eeprom					
						VfdShowMenuOption("%8s",(char *)pCurPage->pItemArray[i].pOptionArray);
						break;
					case RESTRICTED_ITEM_TYPE:
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,1);
						if(pCurPage->pItemArray[i].uOptionNum > 0)
							{
							pTemp = pCurPage->pItemArray + i;
							index = Inquire_restrict_option(pTemp);
							if(!index)
								index = read_curr_option(pTemp);
							}
						break;
					case AV_DELAY_ITEM_TYPE:
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,0);
						pTemp = pCurPage->pItemArray + i;
						index = read_curr_option(pTemp);
						if(index)
							VfdShowMenuOption("%3dms",index);
						else
							VfdShowMenuOption(strOff);
						break;	
					case VOLUME_ITEM_TYPE:
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,0);
						//Add by cjm 2009-03-31
						if (pMenuCurPage->uID==SET_ANALOG_LEVEL_PAGE) //Set Analog level Page
								{
								if(i<=7) //Input1-Input8
									{
									temp= EepromReadByte(ASSIGNED_INPUT1+i);
									if(i==0) //Input1
										{
										if(temp==0) //XLR Digital
											VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"Not Available");
										else //XLR Analog
											{
											temp1= EepromReadByte(ANALOG1_MODE_POSITION + i);
											if(temp1==0) //By pass
												VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"Not Available");
											else //DSP
												{
												pTemp = pCurPage->pItemArray + i;
												index = read_curr_option(pTemp);
												short_table = (short *)pCurPage->pItemArray[i].pOptionArray;
												VfdShowMenuOption("%4ddB",short_table[index]);
												}
											}
										}
									else //Input2-Input8
										{
										if((temp==0)||(temp==1)) //Toslink Coaxial
											VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"Not Available");
										else
											{
											temp1= EepromReadByte(ANALOG1_MODE_POSITION + i);
											if(temp1==0) //By pass
												VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"Not Available");
											else //DSP
												{
												pTemp = pCurPage->pItemArray + i;
												index = read_curr_option(pTemp);
												short_table = (short *)pCurPage->pItemArray[i].pOptionArray;
												VfdShowMenuOption("%4ddB",short_table[index]);
												}
											}
										}
									}
								}
						//Add by cjm 2009-03-31
						else
							{
							pTemp = pCurPage->pItemArray + i;
							index = read_curr_option(pTemp);
							short_table = (short *)pCurPage->pItemArray[i].pOptionArray;
							//VfdShowMenuOption("%4ddB",short_table[index]);
							VfdShowMenuOption("%4d",short_table[index]); //change by cjm
							}
						break;				
					case BALANCE_ITEM_TYPE:
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,0);
						pTemp = pCurPage->pItemArray + i;
						index = read_curr_option(pTemp);
						float_table = (float * )pCurPage->pItemArray[i].pOptionArray;
						VfdShowMenuOption("%3.1fdB",float_table[index]);
						break;
					case PASSWORD_ITEM_TYPE:
						MenuCaptionShow(0, pCurPage->pItemArray[i].pCap,1);
						break;
					}
				break;
				}
			}
		}
	else
		{
#ifdef _DEBUG_MENU
		debug_printf("%d-%d\n",pCurPage->uType,OPTION_TYPE);
#endif
		}
}

void MenuDisplay(MENU_PAGE * pCurPage,char pos)
{
	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);	/*Clear VFD buffer but don't update */
	
	MenuHeaderShow(pCurPage);   /*显示菜单头部分*/
	MenuItemShow(pCurPage,pos); /*显示菜单选项部分这时POS=0*/
#ifdef _DEBUG_MENU
	debug_printf("\n");
#endif
}

MENU_PAGE * pMenuCurPage;	/*current menu page*/
MENU_ITEM * pMenuCurItem;	/*current menu item*/
char bCusrPos = 0;
char bCurOptionIndex = 0;   /*If the current item has options, this variable indicates the current option's ordinal.*/

//
// 设置菜单显示
// bMode--函数执行的模式.(Read\Write\Execute)
// bValue--用于设置将要显示的菜单页的参数
// resValue--
//
SETUP_T SetupMenuDisplay(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
#ifdef _DEBUG_MENU
	debug_printf("menu:%d.%d\n",bValue,resValue);
#endif

	pMenuCurPage = MenuFindPage(bValue);
	if(!pMenuCurPage)
		return FAILURE;
	bCusrPos = resValue;
	pMenuCurItem = &(pMenuCurPage->pItemArray[bCusrPos]);  //Set pMenuCurItem

#if 0
	//这是需要在进入菜单项的时候立即执行的，其余的类型只要显示即可，在LEFT或者RIGHT时才执行
	if(pMenuCurItem->uType == IMMEDIATE_EXEC_ITEM_TYPE)
		{
		bCurOptionIndex = MenuLoadCurOption(pMenuCurItem);
		MenuExecCurOption(pMenuCurItem,bCurOptionIndex);	//csg add to execute it
		}
#endif

	MenuDisplay(pMenuCurPage,bCusrPos);
	return SUCCESS;
}

BOOL SetupMenuInit()
{
//	EepromWriteByte(BALANCE_TEST_MODE_POSITION, 0);	/*把TEST MODE设置为OFF*/
	InPwdInput = 0;
	pMenuCurPage = MenuFindPage(MAIN_PAGE); /*当前的菜单页指向MAIN_PAGE*/
	if(!pMenuCurPage)
		return FALSE;
	pMenuCurItem = pMenuCurPage->pItemArray; /*当前的菜单选项指向MainPageItem*/		
	bCusrPos = 0;

	MenuDisplay(pMenuCurPage,bCusrPos); /*显示菜单头和菜单选项*/

	if(bak_id != RADIO_ID) /*在radio中,LEFT,RIGHT是允许连按的*/
		{
		AddRemoteRepeatKey(_IR_ARROW_LEFT_KEY);
		AddRemoteRepeatKey(_IR_ARROW_RIGHT_KEY);
		}

	InNameChanging = 0;
	return TRUE;
}

void VfdInitDispPwd(char *name)
{
	PwdCounter = 0;
	current_pwd = 0;
	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);	/*Clear VFD buffer but don't update */
	VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,name);
	VfdDisplay(VFD_LINE2_START_POSITION+PwdCounter,VFD_UPDATE_LINE_ALL,"_");
}

void SetupInit()
{
#ifdef _DEBUG_MENU
	debug_printf("\n****SetupInit****\n");
#endif
	//WritePwd(UNIVERSAL_PWD);
	user_pwd = ReadPwd(); /*读取进入设置菜单用户密码*/

	bak_id = GetCurFocusTid(); /*存储当前的tid 为bak_tid*/
	SetCurFocusTid(SETUP_ID);  /*设置当前的TID 为SETUP_ID*/

	if(pwd_mode == 1) /*Password Mode 为on 时,pwd_mode=1,这时需要输入密码*/
		{
		InPwdInput = 1;
		PwdInputState = CHECK_SETUP_PWD; /*检查设置密码状态*/
		VfdInitDispPwd("Enter Password:");
		}
	else /*没设置密码,直接进入设置菜单*/
		{
		SetupMenuInit();
		}
}

void SetupExit()
{
	SpkBlanceTestMode(EXECUTE_MODE,0,1);	/*把TEST MODE设置为OFF*/

	//VfdSetDimerValue(sys_flag.system_dimer);
	Set7Ch1MainVolume(sys_flag.zone1_volume); 
	
	SetCurFocusTid(bak_id);

	if(bak_id != RADIO_ID)	/*在radio中，LEFT,RIGHT是允许连按的*/
		{
		DeleteRemoteRepeatKey(_IR_ARROW_LEFT_KEY);
		DeleteRemoteRepeatKey(_IR_ARROW_RIGHT_KEY);
		}

	DeleteRemoteRepeatKey(_IR_ARROW_DOWN_KEY);
	DeleteRemoteRepeatKey(_IR_ARROW_UP_KEY);
	InNameChanging = 0;

	if(bak_id == RADIO_ID)
		{
		//if(ResetRadio)
		RadioInit(current_radio_mode);
		}
	else
		{
#if 0	//当restore default之后，只有在power on才执行autoseek
		if(IsEverAutoseek())
			{
			}
		else
			{
			Zone1ProcAutoSeek(INPUT_CH71,3,1);	//start seek from 7.1ch, go on seeking until find a source
			SetAutoseekFlag(1);
			}
#endif
		}	
}

extern uchar balance_mode;
extern void DacMute(int on);
extern void SetBalanceTestSignal(uchar channel,uchar on);
void SetupMenuProcBalance()
{
	if((pMenuCurPage->uID == SPK_BALANCE_PAGE)||(pMenuCurPage->uID == SPK_BALANCE_PAGE_MUSIC))
		{
		if(balance_mode==1)
			{
			DacMute(1); /*去手动切换时的噪声*/
			if(pMenuCurItem->pAction == SpkBlanceSetup)
				{
				Change_DSP_Decoder_Code(1,15);/*加载SGEN Code*/  
				DelayMs(50); /*加载完固件后延时下等DSP 稳定*/	
				SetBalanceTestSignal(pMenuCurItem->uPara,1);
				}
			else
				{
				SetBalanceTestSignal(pMenuCurItem->uPara,0);
				}
			DacMute(0);/*去手动切换时的噪声*/		
			}
		}
}

void ProcSetupUpKey()
{
	unsigned char oldPos;
	oldPos = bCusrPos;

	if(InNameChanging) /*修改输入通道名字*/
		{
		if(pMenuCurItem->uType == STRING_ITEM_TYPE)
			{
			FlashPosition = execute_curr_option(pMenuCurItem, _NAME_UP_KEY); /*改变输入通道名字*/
			MsgSetFlash(FLASH_START,VFD_MAX_POSITION - FlashPosition,FLASH_CHARACTER); /*修改字符时显示跳动的"_"*/
			}
		}
	else
		{
		do{
			if(bCusrPos) //Move Cusor upwards, if at top, then go the bottom
				--bCusrPos;
			else
				bCusrPos = pMenuCurPage->uItemNum -1;
			if(oldPos == bCusrPos)
				break;     //Have search all the items

			if(pMenuCurPage->pItemArray[bCusrPos].uType == RESTRICTED_ITEM_TYPE)
				{   
				if(!Inquire_restrict_option(&(pMenuCurPage->pItemArray[bCusrPos]))) // not restricted , break;
					{
					break;
					}
				}
			} while(pMenuCurPage->pItemArray[bCusrPos].uType == DUMMY_ITEM_TYPE || pMenuCurPage->pItemArray[bCusrPos].uType == RESTRICTED_ITEM_TYPE);


		pMenuCurItem = &(pMenuCurPage->pItemArray[bCusrPos]);  //Set pMenuCurItem
		bCurOptionIndex = MenuLoadCurOption(pMenuCurItem);
		}
	SetupMenuProcBalance();
	MenuDisplay(pMenuCurPage,bCusrPos);

	//add cjm 2007-07-27
	if(pMenuCurPage->uID==LISTENPROFILE_PAGE)
		{
			switch(bCusrPos)
				{
				case 0:
					Movie_Music_Speaker_Flag=0;
					break;
				case 1:
					Movie_Music_Speaker_Flag=1;
					break;
				default:
					break;
				}	
			
			if(bCusrPos==0||bCusrPos==1)
				ChangeListenProfileMode();
		}//add cjm 2007-07-27
}
//
void ProcSetupDownKey()
{
	unsigned char oldPos;
	oldPos = bCusrPos;

	if(InNameChanging) /*修改输入通道名字*/
		{
		if(pMenuCurItem->uType == STRING_ITEM_TYPE)
			{
			FlashPosition = execute_curr_option(pMenuCurItem, _NAME_DOWN_KEY); /*改变输入通道名字*/
			MsgSetFlash(FLASH_START,VFD_MAX_POSITION - FlashPosition,FLASH_CHARACTER);  /*修改字符时显示跳动的"_"*/
			}
		}
	else
		{
		do{
			++bCusrPos; 			// Move Cusor upwards, if at top, then go the bottom
			if(bCusrPos >= pMenuCurPage->uItemNum)
				bCusrPos = 0;
			if(oldPos == bCusrPos)
				break;     //Have search all the items
			if(pMenuCurPage->pItemArray[bCusrPos].uType == RESTRICTED_ITEM_TYPE)
				{
				if(pMenuCurPage->pItemArray[bCusrPos].uType == RESTRICTED_ITEM_TYPE)
					{
					if(!Inquire_restrict_option(&(pMenuCurPage->pItemArray[bCusrPos]))) // not restricted , break;
						{
						break;
						}
					}
				}//
			}while(pMenuCurPage->pItemArray[bCusrPos].uType == DUMMY_ITEM_TYPE ||pMenuCurPage->pItemArray[bCusrPos].uType == RESTRICTED_ITEM_TYPE);

		pMenuCurItem = &(pMenuCurPage->pItemArray[bCusrPos]);  //Set pMenuCurItem,这时向下会指向下一个pMenuCurItem
		bCurOptionIndex = MenuLoadCurOption(pMenuCurItem);
		}
	SetupMenuProcBalance();
	MenuDisplay(pMenuCurPage,bCusrPos);

	//add cjm 2007-07-27
	if(pMenuCurPage->uID==LISTENPROFILE_PAGE)
		{
			switch(bCusrPos)
				{
				case 0:
					Movie_Music_Speaker_Flag=0;
					break;
				case 1:
					Movie_Music_Speaker_Flag=1;
					break;
				default:
					break;
				}	
			
			if(bCusrPos==0||bCusrPos==1)
				ChangeListenProfileMode();
		}//add cjm 2007-07-27
}

void ProcSetupLeftKey()
{
	unsigned char update = 0;
	unsigned char temp; 
	unsigned char temp1;
	
	switch(pMenuCurItem->uType)
		{
		case GENERAL_ITEM_TYPE:
		case RESTRICTED_ITEM_TYPE:		
			//add by cjm 2009-03-31
			if(pMenuCurPage->uID==SET_ANALOG_IN_PAGE) //用于Not available 
				{
				if(pMenuCurItem->uPara<=7) //Input 1-Input8
					{
					temp= EepromReadByte(ASSIGNED_INPUT1+pMenuCurItem->uPara);
					if(pMenuCurItem->uPara==0) //Input1
						{
						if(temp==0) //XLR Digital
							return;
						}
					else //Input2-Input8
						{
						if((temp==0)||(temp==1)) //Toslink Coaxial 
							return;
						} 
					}
				}
			//add by cjm 2009-03-31
			
			bCurOptionIndex = read_curr_option(pMenuCurItem);
			if(bCurOptionIndex)
				{
				update = 1;
				bCurOptionIndex--;
				}

			write_curr_option(pMenuCurItem, bCurOptionIndex);
			execute_curr_option(pMenuCurItem, pMenuCurItem->pOptionArray[bCurOptionIndex].uValue);
			break;
		case BALANCE_ITEM_TYPE:
		case DELAY_DISTANCE_ITEM_TYPE:
		case VOLUME_ITEM_TYPE:
		case AV_DELAY_ITEM_TYPE:
			//add by cjm 2009-03-31
			if(pMenuCurPage->uID==SET_ANALOG_LEVEL_PAGE) //用于Not available 
				{
				if(pMenuCurItem->uPara<=7) //Input 1-Input8
					{
					temp= EepromReadByte(ASSIGNED_INPUT1+pMenuCurItem->uPara);
					if(pMenuCurItem->uPara==0) //Input1
						{
						if(temp==0) //XLR Digital
							return;
						else //XLR Analog
							{
							temp1= EepromReadByte(ANALOG1_MODE_POSITION + pMenuCurItem->uPara);
							if(temp1==0) //By pass
								return;
							}
						}
					else //Input2-Input8
						{
						if((temp==0)||(temp==1)) //Toslink Coaxial
							return;
						else //Analog
							{
							temp1= EepromReadByte(ANALOG1_MODE_POSITION + pMenuCurItem->uPara);
							if(temp1==0) //By pass
								return;
							}
						}
					}
				} 
			//add by cjm 2009-03-31
			
			bCurOptionIndex = read_curr_option(pMenuCurItem);
			if(bCurOptionIndex)
				{
				update = 1;
				bCurOptionIndex--;
				}

			write_curr_option(pMenuCurItem, bCurOptionIndex);
			execute_curr_option(pMenuCurItem, bCurOptionIndex);
			break;
		case STRING_ITEM_TYPE:
			if(InNameChanging) /*修改输入通道名字,光标左移*/
				{
				FlashPosition = execute_curr_option(pMenuCurItem, _NAME_LEFT_KEY);
				MsgSetFlash(FLASH_START,VFD_MAX_POSITION - FlashPosition,FLASH_CHARACTER);
				update = 1;
				}
			break;
		default:
			break;
		}

	if(update)
		MenuDisplay(pMenuCurPage,bCusrPos);
}

void ProcSetupRightKey()
{
	unsigned char update = 0;
	unsigned char temp;
	unsigned char temp1; 	

	switch(pMenuCurItem->uType)
		{
		case GENERAL_ITEM_TYPE:
		case RESTRICTED_ITEM_TYPE:
			//add by cjm 2009-03-31
			if(pMenuCurPage->uID==SET_ANALOG_IN_PAGE)
				{
				if(pMenuCurItem->uPara<=7) //Input 1-Input8
					{
					temp= EepromReadByte(ASSIGNED_INPUT1+pMenuCurItem->uPara);
					if(pMenuCurItem->uPara==0) //Input1
						{
						if(temp==0) //XLR Digital
							return;
						}
					else //Input2-Input8
						{
						if((temp==0)||(temp==1)) //Toslink Coaxial
							return;
						}
					}
				}
			//add by cjm 2009-03-31
			
			bCurOptionIndex = read_curr_option(pMenuCurItem);
			if(bCurOptionIndex < (pMenuCurItem->uOptionNum - 1))
				{
				update = 1;
				bCurOptionIndex++;
				}
			else
				{
				bCurOptionIndex = pMenuCurItem->uOptionNum -1;
				}

			write_curr_option(pMenuCurItem, bCurOptionIndex);
			execute_curr_option(pMenuCurItem,pMenuCurItem->pOptionArray[bCurOptionIndex].uValue);
			break;
		case BALANCE_ITEM_TYPE:
		case DELAY_DISTANCE_ITEM_TYPE:
		case VOLUME_ITEM_TYPE:
		case AV_DELAY_ITEM_TYPE:
			//add by cjm 2009-03-31
			if(pMenuCurPage->uID==SET_ANALOG_LEVEL_PAGE) //用于Not available 
				{
				if(pMenuCurItem->uPara<=7) //Input 1-Input8
					{
					temp= EepromReadByte(ASSIGNED_INPUT1+pMenuCurItem->uPara);
					if(pMenuCurItem->uPara==0) //Input1
						{
						if(temp==0) //XLR Digital
							return;
						else //XLR Analog
							{
							temp1= EepromReadByte(ANALOG1_MODE_POSITION + pMenuCurItem->uPara);
							if(temp1==0) //By pass
								return;
							}
						}
					else //Input2-Input8
						{
						if((temp==0)||(temp==1)) //Toslink Coaxial
							return;
						else //Analog
							{
							temp1= EepromReadByte(ANALOG1_MODE_POSITION + pMenuCurItem->uPara);
							if(temp1==0) //By pass
								return;
							}
						}
					}
				}
			//add by cjm 2009-03-31

			bCurOptionIndex = read_curr_option(pMenuCurItem);
			if(bCurOptionIndex < (pMenuCurItem->uOptionNum - 1))
				{
				update = 1;
				bCurOptionIndex++;
				}
			else
				{
				bCurOptionIndex = pMenuCurItem->uOptionNum -1;
				}

			write_curr_option(pMenuCurItem, bCurOptionIndex);
			execute_curr_option(pMenuCurItem, bCurOptionIndex);
			break;
		case STRING_ITEM_TYPE:
			if(InNameChanging) /*修改输入通道名字,光标右移*/
				{
				FlashPosition = execute_curr_option(pMenuCurItem, _NAME_RIGHT_KEY);
				MsgSetFlash(FLASH_START,VFD_MAX_POSITION - FlashPosition,FLASH_CHARACTER);
				update = 1;
				}
			break;
		default:
			break;
		}

	if(update)
		MenuDisplay(pMenuCurPage,bCusrPos);
}

void ProcSetupEnterKey()  
{
	uchar update = 0;
	
	switch(pMenuCurItem->uType)
		{
		case GOTO_PAGE_TYPE:
			if((pMenuCurPage->uID == SPK_BALANCE_PAGE) || (pMenuCurItem->pOptionArray[0].uValue == SPK_BALANCE_PAGE))
				{
				SpkBlanceTestMode(EXECUTE_MODE,0,1);	//把TEST MODE设置为OFF
				//EepromWriteByte(BALANCE_TEST_MODE_POSITION, 0);	//把TEST MODE设置为OFF
				}
			update = execute_curr_option(pMenuCurItem, pMenuCurItem->pOptionArray[0].uValue);

			if((pMenuCurPage->uID==LISTENPROFILE_PAGE)&&(pMenuCurItem->pOptionArray[0].uValue==LIS_PRO_MOVIE_PAGE))
				{
				Movie_Music_Speaker_Flag=0;
				ChangeListenProfileMode(); 
				}
			break;		
			
		case NONE_OPTION_ITEM_TYPE:
			update = execute_curr_option(pMenuCurItem, 0);
			break;
		case STRING_ITEM_TYPE:
			if(InNameChanging)	/*按enter结束name的修改*/
				{
				InNameChanging = 0;
				write_curr_option(pMenuCurItem, 0);	/*保存到EEPROM*/
				update = 1;
				//MenuDisplay(pMenuCurPage,bCusrPos);
				DeleteRemoteRepeatKey(_IR_ARROW_DOWN_KEY);
				DeleteRemoteRepeatKey(_IR_ARROW_UP_KEY);
				}
			else  /*按enter开始修改name */
				{
				InNameChanging = 1;
				FlashPosition = execute_curr_option(pMenuCurItem, _NAME_ENTER_KEY);
				MsgSetFlash(FLASH_START,VFD_MAX_POSITION - FlashPosition, FLASH_CHARACTER);
				AddRemoteRepeatKey(_IR_ARROW_DOWN_KEY);
				AddRemoteRepeatKey(_IR_ARROW_UP_KEY);
				}
			break;
		case PASSWORD_ITEM_TYPE:
			if(InPwdInput)
				{			
				InPwdInput = 0;
				update = 1;
				//MenuDisplay(pMenuCurPage,bCusrPos);
				}
			else
				{
				InPwdInput = 1;
				PwdInputState = ENTER_OLD_PWD;
				VfdInitDispPwd("Enter Old Password:");
				}
			break;
		}
	
	if(update)
		MenuDisplay(pMenuCurPage,bCusrPos);	
}

SETUP_T SetupExitMenu(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	bMode = 0;
	bValue = 0;
	
	SetupExit();
#ifdef _DEBUG_MENU
	debug_printf("\n****SetupExitMenu****\n");
#endif

	MsgSetVfdShow(GetCurFocusTid(),0);
	return SUCCESS;
}


void setupload_memroy1(uchar memory_mode)
{

unsigned char i,j,tmp;

	 if(memory_mode == 0)
	 	 i = 2;
	 else
	 	i =  3;
	 
	for(j = 0; j < MenuPageList[i].uItemNum; j++) /*10个主要菜单列表的子菜单数*/
		{
		switch(MenuPageList[i].pItemArray[j].uType) /*10个中子菜单类型,其中一部分类型不需要加载*/
			{
			case GENERAL_ITEM_TYPE:
				tmp = read_curr_option(&(MenuPageList[i].pItemArray[j])); /*执行主菜单中的子菜单的action	函数读取EEPROM 中的值, 即该子菜单设置的值*/				
				if(1)
					{
					tmp = MenuPageList[i].pItemArray[j].pOptionArray[tmp].uValue; /*子菜单选项值(0X00 or 0x01) */
					execute_curr_option(&(MenuPageList[i].pItemArray[j]), tmp); /*参数:主菜单中的子菜单、子菜单选项值*/
					}
				break;
			case BALANCE_ITEM_TYPE:
			case DELAY_DISTANCE_ITEM_TYPE:
			case AV_DELAY_ITEM_TYPE:
			case STRING_ITEM_TYPE:
			case VOLUME_ITEM_TYPE:
				tmp = read_curr_option(&(MenuPageList[i].pItemArray[j]));
				if(1)
					{
					execute_curr_option(&(MenuPageList[i].pItemArray[j]), tmp);
					}
				break;
			}
		}
}

//加载Eeprom 里的值
extern unsigned char Movie_Music_Speaker_Flag; 
void SetupLoadEeprom(unsigned char exec)
{
	unsigned char i,j,tmp;
	
	for(i = 0; i < MENUPAGELIST_SIZE; i++) /*共27个菜单列表*/
		{
		if((i>9)&&(i<17))
			Movie_Music_Speaker_Flag=1; /*设置MUSIC MODE default*/			
		else
			Movie_Music_Speaker_Flag=0; /*设置Movie MODE default*/	
		
		if(MenuPageList[i].uType == OPTION_TYPE) /*27 个主要菜单列表都是OPTION_TYPE */
			{
			for(j = 0; j < MenuPageList[i].uItemNum; j++) /*27个主要菜单列表的子菜单数*/
				{
				switch(MenuPageList[i].pItemArray[j].uType) /*27个中子菜单类型,其中一部分类型不需要加载*/
					{
					case GENERAL_ITEM_TYPE:
						tmp = read_curr_option(&(MenuPageList[i].pItemArray[j])); /*执行主菜单中的子菜单的action	函数读取EEPROM 中的值, 即该子菜单设置的值*/				
						if(exec)
							{
							tmp = MenuPageList[i].pItemArray[j].pOptionArray[tmp].uValue; /*子菜单选项值(0X00 or 0x01) */
							execute_curr_option(&(MenuPageList[i].pItemArray[j]), tmp); /*参数:主菜单中的子菜单、子菜单选项值*/
							}
						break;
					case BALANCE_ITEM_TYPE:
					case DELAY_DISTANCE_ITEM_TYPE:
					case AV_DELAY_ITEM_TYPE:
					case STRING_ITEM_TYPE:
					case VOLUME_ITEM_TYPE:
						tmp = read_curr_option(&(MenuPageList[i].pItemArray[j]));
						if(exec)
							{
							execute_curr_option(&(MenuPageList[i].pItemArray[j]), tmp);
							}
						break;
					}
				}
			}
		}

	for(i = 0; i < 10; i++) /*共10个菜单列表--目的重新加载前10个菜单列表-默认为Movie Mode*/
		{
		Movie_Music_Speaker_Flag=0; /*设置Movie MODE default*/	
		
		if(MenuPageList[i].uType == OPTION_TYPE) /*10个主要菜单列表都是OPTION_TYPE */
			{
			for(j = 0; j < MenuPageList[i].uItemNum; j++) /*10个主要菜单列表的子菜单数*/
				{
				switch(MenuPageList[i].pItemArray[j].uType) /*10个中子菜单类型,其中一部分类型不需要加载*/
					{
					case GENERAL_ITEM_TYPE:
						tmp = read_curr_option(&(MenuPageList[i].pItemArray[j])); /*执行主菜单中的子菜单的action	函数读取EEPROM 中的值, 即该子菜单设置的值*/				
						if(exec)
							{
							tmp = MenuPageList[i].pItemArray[j].pOptionArray[tmp].uValue; /*子菜单选项值(0X00 or 0x01) */
							execute_curr_option(&(MenuPageList[i].pItemArray[j]), tmp); /*参数:主菜单中的子菜单、子菜单选项值*/
							}
						break;
					case BALANCE_ITEM_TYPE:
					case DELAY_DISTANCE_ITEM_TYPE:
					case AV_DELAY_ITEM_TYPE:
					case STRING_ITEM_TYPE:
					case VOLUME_ITEM_TYPE:
						tmp = read_curr_option(&(MenuPageList[i].pItemArray[j]));
						if(exec)
							{
							execute_curr_option(&(MenuPageList[i].pItemArray[j]), tmp);
							}
						break;
					}
				}
			}
		}
 
	sys_flag.zone1_channel = EepromReadByte(ZONE1_OUT_POSITION);
}
//
//开机加载默认值
void SetupLoadDefault(unsigned char exec)
{
	unsigned char i,j,tmp;
	
	for(i = 0; i < MENUPAGELIST_SIZE; i++) /*共27个菜单列表*/
		{
		if((i>9)&&(i<17))
			Movie_Music_Speaker_Flag=1; /*设置MUSIC MODE default*/			
		else
			Movie_Music_Speaker_Flag=0; /*设置Movie MODE default*/	
				
		if(MenuPageList[i].uType == OPTION_TYPE) /*27个单列表全部为OPTION_TYPE*/
			{  
			for(j = 0; j < MenuPageList[i].uItemNum; j++)
				{
				switch(MenuPageList[i].pItemArray[j].uType) /*GOTO_PAGE_TYPE\NONE_OPTION_ITEM_TYPE\PASSWORD_ITEM_TYPE这三种不加载默认值*/
					{
					case GENERAL_ITEM_TYPE: 
					case STRING_ITEM_TYPE:
					case DELAY_DISTANCE_ITEM_TYPE:
					case AV_DELAY_ITEM_TYPE:
					case VOLUME_ITEM_TYPE:
					case BALANCE_ITEM_TYPE:
						tmp = load_default_option(&(MenuPageList[i].pItemArray[j]));
						write_curr_option(&(MenuPageList[i].pItemArray[j]), tmp);
						if(exec)
							execute_curr_option(&(MenuPageList[i].pItemArray[j]), tmp);
						break;
					}
				}
			}
		}

	WritePwd(DEFAULT_PWD); 
	sys_flag.zone1_volume = volume_table[DEFAULT_ZONE1_VOLUME];
	sys_flag.zone1_channel = DEFAULT_INPUT_CHANNEL;

	if(exec)
		{
		MsgSetVolume(ZONE1_ID,MODE_FIXED,sys_flag.zone1_volume);
		}
}

KEYT PwdKeyService(KEYT key)
{
	if(!InPwdInput)
		return key;

	switch(key)
		{
		case _REMOTE_DIGI_0_KEY:
		case _REMOTE_DIGI_1_KEY:
		case _REMOTE_DIGI_2_KEY:
		case _REMOTE_DIGI_3_KEY:
		case _REMOTE_DIGI_4_KEY:
		case _REMOTE_DIGI_5_KEY:
		case _REMOTE_DIGI_6_KEY:
		case _REMOTE_DIGI_7_KEY:
		case _REMOTE_DIGI_8_KEY:
		case _REMOTE_DIGI_9_KEY:
			current_pwd <<= 4;
			current_pwd |= key - _REMOTE_DIGI_0_KEY;
			VfdDisplay(VFD_LINE2_START_POSITION + PwdCounter,VFD_UPDATE_LINE2,"*"); /*输密码时显示**/
			PwdCounter ++;
			if(PwdCounter == PASSWORD_LENGTH) /*密码固定为4位*/
				{
#ifdef _DEBUG_MENU
				debug_printf("PWD:%04X,%04X\n",current_pwd,user_pwd);
#endif
				switch(PwdInputState)
					{
					case ENTER_OLD_PWD: /*输入旧的密码*/
						if((current_pwd == UNIVERSAL_PWD) || (current_pwd == user_pwd)) /*输入的旧密码正确*/
							{
							VfdInitDispPwd("Enter New Password:");
							PwdInputState = ENTER_NEW_PWD; /*输入的旧密码正确后,输入新密码*/
							}
						else /*输入的旧密码错误*/
							{
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Password error!");
							DelayMs(1000);
							current_pwd = 0;
							InPwdInput = 0;
							MenuDisplay(pMenuCurPage,bCusrPos);
							}
						break;
					case ENTER_NEW_PWD: /*输入新密码*/
						PwdInputState = REENTER_NEW_PWD;
						new_pwd = current_pwd;
						VfdInitDispPwd("ReEnter Password:");
						break;
					case REENTER_NEW_PWD: /*再一次输入新密码*/
						if(new_pwd == current_pwd) /*两次新密码一样,则新密码设置成功*/
							{
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Password Set!");
							user_pwd = current_pwd;
							WritePwd(current_pwd);
							}
						else /*两次输入的新密码不一样*/
							{
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Password Mismatch!");
							}
						DelayMs(1000);
						current_pwd = 0;
						InPwdInput = 0;
						MenuDisplay(pMenuCurPage,bCusrPos);
						break;
					case CHECK_SETUP_PWD: /*检查输入的密码是否正确*/
						if((current_pwd == UNIVERSAL_PWD) || (current_pwd == user_pwd)) /*正确进入设置菜单*/
							{
							SetupMenuInit();
							}
						else /*否则退出设置菜单*/
							{
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Password error!");
							DelayMs(500);
							SetupExitMenu(0,0,0);
							}
						break;
					}
				}
			break;
		case _REMOTE_SETUP_KEY:
		case _PANEL_ENTER_KEY:
			SetupExitMenu(0,0,0);
			break;
		}
	return _NO_KEY;
}

KEYT SetupKeyService(KEYT key)
{
	key = PwdKeyService(key); /*密码设置按键操作*/

	switch(key)
		{
		case _REMOTE_ARROW_UP_KEY:	
		case _PANEL_ARROW_UP_KEY:			
#ifdef _DEBUG_MENU			
			debug_printf("\n key up\n");
#endif		
			ProcSetupUpKey();
			return _NO_KEY;
			
		case _REMOTE_ARROW_DOWN_KEY:
		case _PANEL_ARROW_DOWN_KEY:
#ifdef _DEBUG_MENU
			debug_printf("\nkey down\n");
#endif	
			ProcSetupDownKey();
			return _NO_KEY;
			
		case _REMOTE_ARROW_LEFT_KEY:
		case _PANEL_ARROW_LEFT_KEY:
		case _PANEL_ENCODER_MINUS_KEY:
#ifdef _DEBUG_MENU
			debug_printf("\nkey LEFT\n");
#endif		
			if(pMenuCurPage->uID == MAIN_PAGE)
				ProcSetupUpKey();
			else
				ProcSetupLeftKey();
			return _NO_KEY;
			
		case _REMOTE_ARROW_RIGHT_KEY:
		case _PANEL_ARROW_RIGHT_KEY:
		case _PANEL_ENCODER_PLUS_KEY:			
#ifdef _DEBUG_MENU
			debug_printf("\nkey Right\n");
#endif			
			if(pMenuCurPage->uID == MAIN_PAGE)
				ProcSetupDownKey();
			else
				ProcSetupRightKey();
			return _NO_KEY;
			
		case _REMOTE_ENTER_KEY:
		case _PANEL_ENTER_KEY:
#ifdef _DEBUG_MENU
			debug_printf("\nkey enter\n");
#endif			
			ProcSetupEnterKey();
			return _NO_KEY;
			
		case _REMOTE_SETUP_KEY:
#ifdef _DEBUG_MENU
			debug_printf("\nkey setup\n");
#endif			
			SetupExitMenu(0,0,0);
			return _NO_KEY;
			
		case _REMOTE_POWER_OFF_KEY:
		case _PANEL_POWER_KEY:		
#ifdef _DEBUG_MENU
			debug_printf("\npower off\n");
#endif	
			SetupExit();
			return key;
			
		default:
			return _NO_KEY;
		}
}

#define FLASH_TIME1 400
#define FLASH_TIME2 200
static short flash_time_cnter = 0;
uchar disp_mode = 0;
void SetupFlashHandler(void)
{
	if(InNameChanging)
		{
		if(flash_time_cnter > 0)
			{
			flash_time_cnter --;
			if(flash_time_cnter == 0)
				{
				MsgSetFlash(FLASH_CONT,0,0);
				if(disp_mode)
					flash_time_cnter = FLASH_TIME1;
				else
					flash_time_cnter = FLASH_TIME2;
				}
			}
		}
}

void UpdateVfdFlash(uchar mode, uchar position, uchar value)
{
	static char bak_char,normal_char;
	static uchar current_position;

	if(!InNameChanging)
		return ;

	switch(mode)
		{
		case FLASH_START:
			flash_time_cnter = FLASH_TIME1;
			disp_mode = 0;
			current_position = position;
			normal_char = value;
			MenuDisplay(pMenuCurPage,bCusrPos);
			bak_char = GetVfdbufferChar(current_position);
			flash_time_cnter = FLASH_TIME1;
			break;
		case FLASH_CONT:
			if(disp_mode)
				{
				disp_mode = 0;
				VfdDisplay(current_position, VFD_UPDATE_LINE2, "%c", bak_char);
				}
			else
				{
				disp_mode = 1;
				VfdDisplay(current_position, VFD_UPDATE_LINE2, "%c", normal_char);
				}
			break;
		}
}

TASK_RESULT SetupProcMessage(unsigned char *buffer)
{
	KEYT comm_key;
	switch(buffer[0])
		{
		case MSG_KeyPress:
			comm_key = buffer[1];
			ProcDimerOff(comm_key);
			comm_key = SetupKeyService(comm_key);
			if(comm_key != _NO_KEY)
				{
				return default_proc(buffer);
				}
			return TRUE;
		case MSG_Flash:
			UpdateVfdFlash(buffer[1],buffer[2],buffer[3]); 
			return TRUE;
		}
	return FALSE;
}

