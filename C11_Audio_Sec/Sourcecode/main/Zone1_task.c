#include "macro.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "..\driver\uart.h"
#include "..\driver\timer.h"
#include "..\driver\ioport.h"
#include "..\driver\ir.h"
#include "..\driver\channel.h"
#include "..\driver\dac.h"
#include "..\driver\cs8416.h"
#include "..\driver\mas9116.h"
#include "..\driver\eeprom.h"
#include "..\driver\max335.h"
#include "..\driver\volume.h"
#include "..\driver\vfddriver.h"
#include "..\driver\tuner.h"
#include "..\driver\Hdmi.h"

#include "..\cs495300\Dspdrv.h"
#include "..\cs495300\DSP_API.h"
#include "..\cs495300\DolbyProLogicIIX.h"
#include "..\cs495300\BassManager.h"
#include "..\cs495300\DelayManager.h"

#include "setup_menu.h"
#include "setup_func.h"
#include "vfd_disp.h"
#include "key_task.h"
#include "keydefine.h"
#include "task.h"
#include "message.h"
#include "debug.h"
#include "queue.h"
#include "rs232_protocol.h"
#include "c11_msg_handler.h"
#include "radio_task.h"
//#include "fir.h"

#ifdef _DEBUG
#define _DEBUG_ZONE1_TASK
#endif

extern void AutoSoundInit(void);
extern unsigned char PcmType;
extern unsigned char DSDType;
extern unsigned char stream_type;   
extern unsigned char Movie_Music_Speaker_Flag; 
extern unsigned char CD_Pure_Audio; // jian add in 12-03-06
extern unsigned char CD_8416_channel ;  //jian add in 12-03-08

short temp_dimer_time = 0;
unsigned char Dolby2ch_Flag=0; /*如果流为Dolby 2ch 则默认为用PL2解码*/

char * Zone1SrcName;

char SrcNameInput1[INPUT_NAME_LENGTH+1]="INPUT 1";// = "INPUT 1 "; 
char SrcNameInput2[INPUT_NAME_LENGTH+1]="INPUT 2";// = "INPUT 2 ";
char SrcNameInput3[INPUT_NAME_LENGTH+1]="INPUT 3";// = "INPUT 3 ";
char SrcNameInput4[INPUT_NAME_LENGTH+1]="INPUT 4";// = "INPUT 4 ";
char SrcNameInput5[INPUT_NAME_LENGTH+1]="INPUT 5";// = "INPUT 5 ";
char SrcNameInput6[INPUT_NAME_LENGTH+1]="INPUT 6";// = "INPUT 6 ";
char SrcNameInput7[INPUT_NAME_LENGTH+1]="INPUT 7";// = "INPUT 7 ";
char SrcNameInput8[INPUT_NAME_LENGTH+1]="INPUT 8";// = "INPUT 8 ";
char SrcNameCh71[INPUT_NAME_LENGTH+1]="INPUT 7.1";// = "7.1CH IN";
char SrcNameInputHDMI1[INPUT_NAME_LENGTH+1]="HDMI 1"; //="HDMI 1"
char SrcNameInputHDMI2[INPUT_NAME_LENGTH+1]="HDMI 2"; //="HDMI 2"
char SrcNameInputHDMI3[INPUT_NAME_LENGTH+1]="HDMI 3"; //="HDMI 1"
char SrcNameInputHDMI4[INPUT_NAME_LENGTH+1]="HDMI 4"; //="HDMI 2"
char SrcNameInputARC[INPUT_NAME_LENGTH+1]="TV SOUND"; //="HDMI 2"
char SrcNameInputHDMI[INPUT_NAME_LENGTH+1]="HDMI"; //="HDMI 2"
char C11Logo[PRODUCT_NAME_LENGTH+1];

const char * DefaultName[] = 
{
"INPUT 1     ",
"INPUT 2     ",
"INPUT 3     ",
"INPUT 4     ",
"INPUT 5     ",
"INPUT 6     ",
"INPUT 7     ",
"INPUT 8     ",
"7.1CH IN    ", 
"HDMI 1      ",
"HDMI 2      ",
"HDMI 3      ",
"HDMI 4      ",
"TV SOUND",
"HDMI"
};

char * SrcName[] = 
{
SrcNameInput1,
SrcNameInput2,
SrcNameInput3,
SrcNameInput4,
SrcNameInput5,
SrcNameInput6,
SrcNameInput7,
SrcNameInput8,
SrcNameCh71,
SrcNameInputHDMI1,
SrcNameInputHDMI2,
SrcNameInputHDMI3,
SrcNameInputHDMI4,
SrcNameInputARC,
SrcNameInputHDMI,
};

SYSTEM_FLAG sys_flag;

BOOL IsZone1VolumeValid(short volume)
{
	if((volume <= ZONE1_MAX_VOLUME) && (volume >= ZONE1_MIN_VOLUME))
		return TRUE;
	return FALSE;
}

BOOL MsgSetMute(TASK_ID t_id, unsigned char mode, unsigned char mute)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_Mute;
	buffer[1] = mode;
	buffer[2] = mute;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetStream(unsigned char type,uchar sub_type)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_Stream;
	buffer[1] = type;
	buffer[2] = sub_type;
	return SendMessage(ZONE1_ID,buffer);
}

BOOL MsgSetInputSource(TASK_ID t_id, unsigned char src)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_InputSource;
	buffer[1] = src;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetDSPSetting(TASK_ID t_id,unsigned char value)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_DSP_Setting;
	buffer[1]=value;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetDSPHDMISetting(TASK_ID t_id,unsigned char value)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_DSP_HDMI_Setting;
	buffer[1]=value;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetDspChannel(TASK_ID t_id,unsigned char value)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0]=MSG_DSP_Channel_Setting;
	buffer[1]=value;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetSoftMute(TASK_ID t_id,unsigned char value)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0]=MSG_Soft_Mute;
	buffer[1]=value;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetPower(TASK_ID t_id, unsigned char mode)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_Power;
	buffer[1] = mode;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetVfdShow(TASK_ID t_id,unsigned char mode)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_Show;
	buffer[1] = mode;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetDimer(unsigned char dimer_value,unsigned char mode)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_Dimer;
	buffer[1] = mode;
	buffer[2] = dimer_value;
	return SendMessage(ZONE1_ID,buffer);
}
//
KEYT _cur_key = _NO_KEY;
BOOL MsgSetKey(TASK_ID t_id,unsigned char key_mode,KEYT key_value)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};

	if(key_enable)
		{
		_cur_key = _NO_KEY;
		buffer[0] = MSG_KeyPress;
		buffer[1] = key_value;
		buffer[2] = key_mode;
		return SendMessage(t_id,buffer);
		}
	else
		{
		_cur_key = key_value;
		return FALSE;
		}
}

BOOL MsgSetVolume(TASK_ID t_id,unsigned char mode,short volume)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = Msg_Volume;
	buffer[1] = mode;
	buffer[2] = (volume >> 8) & 0xFF;
	buffer[3] = (volume) & 0xFF;
	return SendMessage(t_id,buffer);
}

BOOL MsgSetFlash(uchar mode, uchar position, uchar value)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_Flash;
	buffer[1] = mode;
	buffer[2] = position;
	buffer[3] = value;
	return SendMessage(SETUP_ID,buffer);
}

BOOL IsZone1PowerOn(void)
{
	if(sys_flag.system_zone1_status == POWER_ON)
		return TRUE;
	return FALSE;
}

static short RefreshTimer = 0;
static TASK_ID refresh_id;
void SetRefresh(TASK_ID t_id, short time)
//time 的单位是0.1秒
{
	refresh_id = t_id;
	RefreshTimer = time;
}

void RefreshHandle(void)
{
	if(RefreshTimer)
		{
		RefreshTimer --;
		if(RefreshTimer == 0)
			{
			MsgSetVfdShow(refresh_id,0);
			}
		}
}

void ShowVersion()
{
	short cnt = 50;
	key_enable = 0;
	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);
	VfdDisplay(VFD_LINE1_START_POSITION + 1,VFD_UPDATE_NONE,CaryLogo);
	VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,"%s  %s",C11_VERSION,__DATE__);
	do{
		DelayMs(100);
		if(_cur_key != _NO_KEY)
			{
			break;
			}
		}while(cnt--);
	key_enable = 1;
}

void VfdUpdateVolumeValue(short volume)
{
	if(IsCurFocusTid(ZONE1_ID) || IsCurFocusTid(RADIO_ID))
		VfdDisplay(VFD_LINE1_START_POSITION+17,VFD_UPDATE_LINE1,"%3d",volume);

	if(IsCurFocusTid(RADIO_ID))
		{
		SetRefresh(RADIO_ID,50);	/*在radio模式下，由于音量显示和preset显示共用一个VFD位置，故显示音量之后5秒要恢复到preset显示*/
		}
}

void VfdUpdateStreamType(unsigned char type,unsigned char sub_type)
{
	uchar decode_mode;
	uchar freq;
	
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nVfdUpdateStreamType\n");
	//debug_printf("\nsys_flag.src_mode=%d",sys_flag.src_mode);
	//debug_printf("\nsys_flag.zone1_channel=%d",sys_flag.zone1_channel);
	//debug_printf("\ntype=%d",type);
#endif  
	
	VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);
	if(sys_flag.zone1_mute_flag == ON)
		{
		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Mute");
		return;
		}
	if(sys_flag.src_mode == 2)
		{
		if(sys_flag.zone1_channel == INPUT_CH71)
			{
			VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Bypass");
			}
		else
			{
			VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Stereo");
			}
		return;
		}
	if(sys_flag.zone1_channel == INPUT_CH71) /*7.1 Analog 输入*/
		{
		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"PCM        48kHz");
		return;    
		}
	
	switch(type)
		{
		case STREAM_NONE:
			VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"NO SIGNAL");
			break;
		case STREAM_DSD:
			if(DSDType==_DSD_2CH) 
				{
				VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DSD 2.0             ");
				}
			else
				{
				VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DSD 5.1             ");
				}
			break;
		case STREAM_PCM:
		if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
				decode_mode = sys_flag.decode_mode.pcm_decode_mode;
				if(CD_8416_channel) // jian add in 12-03-08
					decode_mode  = PCM_MODE_NORMAL;				
				switch(PcmType)    
					{
					case _PCM_2_1:
						switch(decode_mode)
							{
							case PCM_MODE_NORMAL:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"PCM 2.0            "); 
								switch(GetNormalFreq())
								{
								case FREQ_44_1KHZ:
									VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2," 44.1kHz");
									break;
								case FREQ_48KHZ:
									VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2," 48kHz");
									break;
								case FREQ_96KHZ:
									VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2," 96kHz");
									break;
								case FREQ_192KHZ:
									VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2,"192kHz");
									break;
								} 
								break;
							case PCM_MODE_PL2:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY PL II");
								if(sys_flag.decode_mode.pcm_sur_mode)
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Movie");
								else
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Music");
								break;
							case PCM_MODE_PL2X:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY PL IIx");
								if(sys_flag.decode_mode.pcm_sur_mode)
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Movie");
								else
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Music");								
								break;
							case PCM_MODE_NEO6:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DTS NEO:6");
								if(sys_flag.decode_mode.pcm_sur_mode)
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Cinema");
								else
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Music");
								break;
							case PCM_MODE_CES:
								switch(sys_flag.decode_mode.pcm_sur_mode)    
									{  
									case 0:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON          ");
										break;
									case 1:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 All Stereo  ");
										break;
									case 2:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 All Mono    ");
										break;
									default:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON          ");
										break;
									}
								break;
							}			
						break;
					case _PCM_5_1:
						switch(decode_mode)
							{
							case PCM_MODE_NORMAL:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"PCM 5.1            ");
								break;
							case PCM_MODE_PL2X:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY PL IIx");
								if(sys_flag.decode_mode.pcm_sur_mode)
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Movie");
								else
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Music");		
								break;
							case PCM_MODE_NEO6:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DTS NEO:6");
								if(sys_flag.decode_mode.pcm_sur_mode)
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Cinema");
								else
									VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Music");
								break;
							case PCM_MODE_CES:
								switch(sys_flag.decode_mode.pcm_sur_mode)    
									{  
									case 0:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON          ");
										break;
									case 1:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 All Stereo  ");
										break;
									case 2:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 All Mono    ");
										break;
									default:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON          ");
										break;
									}
								break;
							}
						break;
					case _PCM_7_1:
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"PCM 7.1            ");
						break;
					default:
						break;	
					}
				}
			else
				{
				decode_mode = sys_flag.decode_mode.pcm_decode_mode;

				if(GetNormalFreq() == FREQ_192KHZ)	//192K不做其他效果
					{
					decode_mode = PCM_MODE_NORMAL;
					}
				//if((GetNormalFreq() == FREQ_96KHZ) && (decode_mode == PCM_MODE_NEO6))
				//	{
				//	decode_mode = PCM_MODE_NORMAL;
				//	}
				if(CD_8416_channel) // jian add in 12-03-08
					decode_mode  = PCM_MODE_NORMAL;

				switch(decode_mode)
					{
					case PCM_MODE_NORMAL:
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"PCM");
						if(sys_flag.src_mode == 1)
							{
							VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2," 48kHz");
							}
						else
							{
							switch(GetNormalFreq())
								{
								case FREQ_44_1KHZ:
									VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2," 44.1kHz");
									break;
								case FREQ_48KHZ:
									VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2," 48kHz");
									break;
								case FREQ_96KHZ:
									VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2," 96kHz");
									break;
								case FREQ_192KHZ:
									VfdDisplay(VFD_LINE2_START_POSITION+10,VFD_UPDATE_LINE2,"192kHz");
									break;
								}
							}
						break;
					case PCM_MODE_PL2:
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY PL II");
						if(sys_flag.decode_mode.pcm_sur_mode)
							VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Movie");
						else
							VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Music");
						break;
					case PCM_MODE_PL2X: 
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY PL IIx");
						if(sys_flag.decode_mode.pcm_sur_mode)
							VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Movie");
						else
							VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Music");
						break;
					case PCM_MODE_NEO6:
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DTS NEO:6");
						if(sys_flag.decode_mode.pcm_sur_mode)
							VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Cinema");
						else
							VfdDisplay(VFD_LINE2_START_POSITION+14,VFD_UPDATE_LINE2,"Music");
						break;  
					case PCM_MODE_CES:
						switch(sys_flag.decode_mode.pcm_sur_mode)    
							{  
							case 0:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON          ");
								break;
							case 1:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 All Stereo  ");
								break;
							case 2:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 All Mono    ");
								break;
							default:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON          ");
								break;
							}
						break;
					}
				}
			break;
			
		case STREAM_AC3:
			decode_mode = sys_flag.decode_mode.dolby_decode_mode;
			if(sys_flag.headphone_flag)
				{
				switch(sys_flag.decode_mode.dolby_type)
					{
					case _DOLBY_2CH:
					case _DOLBY_5CH:
					case _DOLBY_EX:
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY Headphone     ");
						break;
					case _DOLBY_PLUS: /*高清不支持Headphone*/
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY DIGITAL PLUS");
						break;
					case _DOLBY_TRUE_HD_2ch:
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD 2.0");
						break;
					case _DOLBY_TRUE_HD_5ch:
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD 3/2.1");
						break;
					case _DOLBY_TRUE_HD_6ch:
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD 3/3.1");
						break;
					case  _DOLBY_TRUE_HD_7ch: /*高清不支持Headphone*/
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD 3/4.1");
						break;
					default :
						break;
					}
				}
			else
				{
				switch(sys_flag.decode_mode.dolby_type)
					{
					case _DOLBY_2CH:
						switch(decode_mode)
							{
							case AC3_MODE_CES_ON:  
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON");
								switch(sys_flag.decode_mode.dolby_sur_mode)
									{
									case 0:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON");
										break;
									case 1:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 All Stereo");
										break;
									case 2:
										VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 All Mono");
										break;
									default:
										break;      
									}
								break;
							case AC3_MODE_PL2:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY PL II  ");
								if(sys_flag.decode_mode.dolby_sur_mode)
									VfdDisplay(VFD_LINE2_START_POSITION+13,VFD_UPDATE_LINE2,"Movie");
								else
									VfdDisplay(VFD_LINE2_START_POSITION+13,VFD_UPDATE_LINE2,"Music");
								break;
							case AC3_MODE_PL2X:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY PL IIx ");
								if(sys_flag.decode_mode.dolby_sur_mode)
									VfdDisplay(VFD_LINE2_START_POSITION+13,VFD_UPDATE_LINE2,"Movie");
								else
									VfdDisplay(VFD_LINE2_START_POSITION+13,VFD_UPDATE_LINE2,"Music");
								break;
							case AC3_MODE_NORMAL:
							default:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY DIGITAL 2.0");
								break;
							}
						break;
					case _DOLBY_5CH:
						switch(decode_mode)  
							{
							case AC3_MODE_CES_ON:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON");			
							break;
							case AC3_MODE_PL2X:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY PL IIx ");
								if(sys_flag.decode_mode.dolby_sur_mode)
									VfdDisplay(VFD_LINE2_START_POSITION+13,VFD_UPDATE_LINE2,"Movie");
								else
									VfdDisplay(VFD_LINE2_START_POSITION+13,VFD_UPDATE_LINE2,"Music");
								break;
							case AC3_MODE_EX_ON:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY DIGITAL EX ON");
								break;
							case AC3_MODE_EX_OFF:
								VfdTempDispBlank(VFD_LINE2,VFD_UPDATE_NONE);
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_NONE,"DOLBY DIGITAL 3/2.1");
								VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,50,"DOLBY DIGITAL EX OFF");
								sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL;
								break;
							case AC3_MODE_NORMAL:
							default:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY DIGITAL 3/2.1");
								break;
							}
						break;
					case _DOLBY_EX:
						switch(decode_mode)
							{
							case AC3_MODE_CES_ON:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON");					
								break;
							default:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY DIGITAL EX");
								break;
							}
						break;
					case _DOLBY_PLUS:
						//switch(decode_mode)  
							//{
							//case AC3_MODE_NORMAL:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY DIGITAL PLUS");
							//	break;
							//}
						break;
					case _DOLBY_TRUE_HD_2ch:
						switch(decode_mode)
							{
							case AC3_MODE_PL2X:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD + PLIIx"); 
								break;
							case AC3_MODE_NORMAL:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD 2.0");
								break;
							}
						break;
					case _DOLBY_TRUE_HD_5ch:
						switch(decode_mode)
							{
							case AC3_MODE_PL2X:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD + PLIIx"); 
								break;
							case AC3_MODE_NORMAL:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD 3/2.1");
								break;
							}	
						break;
					case _DOLBY_TRUE_HD_6ch:
						switch(decode_mode)
							{
							case AC3_MODE_PL2X:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD + PLIIx"); 
								break;
							case AC3_MODE_NORMAL:
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD 3/3.1");
								break;
							}	
						break;

					 case _DOLBY_TRUE_HD_7ch:
					 	VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"DOLBY TRUEHD 3/4.1");
					 	break;
					}
				}
			break;
		case STREAM_DTS:
			decode_mode = sys_flag.decode_mode.dts_decode_mode;			
			if(sys_flag.headphone_flag)
				decode_mode = DTS_MODE_NORMAL; //DTS 无Headphone 效果				
			switch(sub_type)
				{
				case _DTS:
					if(decode_mode == DTS_MODE_CES_ON)
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON");
					else
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts");
					break;
				case _DTS_ES_DISCRETE:
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-ES");
					//if(decode_mode == DTS_MODE_CES_ON)
					//	VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"CES 7.1 ON");
					//else
					VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"Discrete");
					break;
				case _DTS_ES_MATRIX:  
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-ES");
					//if(decode_mode == DTS_MODE_CES_ON)
					//	VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"CES 7.1 ON");
					//else
					VfdDisplay(VFD_LINE2_START_POSITION+7,VFD_UPDATE_LINE2,"Matrix");
					break;
				case _DTS_96_24:
					if(decode_mode == DTS_MODE_CES_ON)
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"CES 7.1 ON");
					else
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts 96/24");		
					break;
				case _DTS_HD_MA_48K:
					switch(sys_flag.decode_mode.dts_type)
						{
						case _DTS_HD_MA_48K_2ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA + PL IIx");
							else
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 2.0 48K");
							break;
						case _DTS_HD_MA_48K_5ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA + PL IIx");
							else
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 3/2.1 48K");
							break;
						case _DTS_HD_MA_48K_6ch:
						//	if(decode_mode ==DTS_MODE_PLIIX)
						//		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA + PL IIx");
						//	else
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 3/3.1 48K");
							break;

						case _DTS_HD_MA_48K_7ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 3/4.1 48K");
							break;
						default:
							break;
						}
					break;
				case _DTS_HD_MA_96k_192K:
					switch(sys_flag.decode_mode.dts_type)
						{
						case _DTS_HD_MA_96K_2ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA + PL IIx");
							else
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 2.0 96K");
							break;
						case _DTS_HD_MA_96K_5ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA + PL IIx");
							else
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 3/2.1 96K");
							break;
						case _DTS_HD_MA_96K_7ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 3/4.1 96K");
							break;
						case _DTS_HD_MA_192K_2ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 2.0 192K");
							break;
						case _DTS_HD_MA_192K_5ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 3/2.1 192K");
							break;
						case _DTS_HD_MA_192K_7ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD MA 3/4.1 192K");
							break;
						default:
							break;
						}
					break;
				case _DTS_HD_HRA_48K:
					switch(sys_flag.decode_mode.dts_type)
						{
						case _DTS_HD_HRA_48K_2ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD HRA 2.0 48K");
							break;
						case _DTS_HD_HRA_48K_6ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD HRA 3/2.1 48K");						
							break;
						case _DTS_HD_HRA_48K_7ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD HRA 3/3.1 48K");							
							break;
						case _DTS_HD_HRA_48K_8ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD HRA 3/4.1 48K");		
							break; 
						default:
							break;
						}
					break;
				case _DTS_HD_HRA_96K:
					switch(sys_flag.decode_mode.dts_type)
						{
						case _DTS_HD_HRA_96K_2ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD HRA 2.0 96K");
							break;
						case _DTS_HD_HRA_96K_6ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD HRA 3/2.1 96K");
							break;
						case _DTS_HD_HRA_96K_7ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD HRA 3/3.1 96K");
							break;
						case _DTS_HD_HRA_96K_8ch:
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"dts-HD HRA 3/4.1 96K");
							break;
						default:
							break;
						}
					break;
				}
			break;
		}
}

void VfdUpdateZone1()
{
	VfdDisplayBlank(VFD_LINE1,VFD_UPDATE_NONE);
	if(Movie_Music_Speaker_Flag == 0) // jian add in 10-11-18
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"M1: %s",Zone1SrcName);
	else
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"M2: %s",Zone1SrcName);
	VfdUpdateStreamType(sys_flag.stream_type,sys_flag.stream_subtype);
	VfdUpdateVolumeValue(sys_flag.zone1_volume);  
}
//
char *GetSrcName(unsigned char channel)
{
	char * rtn_name;
	switch(channel)
		{
		case INPUT_1:
			rtn_name = SrcNameInput1;
			break;
		case INPUT_2:
			rtn_name = SrcNameInput2;
			break;
		case INPUT_3:
			rtn_name = SrcNameInput3;
			break;
		case INPUT_4:
			rtn_name = SrcNameInput4;
			break;
		case INPUT_5:
			rtn_name = SrcNameInput5;
			break;
		case INPUT_6:
			rtn_name = SrcNameInput6;
			break;
		case INPUT_7:
			rtn_name = SrcNameInput7;
			break;
		case INPUT_8:
			rtn_name = SrcNameInput8;
			break;
		case INPUT_CH71:
			rtn_name = SrcNameCh71;  
			break;
		case INPUT_HDMI1:
			rtn_name=SrcNameInputHDMI1;
			break;
		case INPUT_HDMI2:
			rtn_name=SrcNameInputHDMI2;
			break;
		case INPUT_HDMI3: // jian add in 10-09-13
			rtn_name=SrcNameInputHDMI3;
			break;
		case INPUT_HDMI4:// jian add in 10-09-13
			rtn_name=SrcNameInputHDMI4;
			break;
		case INPUT_HDMI:// jian add in 10-09-13
			rtn_name=SrcNameInputHDMI;
                         break;
		case ARC_CHANNEL:
			rtn_name=SrcNameInputARC;
			break;
		default:
			break;
		}
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nrtn_name:%s",rtn_name);
#endif
	return rtn_name;
}

extern unsigned char PreDspMode;
extern unsigned char Pre_value;
//当时,哥相信牛哥的话,看到别人买房,都表示出很不屑,
const uchar sourece_talbe[]={INPUT_HDMI1,};

uchar SelectSource(uchar channel)
{
	uchar src_mode = 0;
	uchar input_channel;
	//unsigned char value;

	PreDspMode=8; //change by cjm 清掉DSP POWER MODE
	DSP_Select(0); // jian add in 12-03-02
        if(channel == ARC_CHANNEL) // JIAN ADD IN 10-08-06
        	{
		Pre_value=8; /*DSP 重新复位*/
		SetDspPower(0); /*DSP 保持复位状态*/	
        	Analog7CH1OutSelectChannel(SOURCE_DSP); /*DSP输出后从D/A输出*/
	//	if(channel != sys_flag.zone1_channel)
	        	HDMI_Port_Select(4); /*HDMI 板切换到ARC_CNNAEL 口*/	
		return 0;
        	}
		
	if(channel==INPUT_HDMI1) /*HDMI 1 Input*/
		{
	//	DelayMs(500);
#ifdef _DEBUG_ZONE1_TASK												
		debug_printf("\nHDMI1 Input");
#endif		
		Analog7CH1OutSelectChannel(SOURCE_DSP); /*DSP输出后从D/A输出*/
		HDMI_Port_Select(0); /*HDMI 板切换到HDMI1 口*/	

#if 0		
		switch(EepromReadByte(HDMI1_IN_AUDIO))
			{  
			case 0: //Bitstream
#ifdef _DEBUG_ZONE1_TASK												
				debug_printf("\nHDMI Input:Bitstream");
#endif
				Pre_value=8; /*DSP 重新复位*/
				SetDspPower(0); /*DSP 保持复位状态*/	
				break;
			case 1: //LPCM
#ifdef _DEBUG_ZONE1_TASK															
				debug_printf("\nHDMI Input:LPCM");  
#endif			
				SetDspPower(3); /*DSP 切换到Analog 口*/ 
				DSP_Input_Stream_Select(HDMI_Input); /*输入到DSP源--HDMI(I2S)--放到DSP引导起来后,否则DSP处于忙状态*/
				break;
			default:
				break;  
			}
		#else
				Pre_value=8; /*DSP 重新复位*/
				SetDspPower(0); /*DSP 保持复位状态*/	
		
#endif		
		src_mode=0;
		return src_mode;	
		}

	if(channel==INPUT_HDMI2) /*HDMI 2 Input*/
		{  
	//	DelayMs(500);
#ifdef _DEBUG_ZONE1_TASK												
		debug_printf("\nHDMI2 Input\n");
#endif
		Analog7CH1OutSelectChannel(SOURCE_DSP);  /*DSP输出后从D/A输出*/
		HDMI_Port_Select(1);  /*HDMI 板切换到HDMI2 口*/	
		Pre_value=8; /*DSP 重新复位*/
		SetDspPower(0); /*DSP 保持复位状态*/	
		src_mode=0;
		return src_mode;	  
		}		 


	if(channel==INPUT_HDMI3) /*HDMI 2 Input*/
		{  
	//	DelayMs(500);
#ifdef _DEBUG_ZONE1_TASK												
		debug_printf("\nHDMI3 Input\n");
#endif
		Analog7CH1OutSelectChannel(SOURCE_DSP);  /*DSP输出后从D/A输出*/
		HDMI_Port_Select(2);  /*HDMI 板切换到HDMI3 口*/	
		Pre_value=8; /*DSP 重新复位*/
		SetDspPower(0); /*DSP 保持复位状态*/	
		src_mode=0;
		return src_mode;	  
		}		 
	if(channel==INPUT_HDMI4) /*HDMI 2 Input*/
		{  
	//	DelayMs(500);   
#ifdef _DEBUG_ZONE1_TASK												
		debug_printf("\nHDMI4 Input\n");
#endif
		Analog7CH1OutSelectChannel(SOURCE_DSP);  /*DSP输出后从D/A输出*/
		HDMI_Port_Select(3);  /*HDMI 板切换到HDMI4 口*/	
		Pre_value=8; /*DSP 重新复位*/
		SetDspPower(0); /*DSP 保持复位状态*/	
		src_mode=0;
		return src_mode;	  
		}		 

	InitDSDMode(0); /*D/A init PCM-Mode*/
//	HDMI_Port_Select(3); /*Turn off HDMI */  
	Pre_value=8; /*清掉HDMI 当前返回流稳定信息*/
	DSP_Input_Stream_Select(AD_Input); /*输入到DSP源--4 A/D */  
	if(channel == INPUT_CH71) /*7.1CH Input*/
		{
		AnalogInputTypeSelect(Select_7CH1_RL_INPUT);  
		DigitalInSelect71Channel();
		if(AnalogInMode[channel]) /*DSP*/
			{
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("7.1CH D/A\n");
#endif
			SetDspPower(3);	/*DSP 切换到Analog 口*/ 
			Analog7CH1OutSelectChannel(SOURCE_DSP);	
			src_mode = 1;
			}
		else /*By Pass*/
			{
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("7.1CH Bypass\n");
#endif
			Analog7CH1OutSelectChannel(SOURCE_CH71_IN);
			src_mode = 2;
			}
		return src_mode;
		}

	//input_channel = SystemSelectInputChannel(channel); 
	input_channel = SystemSearchInputChannel(channel); //add cjm 2007-07-31

	if(input_channel == 0) 
		{
		input_channel = 1;	
		if(channel == INPUT_1)
			{
			input_channel = 2;
			} 
		}

	sys_flag.zone1_sub_channel = input_channel;	/*确定当前输入通道sub channel */

	if(input_channel == 3)	/*Analog 通道*/
		{
		Set2ChVolume(AnalogInLevelTable[AnalogInLevel[channel]]); 
		if(AnalogInMode[channel]) /*DSP*/
			{
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("2CH D/A\n");
#endif
			SetDspPower(3);	/*DSP 切换到Analog 口*/ 
			Analog7CH1OutSelectChannel(SOURCE_DSP);
			src_mode = 1;
			}
		else	 /*Bypass*/
			{
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("2CH Bypass\n");
#endif
			Analog7CH1OutSelectChannel(SOURCE_2CH);
			src_mode = 2;
			}
		}
	else	 /*Digital Input*/
		{		
		SetDspPower(1);	/*DSP 切换到spdif 口*/ 
		Analog7CH1OutSelectChannel(SOURCE_DSP);
		}

	return src_mode;    
}
extern unsigned long last_stream;
void ProcInputSelect(unsigned char direction)
{
	char update = 0;
	short temp_volume_value; /*用于各通道独立音量*/

	SoftMute(1);   

	switch(direction)
		{
		case INPUT_1:
		case INPUT_2:
		case INPUT_3:
		case INPUT_4:
		case INPUT_5:
		case INPUT_6:
		case INPUT_7:
		case INPUT_8:
		case INPUT_CH71:
		case INPUT_HDMI1:
		case INPUT_HDMI2:
		case INPUT_HDMI3:
		case INPUT_HDMI4:
		case ARC_CHANNEL:
			sys_flag.zone1_channel = direction;			
			update = 1;
		case INPUT_TUNER:			 // jian add in 12-03-07
			break;
		}
//
	Zone1SrcName = GetSrcName(sys_flag.zone1_channel);
	MsgSetVfdShow(ZONE1_ID,0);
	
	if(update)
		{
		sys_flag.src_mode = SelectSource(sys_flag.zone1_channel);
		sys_flag.stream_type = STREAM_NONE;  /*清掉当前流信息*/
		stream_type=STREAM_NONE; /*清掉当前流信息*/		
		last_stream=0; /*清掉当前流信息*/  
		EepromWriteByte(ZONE1_OUT_POSITION, sys_flag.zone1_channel);  
		}
	
	temp_volume_value=EepromReadByte(ZONE1_VOLUME_POSITION + sys_flag.zone1_channel); /*各通道独立音量*/
	sys_flag.zone1_volume=volume_table[temp_volume_value]; 
	Set7Ch1MainVolume(sys_flag.zone1_volume);  
	if(sys_flag.zone1_channel == INPUT_CH71||(sys_flag.zone1_sub_channel==3))//模拟输入时
	{
		SoftMute(0);
	}
}  

void PowerOnDisplay()
{
	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);
	VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,C11Logo);
	VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,Surround_Processor);
}

void SetMasterVolumeValue(unsigned char volume)
{
	int i=0;

	for(i=0;i<15;i++)
		EepromWriteByte(ZONE1_VOLUME_POSITION+i,volume);  //Input1-Input8 Input7.1 Input_tuner HDMI1 HDMI2Volume
	
	EepromWriteByte(AM_VOLUME_POSITION,volume); //AM Volume
	EepromWriteByte(FM_VOLUME_POSITION,volume); //FM Volume
	EepromWriteByte(HDAM_VOLUME_POSITION,volume); //HDAM Volume
	EepromWriteByte(HDFM_VOLUME_POSITION,volume); //HDFM Volume
	
	sys_flag.zone1_volume=volume_table[volume];    
}

short GetVolumeEepromSetValue(short volume)
{
	short i;

	for(i=0;i<106;i++)
		{
		if(volume==volume_table[i])
			return i;
		} 
}
//
void SystemPowerOn(void)
{
	unsigned char Debug_Value;
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nsys power on");
#endif
	SetResetPin(0); /*System reset-low*/ //change by cjm 2009-7-10
	SystemPower(ON); /*前置面板供电*/
	PowerLed(OFF); /*关闭前置面板灯*/
	DelayMs(1500); /* 延时2S 等电源稳定后*/
	SetIoPowerOn(); 
	SetResetPin(1); /*System reset-High*/ //change by cjm 2009-7-10
	
	if(sys_flag.system_dimer == 0) /*如果Dimer 值为0,则开机时把Dimer值设为2,显示开机LOGO*/
		sys_flag.system_dimer = 2;
	
	VfdSetDimerValue(sys_flag.system_dimer);	
	
	sys_flag.system_status = POWER_ON;
//
#ifdef _DEBUG_ZONE1_TASK
	//debug_printf("\nsys power on1");
#endif

	VolumeInit();  /*Mas9116复位不稳定,重新初始化*/
#ifdef _DEBUG_ZONE1_TASK
	//debug_printf("\nmake1");
#endif

	InitCs8416();  /*init cs8416*/
#ifdef _DEBUG_ZONE1_TASK
//	debug_printf("\nmake2");
#endif

	InitDACReg(); /*init DAC*/      
#ifdef _DEBUG_ZONE1_TASK
//	debug_printf("\nmake3");
#endif

	Set2ChVolume(0); /*2 channel Input Volume Setting*/
#ifdef _DEBUG_ZONE1_TASK
//	debug_printf("\nmake4");
#endif

	//DSD_Select(1); /*硬件BUG,总是使它为高电平*/
	LPC2132HwReset();
#ifdef _DEBUG_ZONE1_TASK
//	debug_printf("\nmake5");
#endif

	IC4094_Default();
#ifdef _DEBUG_ZONE1_TASK
//	debug_printf("\nmake6");
#endif

	SoftMute(1); /*用于去掉开机噪声*/
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("softmute in re power on \n");
#endif

/*jian add in 10-10-25*/
	Set_FLFR_Mas9116_Mute(0); //硬件mute
	Set_SLSR_Mas9116_Mute(0);
	Set_CENLFE_Mas9116_Mute(0);
	Set_SBLSBR_Mas9116_Mute(0);
/*end*/
	
#ifdef _DEBUG_ZONE1_TASK

//	debug_printf("\nmake7");
#endif
	
	PowerOnDisplay(); /*Display product name*/
	
#ifdef _DEBUG_ZONE1_TASK
//	debug_printf("\nmake8");
#endif
	
	key_enable = 0;
	DelayMs(POWER_ON_DELAY_TIME);  
	key_enable = 1;
	
#ifdef _DEBUG_ZONE1_TASK
//	debug_printf("\nsys power on2");
#endif

	SetMasterVolumeValue(EepromReadByte(Master_Volume_Level)); /*开机按照客户的设置音量调节*/
	sys_flag.system_dimer=EepromReadByte(DIMER_POSITION); /*得到保持的Dimer值*/
	VfdSetDimerValue(sys_flag.system_dimer);	/*Set power up dimer*/	
#ifdef _DEBUG_ZONE1_TASK
//	debug_printf("\nsys power o3");
#endif

}

void SystemPowerOff(void)
{
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("sys power off\n");
#endif
	PowerOffMuteControl(); /*关机静音处理*/
	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);
	VfdDisplay(VFD_LINE1_START_POSITION+8,VFD_UPDATE_LINE_ALL,"-OFF-");
	DelayMs(500);
       VfdDisplay(VFD_LINE1_START_POSITION+8,VFD_UPDATE_LINE_ALL,"     "); //jian add in 07-05-30
	//再次开机的时候,会首先出现一个-OFF-.所以在关机之间,先把它清掉  JIAN 07-05-30

	SetIoStandby();
	SystemPower(OFF);
	PowerLed(ON);
	sys_flag.system_status = POWER_OFF;
  
	key_enable = 0;
	DelayMs(POWER_OFF_DELAY_TIME);
	key_enable = 1;
}

void Zone1Mute(uchar mute)
{
	if(mute)
		{
		Set7CH1Mute(Mute);
		SetBalance_Mute(Mute);
		SetHeadphoneMute(UnMute); /*耳机输出刚好相反*/
		}
	else
		{		
		Set7CH1Mute(UnMute);
		SetBalance_Mute(UnMute);
		SetHeadphoneMute(Mute); /*耳机输出刚好相反*/
		}
}

void SetAutoseekFlag(unsigned char on)
{
	if(on)
		EepromWriteByte(AUTO_SEEK_POSITION,1);
	else
		EepromWriteByte(AUTO_SEEK_POSITION,0);
}

unsigned char IsAutoSeekOn()
{
	if(EepromReadByte(AUTO_DETECT_POSITION) == 0)
		return 1;
	return 0;
}

unsigned char IsEverAutoseek()
{
	if(EepromReadByte(AUTO_SEEK_POSITION) == 1)
		return 1;
	return 0;
}

extern unsigned char HDMI_Audio_Out;
extern void SetHdmiAudioOut(uchar mode);
extern BOOL ProcAutoSeekSur(unsigned char start_channel);
void ProcZone1PowerOn(void)
{
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nzone1 power on");
#endif

	if(sys_flag.system_status == POWER_OFF)
		{
		SystemPowerOn();
		}

	if(sys_flag.system_zone1_status != POWER_ON)
		{		
		sys_flag.zone1_channel = EepromReadByte(ZONE1_OUT_POSITION);
	
		if(IsEverAutoseek())
			{ 
			if((0 ==SystemSearchInputChannel(sys_flag.zone1_channel)) && (IsAutoSeekOn())) //change cjm 2007-07-31
				{
				ProcAutoSeekSur(INPUT_CH71); /*change cjm 2007-07-31*/
				}
			else
				{
				MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				}		
			}
		else
			{		
			if(ProcAutoSeekSur(INPUT_CH71)== TRUE) //change cjm 2007-07-31
				{
				SetAutoseekFlag(1);
				}
			else  
				{
				return;
				}		
			}

		MsgSetVolume(ZONE1_ID,MODE_FIXED,sys_flag.zone1_volume);
		MsgSetMute(ZONE1_ID,MODE_FIXED,OFF);
		
//		if((SystemTriggerMode == 0) ||(SystemTriggerMode == 2)) 
		if(SystemTriggerMode == 1) 
			{
			SetTrigerOut(1);	//Triger out 1
			}	

		if(SystemTriggerMode1 == 1) 
			{
			SetTrigerOut1(1);	//Triger out 1
			}	

		if(SystemTriggerMode2 == 1) 
			{
			SetTrigerOut2(1);	//Triger out 1
			}	

		SetHdmiAudioOut(HDMI_Audio_Out); /*HDMI Audio Out Command*/
		}
	sys_flag.system_zone1_status = POWER_ON;
	HdmiSendByte(ADDR_HDMI_POWER_OFF,1); // jian add in 10-09-07
}

void ProcZone1PowerOff(void)
{
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("zone1 power off\n");
#endif

	if(sys_flag.system_zone1_status != POWER_OFF)
		{
		VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_LINE_ALL);
		SetDspPower(0);	/*dsp power off */
		Zone1Mute(ON);		  
//		if((SystemTriggerMode == 0) ||(SystemTriggerMode == 2))
//		if(SystemTriggerMode == 1)
			{
			SetTrigerOut(0);	//Triger out 0
			}		  
//		if(SystemTriggerMode1 == 1)
			{
			SetTrigerOut1(0);	//Triger out 0
			}		  
//		if(SystemTriggerMode2 == 1)
			{
			SetTrigerOut2(0);	//Triger out 0
			}		  

		}

	Dolby2ch_Flag=0;


	HdmiSendByte(ADDR_HDMI_POWER_OFF,0); // jian add in 10-09-07
	if(sys_flag.system_zone2_status != POWER_ON)
		{
		SystemPowerOff();
		}
	else
		{

		if(Movie_Music_Speaker_Flag == 0)			
			VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE_ALL,"M1:    -OFF-        ");
		else
			VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE_ALL,"M2:    -OFF-        ");
		DelayMs(1000);
		MsgSetVfdShow(ZONE2_ID,0);
		}

	sys_flag.system_zone1_status = POWER_OFF;
	
}

/*
#include "auto_sound.c"
#include "flash_update.c"
*/
void LoadSystemStartupValue(void)
{
	unsigned char i; 

	sys_flag.system_zone1_status = POWER_OFF;
	sys_flag.zone1_sub_channel = 1; 
#if 0
	sys_flag.balance_fl = 0;
	sys_flag.balance_fr = 0;
	sys_flag.balance_sl = 0;
	sys_flag.balance_sr = 0;
	sys_flag.balance_sbl = 0;
	sys_flag.balance_sbr = 0;
	sys_flag.balance_sw = 0;
	sys_flag.balance_cen = 0;
#endif
	sys_flag.stream_type = STREAM_NONE;
	sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NORMAL;
	sys_flag.decode_mode.pcm_sur_mode = 1; /*movie*/
	sys_flag.decode_mode.dts_decode_mode = DTS_MODE_NORMAL;
	sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL;
	sys_flag.decode_mode.dolby_sur_mode = 1; /*movie*/

	i = EepromReadByte(PRODUCT_ID_POSITION);
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("PRODUCT_ID_POSITION:%02X\n",i);
#endif
	if(i != EVER_USED) /*如果第一次用就加载默认值*/
		{  
#ifdef _DEBUG_ZONE1_TASK
		debug_printf("Reset EEPROM\n");
#endif
		EepromWriteByte(PRODUCT_ID_POSITION,EVER_USED); /*写EVER_USED表示已经用过了*/
		SetupLoadDefault(0); 
		}
	else /*用过了(不是第一次)就加载保存在Eeprom里的值*/
		{
#ifdef _DEBUG_ZONE1_TASK
		debug_printf("Load EEPROM\n");
#endif
		SetupLoadEeprom(1); 
		}
}

const char * ch_name[] = 
{
"TOSLINK",
"COAXIAL",
"ANALOG",
};

#define MAX_SEEK_CHANNEL 9		/*INPUT1-8和7.1Input*/ 

void SeekSelect(unsigned char current_channel,unsigned char current_sub_channel)
{
	sys_flag.zone1_channel = current_channel;
	sys_flag.zone1_sub_channel = current_sub_channel;

	Zone1SrcName = GetSrcName(sys_flag.zone1_channel);

	if(current_channel == INPUT_CH71) /*7.1 CH Input*/
		{
		if(AnalogInMode[current_channel]) /*DSP */
			{
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("7.1CH D/A\n");
#endif
			SetDspPower(3);	/*DSP 切换到Analog 口*/ 
			Analog7CH1OutSelectChannel(SOURCE_DSP);
			sys_flag.src_mode = 1; /*DSP*/
			}
		else /*By Pass */
			{
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("7.1CH Bypass\n");
#endif
			Analog7CH1OutSelectChannel(SOURCE_CH71_IN);
			sys_flag.src_mode = 2; /*By Pass*/
			}
		}
	else if(current_sub_channel == 3)	/*Analog 通道 */
		{
		Set2ChVolume(AnalogInLevelTable[AnalogInLevel[current_channel]]);
		if(AnalogInMode[current_channel]) /*DSP */
			{
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("2CH D/A\n");
#endif
			SetDspPower(3);	/*DSP 切换到Analog 口*/ 
			Analog7CH1OutSelectChannel(SOURCE_DSP);
			sys_flag.src_mode = 1; /*DSP*/
			}
		else/*By Pass */
			{
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("2CH Bypass\n");
#endif
			Analog7CH1OutSelectChannel(SOURCE_2CH);
			sys_flag.src_mode = 2;  /*By Pass*/
			}
		}
	else	 /*Digital Input*/
		{
		SetDspPower(1);	/*DSP 切换到spdif 口*/ 
		Analog7CH1OutSelectChannel(SOURCE_DSP);
		sys_flag.src_mode = 0;
		}

	EepromWriteByte(ZONE1_OUT_POSITION, sys_flag.zone1_channel);
	MsgSetVfdShow(ZONE1_ID,0);
}
//
#define AUTOSEEK_DELAY_TIME 100

BOOL Zone1ProcAutoSeek(unsigned char start_channel,unsigned char sub_channel,unsigned char goon_seeking)
{
	uchar current_channel,current_sub_channel;
	uchar i;

	key_enable = 0; /*按键不起作用*/

	current_channel = start_channel;
	current_sub_channel = sub_channel + 1;

	if(current_channel == INPUT_CH71)
		{
		current_channel = INPUT_1;  /*INPUT1 只有Digital=2 和Analog=3*/
		current_sub_channel = 2;
		}

	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE); //clear screen
	if(Movie_Music_Speaker_Flag == 0)
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"M1: Auto Seek... %3d",sys_flag.zone1_volume); /*第一行显示内容*/
	else
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"M2: Auto Seek... %3d",sys_flag.zone1_volume); /*第一行显示内容*/
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("Start\n");
#endif
	do{
		for(i=0;i<MAX_SEEK_CHANNEL;i++,current_channel++)
			{
			current_channel %= MAX_SEEK_CHANNEL;
			switch(current_channel)
				{
				case INPUT_CH71: /*7.1Input 就只有一种输入Analog*/
					current_sub_channel = 3;
				case INPUT_1: /*INPUT1 只有Digital =2和Analog=3 */
					if(current_sub_channel == 1)
						current_sub_channel = 2;
				default:
					for(;current_sub_channel<=3;current_sub_channel++)
						{
#ifdef _DEBUG_ZONE1_TASK
						debug_printf("Seek:%s -> %s\n",SrcName[current_channel],ch_name[current_sub_channel-1]);
#endif
						VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE); /*清第二行*/
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"%s:%s",SrcName[current_channel],ch_name[current_sub_channel-1]); /*第二行显示内容*/
						if(CheckInput(current_channel,current_sub_channel))
							{
							SeekSelect(current_channel,current_sub_channel);
							key_enable = 1;
							return TRUE;
							}
						if((_cur_key == _REMOTE_POWER_OFF_KEY) || (_cur_key == _PANEL_POWER_KEY))
							{
							MsgSetPower(ZONE1_ID,0);	/*Set power off*/
							return FALSE;
							}
						DelayMs(AUTOSEEK_DELAY_TIME);
						}
					break;
				}
			current_sub_channel = 1;
			}

		switch(start_channel)
			{
			case INPUT_CH71:
				break;
			case INPUT_1:
				if(current_sub_channel == 1)
					current_sub_channel = 2;
			default:
				for(current_sub_channel = 1;current_sub_channel<=sub_channel;current_sub_channel++)
					{
#ifdef _DEBUG_ZONE1_TASK
					debug_printf("Seek:%s -> %s\n",SrcName[current_channel],ch_name[current_sub_channel-1]);
#endif
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE); /*清第二行*/
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"%s:%s",SrcName[current_channel],ch_name[current_sub_channel-1]); /*第二行显示内容*/

					if(CheckInput(current_channel,current_sub_channel))
						{
						SeekSelect(current_channel,current_sub_channel);					
						key_enable = 1;
						return TRUE;
						}
					if((_cur_key == _REMOTE_POWER_OFF_KEY) || (_cur_key == _PANEL_POWER_KEY))
						{
						MsgSetPower(ZONE1_ID,0);	/*Set power off*/
						return FALSE;
						}
					DelayMs(AUTOSEEK_DELAY_TIME);
					}
				break;
			}
		}while(goon_seeking);

#ifdef _DEBUG_ZONE1_TASK
	debug_printf("Finish\n");
#endif
	key_enable = 1; /*按键起作用*/
	MsgSetVfdShow(ZONE1_ID,0);
	return FALSE;
}

/*
* Input Seek 功能函数
*/
//add cjm 2007-07-20
BOOL ProcAutoSeekSur(unsigned char start_channel)
{
	uchar current_channel,current_sub_channel,current_temp_channel;
	uchar i;
	short temp_volume_value; //add cjm 2007-08-03
	
	key_enable = 0;

	current_channel = current_temp_channel=start_channel;
	if(current_channel == INPUT_CH71) //从当前输入通道的下一个通道开始搜索
		current_channel = INPUT_1; 
	else
		current_channel=current_channel+1; 
	
	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE); //clear screen
	if(Movie_Music_Speaker_Flag == 0)
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"M1: Auto Seek... %3d",sys_flag.zone1_volume); //第一行显示内容
	else	
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"M2: Auto Seek... %3d",sys_flag.zone1_volume); //第一行显示内容

	for(i=current_channel;i<9;i++,current_channel++)
		{     //这时current_sub_channel用于VFD的显示
		if(current_channel==INPUT_CH71) //7.1Input 只有analog 输入
			current_sub_channel=2; 
		else
			current_sub_channel = EepromReadByte(ASSIGNED_INPUT1+i ); //其它的根据Assigne Input 设定进行搜索
		
#ifdef _DEBUG_ZONE1_TASK	
		debug_printf("Seek:%s -> %s\n",SrcName[current_channel],ch_name[current_sub_channel]);
#endif

		VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE); //清第二行
		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"%s:%s",SrcName[current_channel],ch_name[current_sub_channel]); //第二行显示内容

		//由于CheckInput 和SeekSelect 中的current_channel 、current_sub_channel是按照1.2.3这样的顺序
		//而SrcName 和ch_name 中的current_channel 、current_sub_channel是按照0.1.2的顺序
		switch(current_channel)
			{
				case INPUT_1: //Input 1输入只有Digital=2 和Analog=3 两种输入
					current_sub_channel=current_sub_channel+2;
					break;
				case INPUT_CH71: //Input7.1只有Analog=3 输入
					current_sub_channel=3;
					break;
				default:
					current_sub_channel=current_sub_channel+1;
					break;
			}
		
		if(CheckInput(current_channel,current_sub_channel))
			{
			//用于INPUT SEEK 音量的切换
			temp_volume_value=EepromReadByte(ZONE1_VOLUME_POSITION + current_channel); //add cjm 2007-08-03	
			sys_flag.zone1_volume=volume_table[temp_volume_value]; //add cjm 2007-08-03
			Set7Ch1MainVolume(sys_flag.zone1_volume); //add cjm 2007-08-03
			
			SeekSelect(current_channel,current_sub_channel);					
			key_enable = 1;
			return TRUE;
			}

		if((_cur_key == _REMOTE_POWER_OFF_KEY) || (_cur_key == _PANEL_POWER_KEY))
			{
			MsgSetPower(ZONE1_ID,0);	//Set power off
			return FALSE;
			}

		DelayMs(AUTOSEEK_DELAY_TIME);		
		}
	
	current_channel =current_temp_channel; //current_temp_channel 保存了start_channel

	if(current_channel!=INPUT_CH71) //INPUT_CH71 已经从Input1 到Input7.1都搜过了
	{
		for(current_channel=0; current_channel<=current_temp_channel; current_channel++)
		{
			if(current_channel==INPUT_CH71)
				current_sub_channel=2;
			else
				current_sub_channel = EepromReadByte(ASSIGNED_INPUT1+current_channel);

#ifdef _DEBUG_ZONE1_TASK
			debug_printf("Seek:%s -> %s\n",SrcName[current_channel],ch_name[current_sub_channel]);
#endif

			VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE); //清第二行
			VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"%s:%s",SrcName[current_channel],ch_name[current_sub_channel]); //第二行显示内容

			switch(current_channel)
			{
				case INPUT_1: //Input 1输入只有Digital=2 和Analog=3 两种输入
					current_sub_channel=current_sub_channel+2;
					break;
				case INPUT_CH71: //Input7.1只有Analog=3 输入
					current_sub_channel=3;
					break;
				default:
					current_sub_channel=current_sub_channel+1;
					break;
			}

			if(CheckInput(current_channel,current_sub_channel))
			{
				//用于INPUT SEEK 音量的切换
				temp_volume_value=EepromReadByte(ZONE1_VOLUME_POSITION + current_channel); //add cjm 2007-08-03	
				sys_flag.zone1_volume=volume_table[temp_volume_value]; //add cjm 2007-08-03
				Set7Ch1MainVolume(sys_flag.zone1_volume); //add cjm 2007-08-03
				
				SeekSelect(current_channel,current_sub_channel);					
				key_enable = 1;
				return TRUE;
			}

			if((_cur_key == _REMOTE_POWER_OFF_KEY) || (_cur_key == _PANEL_POWER_KEY))
			{
				MsgSetPower(ZONE1_ID,0);	//Set power off
				return FALSE;
			}

			DelayMs(AUTOSEEK_DELAY_TIME);	
		}
	}

	key_enable = 1;
	MsgSetVfdShow(ZONE1_ID,0);
	return FALSE;
}//add cjm 2007-07-20

void PcmProcCesKey()
{
	if(GetNormalFreq() == FREQ_192KHZ) /*192K不做其他效果*/
		return ;

	switch(sys_flag.decode_mode.pcm_decode_mode)
		{
		case PCM_MODE_NORMAL:
			sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_CES;
			break;
		case PCM_MODE_CES:
		default:
			sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NORMAL;
			break;
		}
	
	DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
	MsgSetVfdShow(ZONE1_ID,0);
	MsgSetDSPSetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
}

void Ac3ProcCesKey()
{
	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_PLUS)) /*Dolby Plus 不做CES7.1*/
		return;
	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_2ch)) /*Dolby Ture HD2.0不做CES7.1*/
		return;
	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_5ch)) /*Dolby Ture HD5.1不做CES7.1*/
		return;
	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_6ch)) /*Dolby Ture HD5.1不做CES7.1*/
		return;
	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_7ch)) /*Dolby Ture HD7.1不做CES7.1*/
		return;

	Dolby2ch_Flag=1; /*Dolby 2.0 + CES7.1 BUG*/
	if(sys_flag.decode_mode.dolby_decode_mode != AC3_MODE_CES_ON)
		sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_CES_ON;
	else
		sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL; 	 	

	DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
	MsgSetDSPSetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
}

extern unsigned char DtsType;
void DtsProcCesKey()
{
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_MA_48K)) /*DTS_HD_48K(2.0/5.1/7.1)不做CES7.1*/
		return;
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_MA_96k_192K)) /*DTS_HD_96K(2.0/5.1/7.1)不做CES7.1*/
		return;
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_HRA_48K)) /*DTS_HD_HRA_48K(2.0/5.1/6.1/7.1)不做CES7.1*/
		return;
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_HRA_96K)) /*DTS_HD_HRA_96K(2.0/5.1/6.1/7.1)不做CES7.1*/
		return;

	if(sys_flag.decode_mode.dts_decode_mode != DTS_MODE_CES_ON)
		sys_flag.decode_mode.dts_decode_mode = DTS_MODE_CES_ON;
	else
		sys_flag.decode_mode.dts_decode_mode = DTS_MODE_NORMAL;

	DspDecodeDTS(sys_flag.decode_mode.dts_decode_mode);
	MsgSetDSPSetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
}

void PcmProcNeoKey()
{
	if((GetNormalFreq() == FREQ_192KHZ))	/*192K 不做其他效果*/		
		return ;	

	SoftMute(1);
	if(sys_flag.decode_mode.pcm_decode_mode != PCM_MODE_NEO6)
		sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NEO6;
	else
		sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NORMAL;

	DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
	MsgSetVfdShow(ZONE1_ID,0);
	MsgSetDSPSetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
	SoftMute(0);
}

extern unsigned char PcmType;
void PcmProcPliiKey()
{
 	if(GetNormalFreq() == FREQ_192KHZ)	/*192K不做其他效果*/
		return ;

	SoftMute(1);
	DelayMs(300);
	switch(PcmType)
		{
		case _PCM_2_1:
 			sys_flag.decode_mode.pcm_decode_mode ++;
			switch(sys_flag.decode_mode.pcm_decode_mode)
				{
				case PCM_MODE_PL2:
				case PCM_MODE_PL2X:
					break;
				default:
					sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NORMAL;
					break;
				}			
			break;
		case _PCM_5_1:/*PCM5.1 只有PL2X的功能*/
			sys_flag.decode_mode.pcm_decode_mode ++;
			switch(sys_flag.decode_mode.pcm_decode_mode)
				{
				case PCM_MODE_PL2:
					sys_flag.decode_mode.pcm_decode_mode=PCM_MODE_PL2X;
					break;
				case PCM_MODE_PL2X:
					break;
				default:
					sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NORMAL;
					break;
				}
			break;
		case _PCM_7_1:
			sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NORMAL;
			break;
		}
	
	DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
	MsgSetVfdShow(ZONE1_ID,0);
	MsgSetDSPSetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
	DelayMs(300);
	SoftMute(0);
}




extern void DSP_PLII_DECODE_MODE(unsigned char mode);
extern void DSP_PLIIx_DECODE_MODE_STEREO(unsigned char mode);
extern void DTS_NEO6_Mode(unsigned char Wide_Mode,unsigned char NEO6_Mode);
extern void DspSetCrossbarEffect(unsigned char mode);
void PcmProcSurKey(void)
{
	uchar update = 0;
	
	if(GetNormalFreq() == FREQ_192KHZ)	/*192K不做其他效果*/
		return ;
	
	switch(sys_flag.decode_mode.pcm_decode_mode)
		{
		case PCM_MODE_PL2:
		case PCM_MODE_PL2X:
		case PCM_MODE_NEO6:
			if(sys_flag.decode_mode.pcm_sur_mode)
				{
				sys_flag.decode_mode.pcm_sur_mode = 0;
				}
			else
				{
				sys_flag.decode_mode.pcm_sur_mode = 1;
				}
			update = 1;
			break;
		case PCM_MODE_CES:
			sys_flag.decode_mode.pcm_sur_mode++;
			switch(sys_flag.decode_mode.pcm_sur_mode)
				{
				case 1: /*CES 7.1 ON ALL Stereo*/
					sys_flag.decode_mode.pcm_sur_mode=1;
					break;
				case 2: /*CES 7.1 ON ALL Mono*/
					sys_flag.decode_mode.pcm_sur_mode=2;
					break;
				default: /*CES 7.1 ON*/
					sys_flag.decode_mode.pcm_sur_mode=0;  
					break;
				}
			update=1;
			break;
		}

	switch(sys_flag.decode_mode.pcm_decode_mode)
		{
		case PCM_MODE_PL2:
		case PCM_MODE_PL2X:
			if(sys_flag.decode_mode.pcm_sur_mode)
				DSP_PLIIx_DECODE_MODE_STEREO(0x03);	/*movie */
			else
				DSP_PLIIx_DECODE_MODE_STEREO(0x02);	/*music*/
			break;
		case PCM_MODE_NEO6:
			if(sys_flag.decode_mode.pcm_sur_mode)
				//DTS_NEO6_Mode(0,0); /*Cinema*/ /*由于NEO6有问题,故用PL2X代替*/
				DSP_PLIIx_DECODE_MODE_STEREO(0x03);	/*movie */
			else
				//DTS_NEO6_Mode(0,1); /*Music*/
				DSP_PLIIx_DECODE_MODE_STEREO(0x02);	/*music*/
			break;
		case PCM_MODE_CES:
			DspSetCrossbarEffect(sys_flag.decode_mode.pcm_sur_mode);
			break;
		}

	if(update)
		MsgSetVfdShow(ZONE1_ID,0);
}

void Ac3ProcPliiKey(void)
{
	unsigned char Ture_HD_Channel;
	unsigned char Freq;
	
	SoftMute(1);
	switch(sys_flag.decode_mode.dolby_type)
		{
		case _DOLBY_2CH: /*Dobly 2ch 可以有PL2和PL2X的功能*/
			Dolby2ch_Flag=1;
			sys_flag.decode_mode.dolby_decode_mode ++;
			switch(sys_flag.decode_mode.dolby_decode_mode)
				{
				case AC3_MODE_PL2:
				case AC3_MODE_PL2X:
					break;
				default:
					sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL;
					break;
				}
			DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
			MsgSetVfdShow(ZONE1_ID,0); 
			MsgSetDSPSetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
			break;
		case _DOLBY_5CH:/*Dobly 5ch和DolbyEX 就只有PL2X的功能*/
		case _DOLBY_EX:
			sys_flag.decode_mode.dolby_decode_mode ++;
			switch(sys_flag.decode_mode.dolby_decode_mode)
				{
				case AC3_MODE_PL2:
					sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_PL2X;
				case AC3_MODE_PL2X:
					break;
				default: /*5.1声道输入经过AC3_MODE_NORMAL 解码后还是5.1声道*/
					sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL; 
					break;
				}
			DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
			MsgSetVfdShow(ZONE1_ID,0); 
			MsgSetDSPSetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
			break;
		//case _DOLBY_PLUS: /*市面上暂时只有PLUS7.1*/
		//	break;
		case _DOLBY_TRUE_HD_2ch: /*TureHD2.0/5.1只做PL2X处理*/
		case _DOLBY_TRUE_HD_5ch:
		case _DOLBY_TRUE_HD_6ch:
			Freq=DspReadTRUEHD(0x09);
			//debug_printf("\nFreq=%d",Freq); 
			if(Freq!=0x0C) //192K不做PL2X功能
			{
				sys_flag.decode_mode.dolby_decode_mode ++;
				switch(sys_flag.decode_mode.dolby_decode_mode)
				{
					case AC3_MODE_PL2:
						sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_PL2X;
					case AC3_MODE_PL2X:
						break;
					default:
						sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL; 
						break;
				}
				DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
				MsgSetVfdShow(ZONE1_ID,0); 
				MsgSetDSPHDMISetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
			}
			break;
		//case _DOLBY_TRUE_HD_7ch: /*TureHD7.1 不需要处理*/
		//	break;
		}
	
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("dolby decode:%d\n",sys_flag.decode_mode.dolby_decode_mode );
#endif

	//SetPanoramaMode(); //add cjm 2007-08-06 //change late	
	SoftMute(0);
}

void DtsProcPliiKey(void)
{
	switch(sys_flag.decode_mode.dts_type)
		{
		//case _DTS_HD_HRA_48K_2CH: /*市面上暂时没该片源不做处理*/
		//case _DTS_HD_HRA_48K_6CH: 
		//case _DTS_HD_HRA_96K_2CH: 
		//case _DTS_HD_HRA_96K_6CH: 
		case _DTS_HD_MA_48K_2ch:
		case _DTS_HD_MA_48K_5ch:
		case _DTS_HD_MA_96K_2ch:
		case _DTS_HD_MA_96K_5ch:
			SoftMute(1);
			if(sys_flag.decode_mode.dts_decode_mode != DTS_MODE_PLIIX)
				{
				sys_flag.decode_mode.dts_decode_mode = DTS_MODE_PLIIX;
				} 
			else
				{
				sys_flag.decode_mode.dts_decode_mode = DTS_MODE_NORMAL;	
				}

			DspDecodeDTS(sys_flag.decode_mode.dts_decode_mode);
			MsgSetVfdShow(ZONE1_ID,0); 
			MsgSetDSPHDMISetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
			SoftMute(0);
			break;
		default:
			break;
		}
}

void Ac3ProcSurKey(void)
{
	if(sys_flag.headphone_flag) /*Headphone 不做Surround 处理*/
		{
		return ;
		}
	else
		{
		switch(sys_flag.decode_mode.dolby_decode_mode)
			{
			case AC3_MODE_PL2:
			case AC3_MODE_PL2X:
				if(sys_flag.decode_mode.dolby_sur_mode)
					sys_flag.decode_mode.dolby_sur_mode = 0;
				else
					sys_flag.decode_mode.dolby_sur_mode = 1;						
				break;
			case AC3_MODE_CES_ON:
				if(sys_flag.decode_mode.dolby_type==_DOLBY_2CH) /*Dolby 2.0 CES ON 下有Stereo 和Mono*/
					{
					sys_flag.decode_mode.dolby_sur_mode++;
					switch(sys_flag.decode_mode.dolby_sur_mode)
						{
						case 1: /*CES 7.1 ON ALL Stereo*/
							sys_flag.decode_mode.dolby_sur_mode=1;
							break;
						case 2: /*CES 7.1 ON ALL Mono*/
							sys_flag.decode_mode.dolby_sur_mode=2;
							break;
						default: /*CES 7.1 ON*/
							sys_flag.decode_mode.dolby_sur_mode=0;  
							break;
						}				
					}
				else
					{
					if(sys_flag.decode_mode.dolby_sur_mode)
						sys_flag.decode_mode.dolby_sur_mode = 0;
					else
						sys_flag.decode_mode.dolby_sur_mode = 1;							
					}
				break;
			}

		switch(sys_flag.decode_mode.dolby_decode_mode)
			{
			case AC3_MODE_PL2:
			case AC3_MODE_PL2X:
				if(sys_flag.decode_mode.dolby_sur_mode)
					DSP_PLIIx_DECODE_MODE_MULTI(0x03); /*movie*/
				else
					DSP_PLIIx_DECODE_MODE_MULTI(0x02); /*music*/
				break;  
				
			case AC3_MODE_CES_ON:
				if(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)
					DspSetCrossbarEffect(sys_flag.decode_mode.dolby_sur_mode); 
				break;			
			}  

#ifdef _DEBUG_ZONE1_TASK
		debug_printf("dolby sur:%d\n",sys_flag.decode_mode.dolby_sur_mode );
#endif
		}
	MsgSetVfdShow(ZONE1_ID,0);
}

#define RESET_1_KEY 0x8520
#define VERSION_KEY 0x8521
#define RESET_2_KEY 0x8523
#define UPDATE_KEY 	0x8525
#define acr_channel_key 0x4800
BOOL IsHidenKey(KEYT key)
{
	static unsigned short key_value;
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
			key_value <<= 4;
			key_value |= (key - _REMOTE_DIGI_0_KEY);
			break;
		default:
			key_value = 0;
			break;
		}
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("IsHidenKey:%04X\n",key_value);
#endif
	switch(key_value)
		{
		case RESET_1_KEY:
			key_value = 0;
			return 1;
		case VERSION_KEY:
			key_value = 0;
			return 2;
		case RESET_2_KEY:
			key_value = 0;
			return 3;
		case UPDATE_KEY:
			key_value = 0;
			return 4;
		case acr_channel_key: // jian add in 10-08-06
			key_value = 0;
			return 5;

		default:
			return 0;
		}
}

#if 0
void decode_mode_refesh(void) /*jian add in 10-10-13        当MAST BOOT 再次启动的时候,应该调用这个函数,用来重新设置之前的配置*/
{

//	SoftMute(1);	
	switch(sys_flag.stream_type)
	{
	case STREAM_PCM:
		DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
		break;
	case STREAM_AC3:
		DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
		break;
	case STREAM_DTS: 
		DspDecodeDTS(sys_flag.decode_mode.dts_decode_mode);
		break;
	}	
//	MsgSetDSPHDMISetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
//	SoftMute(0);	
}
#endif
// jian change it in 11-06-21
void decode_mode_refesh(void) /*jian add in 10-10-13        当MAST BOOT 再次启动的时候,应该调用这个函数,用来重新设置之前的配置*/
{

//	SoftMute(1);	
	switch(sys_flag.stream_type)
	{
	case STREAM_PCM:

		DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
		break;
	case STREAM_AC3:
//		if(sys_flag.decode_mode.dolby_type == _DOLBY_EX )
//			break;
		
		if(sys_flag.decode_mode.dolby_decode_mode != 0) // jian add in 11-06-21
			DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
		break;
	case STREAM_DTS: 
//		if(sys_flag.decode_mode.dts_type == _DTS_ES_MATRIX)
//			break;

		if(sys_flag.decode_mode.dts_decode_mode!= 0)
			DspDecodeDTS(sys_flag.decode_mode.dts_decode_mode);
		break;
	}	
//	MsgSetDSPHDMISetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
//	SoftMute(0);	
}

unsigned char Stream;
unsigned char HDRadio_FM_Flag=0; /*面板上FM 与AM共用一个键*/
unsigned char HDRadio_HDFM_Flag=0; /*面板上HDFM 与HDAM 共用一个键*/

static void Zone1ProcessKey(KEYT key)
{  

extern unsigned char Movie_Music_Speaker_Flag; 

	if(sys_flag.system_zone1_status == POWER_OFF) /*Power off key process*/
		{
		switch(key)
			{
			case _REMOTE_POWER_ON_KEY:
			case _PANEL_POWER_KEY:
#ifdef _DEBUG_ZONE1_TASK
				debug_printf("Power On Key");
#endif
				MsgSetPower(ZONE1_ID,1); /*Set power on*/
				break;
			case _HP_OFF_KEY: 				
				sys_flag.headphone_flag = 0;
#ifdef _DEBUG_ZONE1_TASK
				debug_printf("HeadPhone Off\n");
#endif
				break;
			case _HP_ON_KEY:			
				sys_flag.headphone_flag = 1;
#ifdef _DEBUG_ZONE1_TASK
				debug_printf("HeadPhone On\n");
#endif
				break;
			}
		}
	else	 /*Power on key process*/
		{
		switch(key)
			{
			default:
				SetVfdOwner(1);
				break;
			}

		switch(IsHidenKey(key))
			{
			case 1:
				VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Restore Default...  ");
				SetupLoadDefault(1);
				SetAutoseekFlag(0);
				MsgSetPower(ZONE1_ID,0); /*Set power off*/
				break;
			case 2:
				ShowVersion();
				MsgSetVfdShow(ZONE1_ID,0);
				break;
			case 3:
				VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Restore Default...  ");
				SetupLoadDefault(1);
				SetAutoseekFlag(1);
				MsgSetPower(ZONE1_ID,0); /*Set power off*/
				break;
			case 4:
				VfdDisplayBlank(VFD_LINE1,VFD_UPDATE_NONE);
				VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);		
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"   HDMI Software    ");
				VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"     Update...    ");
				HDMI_Software_Update(); /*HDMI Software Update*/
				break;

			case 5: // jian add in 10-08-07
				MsgSetInputSource(ZONE1_ID,14); /*HDMI 2 Input*/  
				debug_printf("channel to arc channle\n");
				break;
			}		
		
		switch(key)
			{
			case _REMOTE_POWER_OFF_KEY:
			case _PANEL_POWER_KEY:
				MsgSetPower(ZONE1_ID,0); /*Set power off */
				break;				
			case _PANEL_ENTER_KEY:
			case _REMOTE_SETUP_KEY:
				SetupInit(); /*enter setup up mode*/
				break;		
			case _REMOTE_TEST_KEY:		
				AutoSoundInit();   
				break;		  
			case _REMOTE_BRIGHT_KEY:	
				MsgSetDimer(0,MODE_PLUS);     
				break;				  
			case _REMOTE_EQ_KEY:
				VfdTempDispBlank(VFD_LINE2,VFD_UPDATE_NONE);
				if(enable_eq)
					{
					enable_eq = 0;
					VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,30,"EQ OFF");
					}
				else
					{
					enable_eq = 1;
					VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,30,"EQ ON");
					}
				EqOn(enable_eq);  
				break;				
			case _REMOTE_FM_KEY:
				RadioInit(_FM_MODE);
				break;
			case _REMOTE_AM_KEY:
				RadioInit(_AM_MODE);
				break;
			case _REMOTE_HD_FM_KEY:				
				RadioInit(_HD_FM_MODE);
				break;
			case _REMOTE_HD_AM_KEY:
				RadioInit(_HD_AM_MODE);
				break;				
			case _PANEL_AM_FM_KEY:
				if(HDRadio_FM_Flag==0)
					{
					HDRadio_FM_Flag=1;
					RadioInit(_FM_MODE);
					}
				else
					{
					HDRadio_FM_Flag=0;
					RadioInit(_AM_MODE);
					}					
				break;				
			case _PANEL_HD_RADIO_KEY:
				if(HDRadio_HDFM_Flag==0)
					{
					HDRadio_HDFM_Flag=1;
					RadioInit(_HD_FM_MODE);
					}
				else
					{
					HDRadio_HDFM_Flag=0;
					RadioInit(_HD_AM_MODE);
					}
				break;					
			case _PANEL_ENCODER_PLUS_KEY:
			case _REMOTE_VOLUME_UP_KEY:
				MsgSetVolume(ZONE1_ID,MODE_PLUS,0);
				break;
			case _PANEL_ENCODER_MINUS_KEY:
			case _REMOTE_VOLUME_DOWN_KEY:
				MsgSetVolume(ZONE1_ID,MODE_MINUS,0);
				break;			
			case _PANEL_INPUT1_KEY:
			case _PANEL_INPUT2_KEY:
			case _PANEL_INPUT3_KEY:
			case _PANEL_INPUT4_KEY:
			case _PANEL_INPUT5_KEY:
			case _PANEL_INPUT6_KEY:
			case _PANEL_INPUT7_KEY:
			case _PANEL_INPUT8_KEY:
				MsgSetInputSource(ZONE1_ID,key - _PANEL_INPUT1_KEY);
				break;								
			case _REMOTE_INPUT1_KEY:
			case _REMOTE_INPUT2_KEY:
			case _REMOTE_INPUT3_KEY:
			case _REMOTE_INPUT4_KEY:
			case _REMOTE_INPUT5_KEY:
			case _REMOTE_INPUT6_KEY:
			case _REMOTE_INPUT7_KEY:
			case _REMOTE_INPUT8_KEY:
				MsgSetInputSource(ZONE1_ID,key - _REMOTE_INPUT1_KEY);
				break;					
			case _PANEL_7CH1_INPUT_KEY:
			case _REMOTE_7CH1_IN_KEY:
				MsgSetInputSource(ZONE1_ID,8); /*7.1 Input*/
				break;				
			case _PANEL_HDMI1_KEY:
			case _REMOTE_HDMI1_KEY:
				MsgSetInputSource(ZONE1_ID,10); /*HDMI 1 Input*/  
				break;								
			case _PANEL_HDMI2_KEY:
			case _REMOTE_HDMI2_KEY:	
				MsgSetInputSource(ZONE1_ID,11); /*HDMI 2 Input*/  		
				break;			   	
			case _PANEL_HDMI3_KEY:
			case _REMOTE_HDMI3_KEY:
				MsgSetInputSource(ZONE1_ID,12); /*HDMI 1 Input*/  
				break;								
			case _PANEL_HDMI4_KEY:
			case _REMOTE_HDMI4_KEY:	
				MsgSetInputSource(ZONE1_ID,13); /*HDMI 2 Input*/  		
				break;			   	
			case _PANEL_TVSOUND_KEY:
			case _REMOTE_TVSOUND_KEY:
				MsgSetInputSource(ZONE1_ID,14); /*HDMI 2 Input*/  		
				break;	
				
			case _REMOTE_MUTE_KEY:
				MsgSetMute(ZONE1_ID,MODE_TOGGLE,0);  
				break;						  
			case _REMOTE_PLIIX_KEY: /*PLIIX Code 只用于Dobly和PCM*/
			case _PANEL_PLIIX_KEY:
				if(sys_flag.headphone_flag || (CD_8416_channel == 1)) /*Headphone时当前功能不做*/
					return ;
				
 				switch(sys_flag.stream_type)
					{
					case STREAM_PCM:
						PcmProcPliiKey();
						break;
					case STREAM_AC3:
						Ac3ProcPliiKey();
						break;
					case STREAM_DTS: 
						DtsProcPliiKey();
						break;
					}			
				break;
			case _REMOTE_SUR_MODE_KEY:
			case _PANEL_SUR_MODE_KEY:	
				if(sys_flag.headphone_flag|| (CD_8416_channel == 1)) /*Headphone时当前功能不做*/
					return ;				
				
				switch(sys_flag.stream_type)
					{
					case STREAM_PCM:
						PcmProcSurKey();
						break;
					case STREAM_AC3:
						Ac3ProcSurKey();
						break;
					case STREAM_DTS: /*DTS 没有Music 和Movie的模式*/
						break;
					}
				break;			
			case _REMOTE_NEO96_24_KEY:
			case _PANEL_NEO6_KEY:	
				if(sys_flag.headphone_flag|| (CD_8416_channel == 1)) /*Headphone时当前功能不做*/
					return ;			
				
				if(sys_flag.stream_type == STREAM_PCM) 
					{
					PcmProcNeoKey();
					}
				break;				
			case _REMOTE_CES7_1_KEY:
			case _PANEL_CES7_1_KEY:
				if(sys_flag.headphone_flag|| (CD_8416_channel == 1)) /*Headphone时当前功能不做*/
					return ;		
				
				switch(sys_flag.stream_type)
					{
					case STREAM_PCM:
						PcmProcCesKey();
						break;
					case STREAM_AC3:
						Ac3ProcCesKey();
						break;
					case STREAM_DTS:
						DtsProcCesKey();
						break;
					}
				break;		
				
			case _HP_OFF_KEY:		
#ifdef _DEBUG_ZONE1_TASK
				debug_printf("\nHeadPhone Off@@@@@@@@@@@@@@\n");
#endif  
				sys_flag.headphone_flag = 0;
				SetDh2On(0);
				Zone1Mute(0); /*关闭耳机输出，打开其它输出*/		
				break;
			case _HP_ON_KEY:
#ifdef _DEBUG_ZONE1_TASK
				debug_printf("\nHeadPhone On@@@@@@@@@@@@@@@@\n");
#endif				
				sys_flag.headphone_flag = 1;
				SetDh2On(1);      
				Zone1Mute(1); /*打开耳机输出，关闭其它输出*/			
				break;

			case _REMOTE_SEEK_KEY:	/*HDMI 1 和HDMI 2不做搜索功能*/	
				if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
					break;
				else		
					ProcAutoSeekSur(sys_flag.zone1_channel); 			
				break;  

			case _REMOTE_LATE_KEY:
				if(sys_flag.stream_type == STREAM_AC3)
					{
					compression_mode ++;
					compression_mode %= 3;
					VfdTempDispBlank(VFD_LINE2,VFD_UPDATE_NONE);
					switch(compression_mode)
						{
						case 0:
							VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,20,"LATE OFF");
							break;
						case 1:
							VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,20,"LATE HALF");
							break;
						case 2:
							VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,20,"LATE FULL");
							break;
						}
					EepromWriteByte(LATE_POSITION, compression_mode);
					SetLate(DolbyType,compression_mode);
					}
				break;
			case _PANEL_UPDATE_KEY:
				//FlashUpdateInit();
				break;

			case _REMOTE_LP_MEMORY1_KEY:  // jian add in 10-11-16
	//		case _REMOTE_ARROW_LEFT_KEY:
				Movie_Music_Speaker_Flag = 0; //Movie mode
				ChangeListenProfileMode();
				Movie_Music_Speaker_Flag = 0; //Movie mode
				MsgSetVfdShow(ZONE1_ID,0);
				break;
//			case _REMOTE_ARROW_RIGHT_KEY:
			case _REMOTE_LP_MEMORY2_KEY:
				Movie_Music_Speaker_Flag = 1; //Movie mode
				ChangeListenProfileMode();
				Movie_Music_Speaker_Flag = 1; //Movie mode				
				MsgSetVfdShow(ZONE1_ID,0);
				break;
				
/*jian add in 10-11-18*/
//
			case _REMOTE_RADIO_CHANNEL_ADD_KEY:
//			case _REMOTE_ARROW_UP_KEY:
				sys_flag.zone1_channel ++;
				switch(sys_flag.zone1_channel )
					{
					case ARC_CHANNEL:
						sys_flag.zone1_channel = INPUT_1;
						break;
					case INPUT_TUNER:
						sys_flag.zone1_channel = INPUT_HDMI1; 
						break;
					case INPUT_HDMI1:
						RadioExit();	
						break;

					}
				ProcInputSelect(sys_flag.zone1_channel );
				break;

			case _REMOTE_RADIO_CHANNEL_RED_KEY:
     // 			case _REMOTE_ARROW_DOWN_KEY:				
				switch(sys_flag.zone1_channel )
					{
					case INPUT_1:
						sys_flag.zone1_channel = ARC_CHANNEL;
						break;
					case INPUT_HDMI1:
						sys_flag.zone1_channel = INPUT_TUNER; 
						break;
					case INPUT_TUNER:
						RadioExit();	
						break;

					}
				sys_flag.zone1_channel --;
				ProcInputSelect(sys_flag.zone1_channel );
				break;

/*end 10-11-18*/

#if 0
			case _REMOTE_DIGI_0_KEY:
				break;
			case _REMOTE_DIGI_1_KEY:
				break;
			case _REMOTE_DIGI_2_KEY:		
				break;
			case _REMOTE_DIGI_3_KEY:
				break;  
			case _REMOTE_DIGI_4_KEY:
				break;
			case _REMOTE_DIGI_5_KEY:	
				break;
			case _REMOTE_DIGI_6_KEY:				
				break;				
			case _REMOTE_DIGI_7_KEY:
				break;
			case _REMOTE_DIGI_8_KEY:
				break;
			case _REMOTE_DIGI_9_KEY: 
				break;  			
#endif				
			}			
		}	
}

unsigned char DSP_HDMI_Setting_Flag=0;
extern uchar  dsp_repower_flag ; // jian add in 10-10-13
TASK_RESULT Zone1ProcMessage(unsigned char *buffer)
{
	KEYT zone1_key;
	static char Pre_Message;
	char Message=0;
	short temp_volume; 
	short temp_volume_value; 
	short max_vol_Eeprom;
	short max_vol_value;


	Message=buffer[0]; 
	if(Message!=Pre_Message) 
		{
		Pre_Message=Message;
		}
	else
		{  
		if((Pre_Message==20)) /*如果联系2次消息一样,且都是MSG_DSP_Setting 就不做设置*/
			return;
		}

	switch(buffer[0])
		{
		case MSG_KeyPress:
			zone1_key = buffer[1];
			ProcDimerOff(zone1_key); 
			Zone1ProcessKey(zone1_key);
			return TRUE;		
		case MSG_Power:
			if(buffer[1])
				ProcZone1PowerOn();
			else
				ProcZone1PowerOff();
			break;		
		case MSG_InputSource:
			if(IsZone1PowerOn())
				{
				ProcInputSelect(buffer[1]);
				}
			break;				
		case MSG_DSP_Setting: //20
		        if(last_stream == 0)
		        	{
		        	debug_printf("msg_dsp_setting!\n");
				break;
		        	}
				
				/*jian add in 10-10-13*/
	//		if(dsp_repower_flag)  // 10 -10 -15
				{
		
				dsp_repower_flag = 0;
				DelayMs(100);
				}
				/*end   */
		//	DelayMs(1500);//jian add in 10-08-13
			DspLoadDefaultSetting(buffer[1]);       		
			MsgSetVolume(ZONE1_ID,MODE_FIXED,sys_flag.zone1_volume);  //jian 10-11-02
			break;
		case MSG_DSP_HDMI_Setting:
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("\n____BeforeHDMIDspLoadDefaultSetting");
#endif	
			decode_mode_refesh();
			//SoftMute(1);   
			//DSP_Delay_Firmware_Init(); /*加载Delay设置值--高清流容易把DSP搞死*/
			SetBassManagerSpeaker(); /*加载低音管理设置值*/    
			SoftMute(0); /*与DSPLOOP 里检测到的高清流对应*/
#ifdef _DEBUG_ZONE1_TASK
			debug_printf("\n____AfterHDMIDspLoadDefaultSetting");  
#endif			
			break;
		case MSG_Soft_Mute:
			SoftMute(buffer[1]);
			break;
		case MSG_DSP_Channel_Setting:
			DspChannelOutSetting(sys_flag.decode_mode.pcm_decode_mode); 
			break;
		case MSG_Stream:
			if(IsZone1PowerOn())
				{
				sys_flag.stream_type = buffer[1];
				sys_flag.stream_subtype = buffer[2];
				if(IsCurFocusTid(ZONE1_ID) || (IsCurFocusTid(RADIO_ID)))   
					VfdUpdateStreamType(sys_flag.stream_type,sys_flag.stream_subtype);
				}
			break;				
		case MSG_Show:
			if(IsZone1PowerOn() && IsCurFocusTid(ZONE1_ID))
				{
				VfdUpdateZone1();
				}
			break;			
		case MSG_Dimer:
			if(IsZone1PowerOn())
				{
				switch(buffer[1])
					{
					case MODE_FIXED:
						sys_flag.system_dimer = buffer[2];
						break;
					case MODE_PLUS:
						sys_flag.system_dimer ++;
						if(sys_flag.system_dimer > MAX_DIMER)
							sys_flag.system_dimer = 0;
						break;
					}
				VfdSetDimerValue(sys_flag.system_dimer);
				BrightSetup(WRITE_VALUE_MODE,sys_flag.system_dimer,0);
				}
			break;				
		case MSG_Mute:
			if(IsZone1PowerOn())
				{
				switch(buffer[1])
					{
					case MODE_TOGGLE:
						if(sys_flag.zone1_mute_flag == ON)
							{
							sys_flag.zone1_mute_flag = OFF;
							}
						else{
							sys_flag.zone1_mute_flag = ON;
							}
						break;
					case MODE_FIXED:
						sys_flag.zone1_mute_flag = buffer[2];
						break;
					}
				Zone1Mute(sys_flag.zone1_mute_flag);
				MsgSetVfdShow(ZONE1_ID,0);

				if(sys_flag.headphone_flag ==1)
					{
					Zone1Mute(1); /*打开耳机输出，关闭其它输出*/	
					}
				}
			break;					
		case Msg_Volume:
			if(IsZone1PowerOn())
				{
				switch(buffer[1])
					{
					case MODE_PLUS: 
						if(GetCurFocusTid() != RADIO_ID)
						{
							temp_volume_value=EepromReadByte(ZONE1_VOLUME_POSITION + sys_flag.zone1_channel); 	
						}
						else
						{
							switch(current_radio_mode)
								{
								case _FM_MODE:
									temp_volume_value=EepromReadByte(FM_VOLUME_POSITION);
									break;
								case _AM_MODE:
									temp_volume_value=EepromReadByte(AM_VOLUME_POSITION);
									break;
								case _HD_FM_MODE:
									temp_volume_value=EepromReadByte(HDFM_VOLUME_POSITION);
									break;
								case _HD_AM_MODE:
									temp_volume_value=EepromReadByte(HDAM_VOLUME_POSITION);
									break;
								default:
									break;
								}							
						}
						
						max_vol_Eeprom=EepromReadByte(Set_Max_Volume_Level); 
						max_vol_value=volume_table[max_vol_Eeprom];
						sys_flag.zone1_volume=volume_table[temp_volume_value];
						//debug_printf("\r\nmax_vol_value=%d",max_vol_value);
						//debug_printf("\r\nsys_flag.zone1_volume=%d",sys_flag.zone1_volume);
						if(sys_flag.zone1_volume<max_vol_value)
						{
							if(sys_flag.zone1_volume < ZONE1_MAX_VOLUME)
							{
								sys_flag.zone1_volume += ZONE1_VOLUME_STEP;
								temp_volume_value+=ZONE1_VOLUME_STEP; 
							}
						}
						
						if(GetCurFocusTid() != RADIO_ID)
						{
							EepromWriteByte(ZONE1_VOLUME_POSITION+sys_flag.zone1_channel,temp_volume_value); 
						}
						else
						{
							switch(current_radio_mode)
								{
								case _FM_MODE:
									EepromWriteByte(FM_VOLUME_POSITION,temp_volume_value);
									break;
								case _AM_MODE :
									EepromWriteByte(AM_VOLUME_POSITION,temp_volume_value);
									break;
								case _HD_FM_MODE:
									EepromWriteByte(HDFM_VOLUME_POSITION,temp_volume_value);
									break;
								case _HD_AM_MODE:
									EepromWriteByte(HDAM_VOLUME_POSITION,temp_volume_value);
									break;
								default:
									break;
								}
						}
						break;
					case MODE_MINUS:
						if(GetCurFocusTid() != RADIO_ID)
							temp_volume_value=EepromReadByte(ZONE1_VOLUME_POSITION + sys_flag.zone1_channel); 
						else
							{
							switch(current_radio_mode)
								{
								case _FM_MODE:
									temp_volume_value=EepromReadByte(FM_VOLUME_POSITION);
									break;
								case _AM_MODE:
									temp_volume_value=EepromReadByte(AM_VOLUME_POSITION);
									break;
								case _HD_FM_MODE:
									temp_volume_value=EepromReadByte(HDFM_VOLUME_POSITION);
									break;
								case _HD_AM_MODE:
									temp_volume_value=EepromReadByte(HDAM_VOLUME_POSITION);
									break;
								default:
									break;
								}							
							}

						sys_flag.zone1_volume=volume_table[temp_volume_value]; 				
						if(sys_flag.zone1_volume > ZONE1_MIN_VOLUME)
							{
							sys_flag.zone1_volume -= ZONE1_VOLUME_STEP;
							temp_volume_value-=ZONE1_VOLUME_STEP;
							}
						
						if(GetCurFocusTid() != RADIO_ID)
							EepromWriteByte(ZONE1_VOLUME_POSITION+sys_flag.zone1_channel,temp_volume_value); 
						else
							{
							switch(current_radio_mode)
								{
								case _FM_MODE:
									EepromWriteByte(FM_VOLUME_POSITION,temp_volume_value);
									break;
								case _AM_MODE:
									EepromWriteByte(AM_VOLUME_POSITION,temp_volume_value);
									break;
								case _HD_FM_MODE:
									EepromWriteByte(HDFM_VOLUME_POSITION,temp_volume_value);
									break;
								case _HD_AM_MODE:
									EepromWriteByte(HDAM_VOLUME_POSITION,temp_volume_value);
									break;
								default:
									break;
								}
							}
						break;
					case MODE_FIXED: 
						temp_volume = (buffer[2] << 8) | buffer[3];
						if(IsZone1VolumeValid(temp_volume) == TRUE)
							{
							sys_flag.zone1_volume = temp_volume;
							temp_volume_value=GetVolumeEepromSetValue(temp_volume);
							
							if(GetCurFocusTid() != RADIO_ID)
								EepromWriteByte(ZONE1_VOLUME_POSITION+sys_flag.zone1_channel,temp_volume_value); 
							else
								{
								switch(current_radio_mode)
									{
									case _FM_MODE:
										EepromWriteByte(FM_VOLUME_POSITION,temp_volume_value);
										break;
									case _AM_MODE:
										EepromWriteByte(AM_VOLUME_POSITION,temp_volume_value);
										break;
									case _HD_FM_MODE:
										EepromWriteByte(HDFM_VOLUME_POSITION,temp_volume_value);
										break;
									case _HD_AM_MODE:
										EepromWriteByte(HDAM_VOLUME_POSITION,temp_volume_value);
										break;
									default: 
										break;
									}
								}		
							}
						break;
					}
#ifdef _DEBUG_ZONE1_TASK
				debug_printf("\nzone1 volume:%d\n",sys_flag.zone1_volume);
#endif
				Set7Ch1MainVolume(sys_flag.zone1_volume);
				VfdUpdateVolumeValue(sys_flag.zone1_volume);
				}
			break;	
		}
	return FALSE;
}

