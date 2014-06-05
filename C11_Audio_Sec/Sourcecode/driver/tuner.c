/* @file 		Tuner.c
 * @brief 		HD_Radio Driver File
 *
 * @author 	chenjianming
 * @date 	  	2008-03-27
 * @version 	1.0
 * @note         该HD_Radio 模块的调试就是按照该模块的串口协议
                     把相应的串口命令发送给该模块就可以实现
                     相应的功能。(只要串口初始化好后,把相应的
                     命令发送给该模块就可以控制这个模块了)
 */
#include "Tuner.h"
#include "..\driver\uart.h"
#include "..\main\Macro.h"
#include "..\main\vfd_disp.h"
#include "..\main\Radio_task.h"
#include "..\main\queue.h"
#include "..\main\Debug.h"

#ifdef _DEBUG  
#define _DEBUG_TUNER 
#endif

extern void DelayMs(int delay_time);

/*
* @brief This command is used to set monitoring or request the SIS and PSD data.
*
* @author chenjianming
* @data    2008-3-31
* @Para :
*	      SIS_Flag=1--可以返回HD Radio 中CMD_IND_SIS_CHANGE 信息包
*	      PSD_Flag=1--可以返回HD Radio 中CMD_IND_PSD_CHANGE 信息包
* @note:
*           对于checksum 的计算,Datasheet有说明。
*/
void Tuner_Req_Radio_Monitor_Command(unsigned char Radio_Service,unsigned char SIS_Flag,unsigned char PSD_Flag)
{
	unsigned char Para_Radio_Service,Para_SIS,Para_PSD,Check_Sum; 
		
	Uart1Send(0x5A); Uart1Send(0xA5); //Head  
	Uart1Send(0x00); Uart1Send(0x04); //Command Length
	Uart1Send(0x01); //command

	if(Radio_Service)
		Para_Radio_Service=0x01; //Request
	else
		Para_Radio_Service=0x00; //Set monitor
	Uart1Send(Para_Radio_Service); 
	
	if(SIS_Flag)
		Para_SIS=0x01; //True
	else
		Para_SIS=0X00; //Flase
	Uart1Send(Para_SIS);
	
	if(PSD_Flag)	
		Para_PSD=0X01; //True
	else
		Para_PSD=0X00; //Flase
	Uart1Send(Para_PSD);

	if(Para_Radio_Service==1) //clac check_sum
		{
		if(SIS_Flag!=PSD_Flag)
			Check_Sum=0x01;
		else
			Check_Sum=0x00;		
		}
	else
		{
		if(SIS_Flag!=PSD_Flag)
			Check_Sum=0x00;
		else
			Check_Sum=0x01;		  
		}
		
	Uart1Send(Check_Sum); //check sum
}

/*
* @brief This command is used to change Band
*
* @author chenjianming
* @data    2008-3-27
*/
void Tuner_Band_Command(unsigned char band)
{
	unsigned char command_para;
	unsigned char check_sum;

	if(band==Band_FM)
		{
		command_para=0x01;
		check_sum=0x03;
		}
	else
		{
		command_para=0x00;  
		check_sum=0x02;		
		}
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x02); //Command length
	Uart1Send(0x02); //Command
	Uart1Send(command_para);
	Uart1Send(check_sum);
}

/*
* @brief This command is used to tune up frequency
*
* @author chenjianming
* @data  2008-3-31
* @para 
*            False: Tune to the next frequency
*                      (FM-200KHZ step  AM-10KHz step)
*            True: Tune to the next available sub channel
*                     If none ,tune to the next frequency
*/
void Tuner_Req_Tune_Up(unsigned char mode)
{
	unsigned char command_para;
	unsigned char check_sum;
	
	if(mode==Tune_Up_FALSE) //Flase
		{
		command_para=0x00;
		check_sum=0x06;
		}
	else //True
		{
		command_para=0x01;
		check_sum=0x07;
		}
	
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x02); //Command length
	Uart1Send(0x06); //Command
	Uart1Send(command_para);
	Uart1Send(check_sum);
}

/*
* @brief This command is used to tune down frequency
*
* @author chenjianming
* @data  2008-3-31
* @para 
*            False: Tune to the previous frequency
*                      (FM-200KHZ step  AM-10KHz step)
*            True: Tune to the previous available sub channel
*                     If none ,tune to the previous frequency
*/
void Tuner_Req_Tune_Down(unsigned char mode)
{
	unsigned char command_para;
	unsigned char check_sum;
	
	if(mode==Tune_Up_FALSE)
		{
		command_para=0x00;
		check_sum=0x07;
		}
	else
		{
		command_para=0x01;
		check_sum=0x06;
		}
	
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x02); //Command length
	Uart1Send(0x07); //Command
	Uart1Send(command_para);
	Uart1Send(check_sum);
}

/*
* @brief stop tune seeking command
*
* @author chenjianming
* @data  2008-3-31
*/
void Tuner_Stop_Seek(void)
{
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x01); //Command Length
	Uart1Send(0x0A); //Command
	Uart1Send(0X0A); //Checksum
}

/*
* @brief This command is used to seek up or down
*
* @author chenjianming
* @data  2008-3-31
*/
void Tuner_Seek_Command(unsigned char direction)
{
	unsigned char command_para;
	unsigned char check_sum;

	if(direction==Seek_Up)
		{
		command_para=0x00;
		check_sum=0x08;
		}
	else
		{
		command_para=0x01;
		check_sum=0x09;		
		}
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x02); //Command Length
	Uart1Send(0x08); //Command
	Uart1Send(command_para);
	Uart1Send(check_sum);
}

/*
* @brief This command is used to seek frequency with HD Radio signal
*
* @author chenjianming
* @data  2008-3-31
*/
void Tuner_HDSEEK_Command(unsigned char direction)
{
	unsigned char command_para;
	unsigned char check_sum;

	if(direction==Seek_Up)
		{
		command_para=0x00;
		check_sum=0x09;
		}
	else
		{
		command_para=0x01;
		check_sum=0x08;		
		}
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x02); //Command Length
	Uart1Send(0x09); //Command
	Uart1Send(command_para);
	Uart1Send(check_sum);
}

/*
* @brief mute control command
*
* @author chenjianming
* @data    2008-3-31
*/
void Tuner_Mute_Command(unsigned char mute)
{
	unsigned char command_para;
	unsigned char check_sum;

	if(mute==1) //MUTE
		{
		command_para=0x01;
		check_sum=0x0C;
		}
	else //UNMUTE
		{
		command_para=0x00;
		check_sum=0x0D;
		}
	
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x02); //Command length
	Uart1Send(0x0D); //Command 
	Uart1Send(command_para);
	Uart1Send(check_sum);
}

/*
* @brief Volume Change command
*
* @author chenjianming
* @data    2008-3-31
* @note 初始化为0DB
*/
void Tuner_Volume_Init(void)
{
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x02); //Command length
	Uart1Send(0x0B); //Command 
	Uart1Send(0x2F); //0x2F->0DB 
	Uart1Send(0x24); //check sum 	
}

/*
* @brief This command is used to reset the module
*
* @author chenjianming
* @data    2008-4-25
* @note The module will be reset in 50ms after sending the ACK packet
*/
void Tuner_Ext_Reset(void)
{
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x01);
	Uart1Send(0xFF); Uart1Send(0x00);
	Uart1Send(0xFF);
}

/*
* @brief This command is used to query the current frequency
*
* @author chenjianming
* @data    2008-3-31
*/
void Tuner_Req_Freq(void)
{
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x01); //Command Length
	Uart1Send(0xE3); //Command 
	Uart1Send(0xE3); //Checksum
}

/*
* @brief calculate the Freq checksum
*
* @author chenjianming
* @data    2008-3-31
*/
unsigned char FreqCheckSum(unsigned char command,unsigned char Freq_H,unsigned char Freq_L)
{
	unsigned char checksum=0;

	checksum ^=command;
	checksum ^=Freq_H;
	checksum ^=Freq_L;

	return checksum;
}

/*
* @brief This command is used to setting tuner frequency
*
* @author chenjianming
* @data    2008-3-31
*/
void Tuner_Write_Freq_Command(unsigned char mode,unsigned int Tuner_Freq)
{
	unsigned long Freq;
	unsigned char Freq_H,Freq_L;
	unsigned char check_sum;

	if((mode==_FM_MODE)||(mode==_HD_FM_MODE))
		{
		Freq=Tuner_Freq/100;
		}
	else 
		{
		Freq=Tuner_Freq;
		}
	
	Freq_H=(Freq>>8 & 0xFF);
	Freq_L=Freq & 0xFF;
	check_sum=FreqCheckSum(0x03,Freq_H,Freq_L); //0X03-tuner Freq Command

#ifdef _DEBUG_TUNER
	//debug_printf("\nFreq=%d",Freq);
	//debug_printf("\nFreq_H=%X",Freq_H);
	//debug_printf(" Freq_L=%X\n",Freq_L);
	//debug_printf("check_sum=%X\n",check_sum);
#endif

	Uart1Send(0x5A);	 Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x03); //Command Length
	Uart1Send(0x03); //Command
	Uart1Send(Freq_H); Uart1Send(Freq_L); //Tuner Freq
	Uart1Send(check_sum); //Check Sum
}

/*
* @brief HD Radio Init
*
* @author chenjianming
* @data    2008-3-31
*/
extern unsigned char Tuner_Signal_Quality;
extern unsigned char Tuner_Reply_Flag;
extern unsigned int Tuner_Freq_H;
extern unsigned int Tuner_Freq_L;
void InitTuner(unsigned char mode)
{
#ifdef _DEBUG_TUNER
	//debug_printf("\nInit Tuner");
#endif

	switch(mode)
		{
			case _FM_MODE:
#ifdef _DEBUG_TUNER
				debug_printf("\nFM Init\n");
#endif				
				//MsgSetRadioLoadPreset(system_position); //dele by cjm 2008-11-3
				Tuner_Band_Command(Band_FM);
				DelayMs(30);				
				Tuner_Seek_Command(Seek_Up); /*用于AM/FM 与HDAM/HDFM 之间的却换*/
				DelayMs(30);
				Tuner_Stop_Seek();
				DelayMs(30); 
				Tuner_Write_Freq_Command(current_radio_mode,system_freq); 
				break;
			case _AM_MODE:
#ifdef _DEBUG_TUNER  
				debug_printf("\nAM Init\n");
#endif
				//MsgSetRadioLoadPreset(system_position); //dele by cjm 2008-11-3
				Tuner_Band_Command(Band_AM);		
				DelayMs(30);				
				Tuner_Seek_Command(Seek_Up); /*用于AM/FM 与HDAM/HDFM 之间的却换*/	
				DelayMs(30);
				Tuner_Stop_Seek();
				DelayMs(30);
				Tuner_Write_Freq_Command(current_radio_mode,system_freq); 
				break;
			case _HD_FM_MODE:
#ifdef _DEBUG_TUNER
				debug_printf("\nHDFM Init\n");
#endif
				Tuner_Band_Command(Band_FM);
				DelayMs(1000);
				Tuner_HDSEEK_Command(Seek_Down); /*解决刚切换到HDFM时电台跟显示不符的问题*/
				DelayMs(50);
				
				Tuner_Stop_Seek(); //add by cjm 2008-11-26
				DelayMs(50);      	
#if 0 //dele by cjm 2008-11-26
				HDFM_position=system_position; 
				
				Tuner_Reply_Flag=1;				
				Tuner_Req_Radio_Monitor_Command(1,1,0); /*向HD Radio 发送命令设置标志位读取SIS CHANGE 包*/	
				DelayMs(300); 
				do{
					C11_Tuner_SIS_Change();
					}while(Tuner_Reply_Flag);	
				system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);	
#endif				
				Tuner_Write_Freq_Command(current_radio_mode,system_freq);
				break;
			case _HD_AM_MODE:
#ifdef _DEBUG_TUNER
				debug_printf("\nHDAM Init\n");
#endif				
				Tuner_Band_Command(Band_AM);   
				DelayMs(1000);
				Tuner_HDSEEK_Command(Seek_Down); /*解决刚切换到HDFM时电台跟显示不符的问题*/ 
				DelayMs(50);
				
				Tuner_Stop_Seek(); //add by cjm 2008-11-26      
				DelayMs(50);    
#if 0 //dele by cjm 2008-11-26
				HDAM_position=system_position; 
				
				Tuner_Reply_Flag=1;				
				Tuner_Req_Radio_Monitor_Command(1,1,0); /*想HD Radio 发送命令设置标志位读取SIS CHANGE 包*/	
				DelayMs(300);
				do{
					C11_Tuner_SIS_Change();
					}while(Tuner_Reply_Flag);				
				system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);	
#endif				
				Tuner_Write_Freq_Command(current_radio_mode,system_freq);
				break;  
		}
}

#if 0
/*
* @brief HD Radio version command
*
* @author chenjianming
* @data  2008-3-31
*/
void Tuner_Version(void)
{
	Uart1Send(0x5A); Uart1Send(0xA5); //Head
	Uart1Send(0x00); Uart1Send(0x01); //Command Length
	Uart1Send(0x0C); //Command
	Uart1Send(0x0C); //Checksum
}
#endif

