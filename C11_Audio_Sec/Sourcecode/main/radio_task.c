/* @file 		Radio_task.c
 * @brief 		HD_Radio 应用层文件
 *
 * @author 	chenjianming
 * @date 	  	2008-03-11
 * @note 		refer to chenshiguang C11 code
 * @version 	1.0
 */
#include "Macro.h"
#include "task.h"
#include "debug.h"
#include <stdio.h>
#include "Radio_task.h"
#include "message.h"
#include "keydefine.h"
#include "vfd_disp.h"
#include "c11_msg_handler.h"
#include "key_tab.h"
#include "setup_menu.h"
#include "queue.h"
#include "Rs232_protocol.h"

#include "..\driver\vfddriver.h"
#include "..\driver\tuner.h"
#include "..\driver\channel.h"
#include "..\driver\volume.h"
#include "..\driver\eeprom.h"
#include "..\driver\ioport.h"
#include "..\driver\uart.h"
#include "..\driver\Dac.h"
#include "..\driver\Hdmi.h"
#include "..\driver\rbds.h"

#include "..\cs495300\dspdrv.h"
#include <string.h>
//#include <stdio.h>          
#include "rbds.h"
#ifdef _DEBUG
	#define _DEBUG_RADIO_TASK  
#endif
	#define _DEBUG_RADIO_TASK  
	
enum{
RADIO_IDLE = 0,
RADIO_SEARCHING,
RADIO_SCANING,
RADIO_SAVING,
RADIO_TEXTING,
};

unsigned char HD_Radio_Power_Flag=0;
unsigned char Tuner_Text_Disp_Flag=0;
//unsigned char Tuner_Reply_Flag=0;

extern uchar psd_vaild; // jian add in 11-08-18
unsigned char Tuner_Signal_Quality=0;
unsigned int Tuner_Freq_H=0;
unsigned int Tuner_Freq_L=0;

#define ID3_TAG_LEN 65
uint8 Artist[ID3_TAG_LEN];
uint8 Title[ID3_TAG_LEN];

uint8 Artist_len = 0;
uint8 Title_len = 0;

/*jian add in 11-08-17*/
uchar currProgram = 0;
//uchar Tuner_Signal_Quality = 0;
uchar da_status = 0;
/*end*/

/*start*/
uint32 currSeekLevel = 0;
uint32 CD_NO;
/*jian add in 11-08-17*/


extern unsigned char key_enable;
extern uchar radio_time_tick;

uchar stereo_mode = 1;	/*1: auto,0: force mono*/
uchar current_st_mode = 0;

FREQT DEFAULT_AM_FREQ = AM_10K_MIN_FREQ;
FREQT MAX_AM_FREQ = AM_10K_MAX_FREQ;
FREQT MIN_AM_FREQ = AM_10K_MIN_FREQ;

FREQT DEFAULT_HDAM_FREQ=HD_AM_MIN_FREEQ;
FREQT MAX_HDAM_FREQ=HD_AM_MAX_FREQ;
FREQT MIN_HDAM_FREQ=HD_AM_MIN_FREEQ;

FREQT DEFAULT_FM_FREQ = EUROPE_FM_MIN_FREQ;
FREQT MAX_FM_FREQ = EUROPE_FM_MAX_FREQ;
FREQT MIN_FM_FREQ = EUROPE_FM_MIN_FREQ;

FREQT DEFAULT_HDFM_FREQ=HD_FM_MIN_FREQ;
FREQT MAX_HDFM_FREQ=HD_FM_MAX_FREQ;
FREQT MIN_HDFM_FREQ=HD_FM_MIN_FREQ;

uchar am_preset_base;
uchar HDFM_preset_base;
uchar HDAM_preset_base;

unsigned char current_save_position;
short temp_radio_time = 0;
unsigned char current_radio_mode = _FM_MODE;	/*当前radio模式，FM或者AM*/

FREQT system_freq=0;	/*当前radio频率，可以是AM或者FM*/
FREQT fm_freq=0;		/*用来保存FM的频率，用于在AM,FM切换时记忆用*/
FREQT am_freq=0;		/*用来保存AM的频率，用于在AM,FM切换时记忆用*/
FREQT HDFM_freq=0;	/*用来保存HDFM的频率*/
FREQT HDAM_freq=0;	/*用来保存HDAM的频率*/

unsigned char system_position = DEFAULT_FM_POSITION;	/*当前radioPRESET，可以是AM或者FM*/
unsigned char am_position = DEFAULT_AM_POSITION;		/*用来保存FM的PRESET，用于在AM,FM切换时记忆用*/
unsigned char fm_position = DEFAULT_FM_POSITION;		/*用来保存AM的PRESET，用于在AM,FM切换时记忆用*/
unsigned char HDFM_position=DEFAULT_HDFM_POSITION;     /*用来保存HDFM的PRESET*/
unsigned char HDAM_position=DEFAULT_HDAM_POSITION;    /*用来保存HDAM的PRESET*/

//unsigned char Tuner_Text_Rec_Buffer[Tuner_TEXT_BUF_SIZE];
unsigned char Tuner_Text_Disp_Buffer[Tuner_TEXT_DISP_SIZE];
R_STATUS current_radio_status = RADIO_IDLE;
R_STATUS last_radio_status;
void SetRadioStatus(R_STATUS status)
{
	current_radio_status = status;
}

R_STATUS GetRadioStatus()
{
	return current_radio_status;
}

BOOL IsFreqValid(FREQT freq, unsigned char radio_mode)
{
	switch(radio_mode)
		{
		case _FM_MODE:
			if((freq >= MIN_FM_FREQ) && (freq <= MAX_FM_FREQ))
				return TRUE;			
			break;
		case _AM_MODE:
			if((freq >= MIN_AM_FREQ) && (freq <= MAX_AM_FREQ))
				return TRUE;			
			break;
		case _HD_FM_MODE:
			if((freq>=MIN_HDFM_FREQ) && (freq<=MAX_HDFM_FREQ))
				return TRUE;
			break;
		case _HD_AM_MODE:
			if((freq>=MIN_HDAM_FREQ) && (freq<=MAX_HDAM_FREQ))
				return TRUE;
			break;			
		}
	return FALSE;
}

BOOL MsgSetRadioFreq(unsigned char mode,FREQT freq)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};

	buffer[0] = MSG_RadioFreq;
	buffer[1] = mode;
	buffer[2] = (freq >> 24) & 0xFF;
	buffer[3] = (freq >> 16) & 0xFF;
	buffer[4] = (freq >> 8) & 0xFF;
	buffer[5] = freq & 0xFF;

	return SendMessage(RADIO_ID,buffer);
}

BOOL MsgSetRadioSearch(unsigned char mode,unsigned char init)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_RadioSearch;
	buffer[1] = mode;
	buffer[2] = init;

	return SendMessage(RADIO_ID,buffer);
}

BOOL MsgSetRadioLoadPreset(unsigned char position)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_RadioPreset;
	buffer[1] = 0;	/*load*/
	buffer[2] = position;
	return SendMessage(RADIO_ID,buffer);
}

BOOL MsgTextDisplay(unsigned char mode)
{
	unsigned char buffer[MESSAGE_SIZE]={0};

	buffer[0]=MSG_HDRADIO_Text;
	buffer[1]=mode;
	return SendMessage(RADIO_ID,buffer);
}

static FREQT get_freq(unsigned char *buffer)
{
	FREQT freq;
	freq = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]));
	return freq;
}
//
BOOL MsgSetRadioSavePreset(unsigned char position,FREQT freq)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_RadioPreset;
	buffer[1] = 1;	/*save*/
	buffer[2] = position;
	buffer[3] = (freq >> 24) & 0xFF;
	buffer[4] = (freq >> 16) & 0xFF;
	buffer[5] = (freq >> 8) & 0xFF;
	buffer[6] = freq & 0xFF;
	return SendMessage(RADIO_ID,buffer);
}

BOOL SaveFmPreset(unsigned char position,unsigned int freq,unsigned char tuner_mode)
{
	switch(tuner_mode)
		{
		case _FM_MODE:
			if((position <=FM_PRESET_NUM) && (freq >= MIN_FM_FREQ) && (freq <= MAX_FM_FREQ))
				{
				freq = freq - MIN_FM_FREQ;
				freq /= FM_STEP;
				EepromWriteByte(EUROPE_FM_P00 + position,(unsigned char)freq);
				return TRUE;
				}			
			break;
		case _AM_MODE:
			if((position <=AM_PRESET_NUM) && (freq >= MIN_AM_FREQ) && (freq <= MAX_AM_FREQ))
				{
				freq = freq - MIN_AM_FREQ;
				freq /= AM_STEP;
				EepromWriteByte(AM_10K_P00 + position,(unsigned char)freq);
				return TRUE;
				}			
			break;
		case _HD_FM_MODE:
			if((position<=HDFM_PRESET_NUM)&&(freq>=MIN_HDFM_FREQ)&&(freq<=MAX_HDFM_FREQ))
				{
				freq=freq-MIN_HDFM_FREQ;
				freq/=HDFM_STEP;
				EepromWriteByte(HDFM_P00 + position,(unsigned char)freq);
				}
			break;
		case _HD_AM_MODE:
			if((position<=HDAM_PRESET_NUM)&&(freq>=MIN_HDAM_FREQ)&&(freq<=MAX_HDAM_FREQ))
				{
				freq=freq-MIN_HDAM_FREQ;
				freq/=HDAM_STEP;
				EepromWriteByte(HDAM_P00 + position,(unsigned char)freq);
				}			
			break;
		}
	return FALSE;
}

BOOL LoadRadioPreset(unsigned char position,unsigned int * freq,unsigned char tuner_mode)
{
	unsigned int rtn_freq;
	switch(tuner_mode)
		{
		case _FM_MODE:
			rtn_freq = EepromReadByte(EUROPE_FM_P00 + position);
#ifdef  _DEBUG_RADIO_TASK
			debug_printf("\nrFM_freq=%d",rtn_freq);
#endif
			rtn_freq = rtn_freq * FM_STEP;
			rtn_freq = rtn_freq + MIN_FM_FREQ;
			if((rtn_freq >= MIN_FM_FREQ) && (rtn_freq <= MAX_FM_FREQ))
				{
				*freq = rtn_freq;
				return TRUE;
				}
			*freq = DEFAULT_FM_FREQ;	/*if load preset false, return default fm freq*/			
			break;
		case _AM_MODE:
			rtn_freq = EepromReadByte(AM_10K_P00 + position);
#ifdef _DEBUG_RADIO_TASK
			debug_printf("\nrAM_freq=%d",rtn_freq);
#endif			
			rtn_freq = rtn_freq * AM_STEP;
			rtn_freq = rtn_freq + MIN_AM_FREQ;
			if((rtn_freq >= MIN_AM_FREQ) && (rtn_freq <= MAX_AM_FREQ))
				{
				*freq = rtn_freq;
				return TRUE;
				}
			*freq = DEFAULT_AM_FREQ;	/*if load preset false, return default am freq*/			
			break;
		case _HD_FM_MODE:
			rtn_freq = EepromReadByte(HDFM_P00 + position);
#ifdef _DEBUG_RADIO_TASK
			debug_printf("\nrHDFM_freq0=%d",rtn_freq);
#endif			
			rtn_freq = rtn_freq * HDFM_STEP;
			rtn_freq = rtn_freq + MIN_HDFM_FREQ;
#ifdef _DEBUG_RADIO_TASK
			debug_printf("\nrHDFM_freq1=%d",rtn_freq);
#endif				
			if((rtn_freq >= MIN_HDFM_FREQ) && (rtn_freq <= MAX_HDFM_FREQ))
				{
				*freq = rtn_freq;
				return TRUE;
				}
			*freq = DEFAULT_HDFM_FREQ;	/*if load preset false, return default am freq*/			
			break;
		case _HD_AM_MODE:
			rtn_freq = EepromReadByte(HDAM_P00 + position);
#ifdef _DEBUG_RADIO_TASK
			debug_printf("\nrHDAM_freq=%d",rtn_freq);
#endif			
			rtn_freq = rtn_freq * HDAM_STEP;
			rtn_freq = rtn_freq + MIN_HDAM_FREQ;
			if((rtn_freq >= MIN_HDAM_FREQ) && (rtn_freq <= MAX_HDAM_FREQ))
				{
				*freq = rtn_freq;
				return TRUE;
				}
			*freq = DEFAULT_HDAM_FREQ;	/*if load preset false, return default am freq*/				
			break;
		}
	return FALSE;
}

BOOL IsTunerIfCorrect(unsigned char Signal_Quality)
{
	if(Signal_Quality>=1) /*如果信号质量大于1就认为是有效电台*/
		return TRUE;
	else
		return FALSE;
}

BOOL ISValidTunerPostion(unsigned char mode)
{
	static unsigned int old_freq=0; /*该变量用于当全部范围搜过一遍后就不再搜了*/

#ifdef _DEBUG_RADIO_TASK
	//debug_printf("\nold_freq=%d\n",old_freq);
	//debug_printf("\nsystem_freq=%d\n",system_freq);
#endif	
	
	switch(mode)
		{
			case _FM_MODE:
				if((system_position<=FM_PRESET_NUM)&&(system_freq < MAX_FM_FREQ)&&(system_freq>old_freq))
					{
					old_freq=system_freq;
					return TRUE;
					}
				else
					{
					old_freq=0;
					return FALSE;
					}
				break;
			case _AM_MODE:
				if((system_position<=AM_PRESET_NUM)&&(system_freq < MAX_AM_FREQ)&&(system_freq>old_freq))
					{
					old_freq=system_freq;					
					return TRUE;
					}
				else 
					{
					old_freq=0;
					return FALSE;
					}
				break;
			case _HD_FM_MODE:
				if((system_position<=HDFM_PRESET_NUM)&&(system_freq < MAX_HDFM_FREQ)&&(system_freq>old_freq))
					{
					old_freq=system_freq;					
					return TRUE;
					}
				else
					{
					old_freq=0;
					return FALSE;
					}
				break;
			case _HD_AM_MODE:
				if((system_position<=HDAM_PRESET_NUM)&&(system_freq < MAX_HDAM_FREQ)&&(system_freq>old_freq))
					{
					old_freq=system_freq;
					return TRUE;
					}
				else
					{
					old_freq=0;
					return FALSE;
					}
				break;
		}
		return TRUE;
}

void RadioDisplay(unsigned int freq,unsigned char tuner_mode,unsigned char position)
{
	unsigned char freq_message[] = "        ";
	
#ifdef _DEBUG_RADIO_TASK	
	//debug_printf("\nfreq=%d",freq);
#endif

	switch(tuner_mode)
		{
		case _FM_MODE:
			sprintf(freq_message,"%3d.%01dMHz",((freq/100) / 10),((freq/100) % 10));
			if(position & 0x80)
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Z1:FM %s   P%d-",freq_message,(position&0x7F)/10);
			else
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Z1:FM %s   P%02d",freq_message,position);
			fm_freq = freq;			
			break;
		case _AM_MODE:
			sprintf(freq_message," %4dKHz",freq);
			if(position & 0x80)
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Z1:AM %s   P%d-",freq_message,(position&0x7F)/10);
			else
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Z1:AM %s   P%02d",freq_message,position);
			am_freq = freq;			
			break;
		case _HD_FM_MODE:
			sprintf(freq_message,"%3d.%01dMHz",((freq/100) / 10),((freq/100) % 10));
			if(position & 0x80)
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Z1:HDFM %s P%d-",freq_message,(position&0x7F)/10);
			else
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Z1:HDFM %s P%02d",freq_message,position);	
			HDFM_freq=freq;
			break;
		case _HD_AM_MODE:
			sprintf(freq_message," %4dKHz",freq);
			if(position & 0x80)
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Z1:HDAM%s  P%d-",freq_message,(position&0x7F)/10);
			else
				VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Z1:HDAM%s  P%02d",freq_message,position);
			HDAM_freq = freq;						
			break;
		}

	//add by cjm 2009-03-30
	if(sys_flag.system_zone1_status == POWER_OFF)
		{
		if(IsZone2PowerOn())
			{
			if(sys_flag.zone2_channel == INPUT_TUNER)
				{
				VfdTempDispBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);
				switch(current_radio_mode)
					{
					case _FM_MODE:
						VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,30,"Z2: FM %3d.%01dMHz  ",((system_freq / 100)/10),((system_freq/100) % 10));
						break;
					case _AM_MODE:
						VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,30,"Z2: AM %4dKHz   ",system_freq);
						break;
					case _HD_FM_MODE: 
						VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,30,"Z2: HDFM %3d.%01dMHz  ",((system_freq / 100)/10),((system_freq/100) % 10));
						break;
					case _HD_AM_MODE:
						VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,30,"Z2: HDAM %4dKHz   ",system_freq);
						break;
					}
				if(sys_flag.zone2_mute_flag == ON)
					{
					VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_NONE,30,"Mute");
					}
				VfdTempDisp(VFD_LINE1_START_POSITION+17,VFD_UPDATE_LINE_ALL,30,"%3d",sys_flag.zone2_volume);
				}
			}
		}	
	//add by cjm 2009-03-30
}

#if 0
uchar *Trim(uchar *strVal)
{
	UINT8 *p1, *p2;

	if(strVal == NULL) return NULL;
	if(strlen(strVal) == 0) return strVal;

	p1 = strVal;
	while(p1 != 0 && *p1 == ' ')
		p1++;

	p2 = p1 + strlen(p1) -1;
	while(*p2 == ' ' && p1 != p2)
		p2--;

	*(p2+1) = 0;

	return p1;
}
#endif

void GetTextDispBuffer(unsigned char start_byte)
{
	unsigned char i,j=0;
        static uchar buffer_switch =0;
//	extern
	if (!RBDS_IsValid() )
		return;

	Artist_len = strlen(Artist);
	Title_len =  strlen(Title);
	
	debug_printf("Artist_len:%d",Artist_len);
	debug_printf("%s\n",Artist);
	debug_printf("Title_len:%d",Title_len);	
	debug_printf("%s\n",Title);
	debug_printf("start_byte:%d\n",start_byte);
		
	for(i=start_byte;i<start_byte+20;i++) /*一行20个字节*/
		{
		if(i < Artist_len)
			{
			buffer_switch =0;
			}
		else if(i< (Artist_len+Title_len))
			{
			buffer_switch = 1;
			}
		else
			{
			//Tuner_Text_Count =  0;
			return;
			}
			
			
		if(buffer_switch)
			{
			Tuner_Text_Disp_Buffer[j++]  = Title[i];
			}
		else	
			{
			Tuner_Text_Disp_Buffer[j++]  = Artist[i];  // jian add in 11-08-18
			}
		}
}

#if 0//测试时用    
void SetTextBuffer(void)
{
	unsigned char i;
	for(i=0;i<5;i++)
		Tuner_Text_Rec_Buffer[i]=0x52; //R
	for(i=5;i<10;i++)
		Tuner_Text_Rec_Buffer[i]=0x53; //S
	for(i=10;i<15;i++)
		Tuner_Text_Rec_Buffer[i]=0x54; //T
	for(i=15;i<20;i++)
		Tuner_Text_Rec_Buffer[i]=0x55; //U
	for(i=20;i<25;i++)
		Tuner_Text_Rec_Buffer[i]=0x56; //V
	for(i=25;i<30;i++)
		Tuner_Text_Rec_Buffer[i]=0x57; //w
	for(i=30;i<35;i++)
		Tuner_Text_Rec_Buffer[i]=0x58; //x
	for(i=35;i<40;i++)
		Tuner_Text_Rec_Buffer[i]=0x59; //Y
		
	for(i=40;i<45;i++)
		Tuner_Text_Rec_Buffer[i]=0x61; //a
	for(i=45;i<50;i++)
		Tuner_Text_Rec_Buffer[i]=0x62; //b
	for(i=50;i<55;i++)
		Tuner_Text_Rec_Buffer[i]=0x63; //c
	for(i=55;i<60;i++)
		Tuner_Text_Rec_Buffer[i]=0x64; //d
	for(i=60;i<65;i++)
		Tuner_Text_Rec_Buffer[i]=0x65; //e		
	for(i=65;i<70;i++)
		Tuner_Text_Rec_Buffer[i]=0x66; //f
	for(i=70;i<75;i++)
		Tuner_Text_Rec_Buffer[i]=0x67; //g
	for(i=75;i<80;i++)
		Tuner_Text_Rec_Buffer[i]=0x68; //h		
}
#endif

extern unsigned char Tuner_Text_Count;  
void Radio_Text_Display(void)
{
	//SetTextBuffer();   
	GetTextDispBuffer(Tuner_Text_Count);
	VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2); /*在第二行显示text diplay*/
	VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"%s",Tuner_Text_Disp_Buffer);
	DelayMs(1000); /*进行显示1000 ms*/  
}

#if 0
void TunerResetPreset()
{
	uchar i;
	for(i=0;i<FM_PRESET_NUM;i++)
		{
		EepromWriteByte(EUROPE_FM_P00 + i, 0);
		EepromWriteByte(JAPAN_FM_P00 + i, 0);
		}
	for(i=0;i<AM_PRESET_NUM;i++)
		{
		EepromWriteByte(AM_9K_P00 + i, 0);
		EepromWriteByte(AM_10K_P00 + i, 0);
		}
}
#endif

/*HD Radio 各模式之间切换*/
extern unsigned char PreDspMode;
extern unsigned long last_stream; // jian add in 12-03-07
BOOL RadioChangeMode(unsigned char mode)
{
	short temp_volume_value; 

	DacMute(1);
	SetCurFocusTid(RADIO_ID);
	if(HD_Radio_Power_Flag==0)
		{
		HD_Radio_Power_Flag=1;
		HDRadioPower(ON); /*HD Radio 上电*/
		VfdDisplayBlank(VFD_LINE1,VFD_UPDATE_NONE);
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1," Power On HD Radio  ");
		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"    Please Wait!    ");
		DelayMs(3000); /*显示3 秒--这个延时是为了HD Radio上电初始化时间*/
		}

	switch(mode)
		{
		case _AM_MODE:
			current_radio_mode = _AM_MODE;
			system_freq = am_freq;
			system_position = am_position;		
			temp_volume_value=EepromReadByte(AM_VOLUME_POSITION); 
			sys_flag.zone1_volume=volume_table[temp_volume_value]; 
			Set7Ch1MainVolume(sys_flag.zone1_volume); 				
			break;
		case _FM_MODE:
			current_radio_mode = _FM_MODE;
			system_freq = fm_freq;
			system_position = fm_position;		
			temp_volume_value=EepromReadByte(FM_VOLUME_POSITION); 
			sys_flag.zone1_volume=volume_table[temp_volume_value];
			Set7Ch1MainVolume(sys_flag.zone1_volume);		
			break;
		case _HD_AM_MODE:
			current_radio_mode = _HD_AM_MODE;
			system_freq = HDAM_freq;
			system_position = HDAM_position;	
			temp_volume_value=EepromReadByte(HDAM_VOLUME_POSITION); 
			sys_flag.zone1_volume=volume_table[temp_volume_value]; 
			Set7Ch1MainVolume(sys_flag.zone1_volume); 						
			break;
		case _HD_FM_MODE:
			current_radio_mode = _HD_FM_MODE;
			system_freq = HDFM_freq;  
			system_position = HDFM_position;	
			temp_volume_value=EepromReadByte(HDFM_VOLUME_POSITION); 
			sys_flag.zone1_volume=volume_table[temp_volume_value]; 
			Set7Ch1MainVolume(sys_flag.zone1_volume); 				
			break;
		}
	
	sys_flag.zone1_channel =INPUT_TUNER; 
	sys_flag.stream_type = STREAM_NONE; /*清掉以前留下的流信息*/	
	last_stream =0xffff;// jian add in 03-07
//	HDMI_Port_Select(3); /*Turn off HDMI */  
	DSP_Input_Stream_Select(AD_Input); /*输入到DSP源--A/D CS8416*/
	InitTuner(current_radio_mode);
	Tuner_Mute_Command(Tune_Unmute);
	Tuner_Volume_Init(); /*Set HD Radio volume*/	

	if(AnalogInMode[INPUT_TUNER])
		{
#ifdef _DEBUG_RADIO_TASK
		debug_printf("\nTuner -> DSP\n");
#endif
		PreDspMode=8;
		SetDspPower(1);  
		DigiSendByte(ADDR_ZONE1,0x03);/*通道切到HD Radio 数字口*/
		Analog7CH1OutSelectChannel(SOURCE_DSP);
		sys_flag.src_mode=0;
		}
	else
		{
#ifdef _DEBUG_RADIO_TASK
		debug_printf("\nTuner -> Bypass\n");
#endif
		AnalogInSelectChannel(ANALOG_IN_TUNER); /*通道切换到HD Radio 模拟口*/
		Analog7CH1OutSelectChannel(SOURCE_2CH);
		sys_flag.src_mode=2;
		}
	DacMute(0);
	
	return TRUE;
}

/*收音机初始化*/
BOOL RadioInit(unsigned char mode)
{
	short temp_volume_value; 
	FREQT temp_freq;

	
	DacMute(1);
	SoftMute(1);
	SetCurFocusTid(RADIO_ID);
	RBDS_Init();
	if(HD_Radio_Power_Flag==0)
		{
		HD_Radio_Power_Flag=1;
		HDRadioPower(ON); /*HD Radio 上电*/
		VfdDisplayBlank(VFD_LINE1,VFD_UPDATE_NONE);
		VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1," Power On HD Radio  ");
		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"    Please Wait!    ");
		DelayMs(3000); /*显示3 秒--这个延时是为了HD Radio上电初始化时间*/
		last_stream =0xffff;// jian add in 03-07
	//	ReadSoftwareVersion(uint8 * p, uint8 len)
		}
	
	switch(mode)
		{
		case _FM_MODE:
			current_radio_mode = _FM_MODE;
			system_freq = fm_freq;
			system_position = fm_position;		
			temp_volume_value=EepromReadByte(FM_VOLUME_POSITION); 
			sys_flag.zone1_volume=volume_table[temp_volume_value];
			Set7Ch1MainVolume(sys_flag.zone1_volume);		
			break;
		case _AM_MODE:
			current_radio_mode = _AM_MODE;
			system_freq = am_freq;
			system_position = am_position;		
			temp_volume_value=EepromReadByte(AM_VOLUME_POSITION); 
			sys_flag.zone1_volume=volume_table[temp_volume_value]; 
			Set7Ch1MainVolume(sys_flag.zone1_volume); 	
			break;			
		case _HD_FM_MODE:
			current_radio_mode = _HD_FM_MODE;
			system_freq = HDFM_freq;  
			system_position = HDFM_position;	
			temp_volume_value=EepromReadByte(HDFM_VOLUME_POSITION); 
			sys_flag.zone1_volume=volume_table[temp_volume_value]; 
			Set7Ch1MainVolume(sys_flag.zone1_volume); 					
			break;
		case _HD_AM_MODE:
			current_radio_mode = _HD_AM_MODE;
			system_freq = HDAM_freq;
			system_position = HDAM_position;	
			temp_volume_value=EepromReadByte(HDAM_VOLUME_POSITION); 
			sys_flag.zone1_volume=volume_table[temp_volume_value]; 
			Set7Ch1MainVolume(sys_flag.zone1_volume); 				
			break;
		}
//
#ifdef _DEBUG_RADIO_TASK
	debug_printf("\nsystem_freq=%d",system_freq);
	debug_printf("\nsystem_position=%d",system_position);
	debug_printf("\nsystem_position111=%d",system_position);
#endif
	sys_flag.zone1_channel =INPUT_TUNER; 
	sys_flag.stream_type = STREAM_NONE; /*清掉以前留下的流信息*/
//	HDMI_Port_Select(3); /*Turn off HDMI */  
	DSP_Input_Stream_Select(AD_Input); /*输入到DSP源--A/D CS8416*/
	InitTuner(current_radio_mode);
//	if((current_radio_mode==_FM_MODE)||(current_radio_mode==_AM_MODE)) //add by cjm 2008-11-3  remove it in 10-10-15
		{
		MsgSetRadioLoadPreset(system_position); 
		}

	if((current_radio_mode==_HD_AM_MODE)||(current_radio_mode==_HD_FM_MODE)) //add by cjm 2008-11-3  remove it in 10-10-15
		{
		 LoadRadioPreset(system_position,&temp_freq,current_radio_mode);
		 debug_printf("\LoadRadioPreset \n");		  // jian add in 10-10-18
		 system_freq = temp_freq;
		}
	
	Tuner_Mute_Command(Tune_Unmute);
	Tuner_Volume_Init(); /*Set HD Radio volume*/	
	MsgSetVfdShow(RADIO_ID,0);
	  
	if(AnalogInMode[INPUT_TUNER])
		{
#ifdef _DEBUG_RADIO_TASK
		debug_printf("\nTuner -> DSP\n");
#endif
		PreDspMode=8;
		SetDspPower(1);  
		//SetDownmix(1);
		DigiSendByte(ADDR_ZONE1,0x03);/*通道切到HD Radio 数字口*/
		Analog7CH1OutSelectChannel(SOURCE_DSP);
		sys_flag.src_mode=0;
		}
	else
		{
#ifdef _DEBUG_RADIO_TASK
		debug_printf("\nTuner -> Bypass\n");
#endif
		AnalogInSelectChannel(ANALOG_IN_TUNER); /*通道切换到HD Radio 模拟口*/
		Analog7CH1OutSelectChannel(SOURCE_2CH);
		sys_flag.src_mode=2;
		}

	AddRemoteRepeatKey(_IR_ARROW_LEFT_KEY);
	AddRemoteRepeatKey(_IR_ARROW_RIGHT_KEY);

	DacMute(0);
	SoftMute(0);
	return TRUE;
}

void RadioExit()
{
	if((sys_flag.system_zone2_status == POWER_ON)&&(sys_flag.zone2_channel == INPUT_TUNER))
		{
		;
		}
	else
		{
		if(HD_Radio_Power_Flag==1)
			{
			HD_Radio_Power_Flag=0;
			HDRadioPower(OFF); /*关闭HD Radio 电源*/
			}
		}
	
	SetCurFocusTid(ZONE1_ID);
	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);

	DeleteRemoteRepeatKey(_IR_ARROW_LEFT_KEY);
	DeleteRemoteRepeatKey(_IR_ARROW_RIGHT_KEY);
}

unsigned char CalcRadioPosition(unsigned char posi,unsigned char radio_mode,unsigned char mode)
{
	switch(radio_mode)
		{
		case _FM_MODE:
			switch(mode)
				{
				case MODE_MINUS:
					if(posi)
						posi --;
					break;
				case MODE_PLUS:
					if(posi < FM_PRESET_NUM)
						posi ++;
					break;
				case MODE_FIXED:
					if(posi > FM_PRESET_NUM)
						posi = DEFAULT_FM_POSITION;
					break;
				}			
			break;
		case _AM_MODE:
			switch(mode)
				{
				case MODE_MINUS:
					if(posi)
						posi --;
					break;
				case MODE_PLUS:
					if(posi < AM_PRESET_NUM)
						posi ++;
					break;
				case MODE_FIXED:
					if(posi > AM_PRESET_NUM)
						posi = DEFAULT_AM_POSITION;
					break;
				}			
			break;
		case _HD_FM_MODE:
			switch(mode)
				{
				case MODE_MINUS:
					if(posi)
						posi--;
					break;
				case MODE_PLUS:
					if(posi < HDFM_PRESET_NUM)
						posi ++;
					break;
				case MODE_FIXED:
					if(posi > HDFM_PRESET_NUM)
						posi = DEFAULT_HDFM_POSITION;
					break;
				}
			break;
		case _HD_AM_MODE:
			switch(mode)
				{
				case MODE_MINUS:
					if(posi)
						posi--;  
					break;
				case MODE_PLUS:
					if(posi < HDAM_PRESET_NUM)
						posi ++;
					break;
				case MODE_FIXED:
					if(posi > HDAM_PRESET_NUM)
						posi = DEFAULT_HDAM_POSITION;
					break;		
				}
			break;
		}
	return posi;
}

FREQT CalcRadioFreq(FREQT freq,unsigned char radio_mode,unsigned char dir)
{
	switch(radio_mode)
		{
		case _FM_MODE:
			if(dir)
				{
				freq += FM_STEP;
				if(freq > MAX_FM_FREQ)
					freq = MIN_FM_FREQ;				
				}
			else
				{
				freq -= FM_STEP;
				if(freq < MIN_FM_FREQ)
					freq = MAX_FM_FREQ;				
				}
			break;
		case _AM_MODE:
			if(dir)
				{
				freq += AM_STEP;
				if(freq > MAX_AM_FREQ)
					freq = MIN_AM_FREQ;				
				}
			else
				{
				freq -= AM_STEP;
				if(freq < MIN_AM_FREQ)
					freq = MAX_AM_FREQ;							
				}
			break;
		case _HD_FM_MODE:
			if(dir)
				{
				freq+=HDFM_STEP;
				if(freq>MAX_HDFM_FREQ)
					freq=MIN_HDFM_FREQ;
				}
			else
				{
				freq-=HDFM_STEP;
				if(freq<MIN_HDFM_FREQ)
					freq=MAX_HDFM_FREQ;
				}
			break;
		case _HD_AM_MODE:
			if(dir)
				{
				freq+=HDAM_STEP;
				if(freq>MAX_HDAM_FREQ)
					freq=MIN_HDAM_FREQ;
				}
			else
				{
				freq-=HDAM_STEP;
				if(freq<MIN_HDAM_FREQ)
					freq=MAX_HDAM_FREQ;
				}
			break;
		}
	return freq;
}

FREQT CalcCurrenTunerFreq(unsigned char mode,unsigned int Freq_H,unsigned int Freq_L)
{
	unsigned int freq;
	
#ifdef _DEBUG_RADIO_TASK
	debug_printf("\nFreq_H_1=%02X",Freq_H); 
	debug_printf("\nFreq_L_1=%02X",Freq_L);
#endif

	freq=((Freq_H<<8)& 0xFFFF)|Freq_L;	

	if((mode==_FM_MODE)||(mode==_HD_FM_MODE))
		freq=freq*10;
	
#ifdef _DEBUG_RADIO_TASK
	debug_printf("\nfreq=%d",freq);
#endif
	return freq;
}

KEYT digi_key_value = 0;

#define IsDigiKeyValid (digi_key_value & 0x80)
#define ClearDigiKeyValid digi_key_value &= ~0x80
static KEYT PreProcDigitKey(KEYT key)
{
	static unsigned char digi_10_key = 0;
	static unsigned char digi_key_count=0; /*选台需要2个按键组合,纪录按键次数*/

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
			if(digi_key_count==0)
				{
				digi_key_count++;
				digi_10_key=(key - _REMOTE_DIGI_0_KEY)*10;
				digi_key_value=digi_10_key;
				key = _DIGI_KEY;
				}	
			else
				{
				digi_key_count=0;
				digi_key_value = key - _REMOTE_DIGI_0_KEY + digi_10_key;
				digi_10_key = 0;
				digi_key_value |= 0x80; /*为有效选台按键值*/			
				key = _DIGI_KEY;
				}
			break;
		default:
			digi_key_count=0;			
			digi_10_key = 0;
			break;		
		}
	return key;
}

static void SaveExit()
{
	SetRadioStatus(RADIO_IDLE);
	MsgSetVfdShow(RADIO_ID,0);
}

void UpdateSaveVfd(unsigned long posi)
{	
	if(posi & 0x80)
		{
		VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"    SAVE P%d-?       ",((posi & 0x7F)/10));
		return;
		}
	VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"    SAVE P%02d?       ",posi);
}

static void SaveInit()
{
	SetRadioStatus(RADIO_SAVING);
	current_save_position = system_position;
	UpdateSaveVfd(current_save_position);
}

void TextDisplayExit(void)
{
	Tuner_Text_Disp_Flag=0; /*退出Text Display显示,恢复原来显示*/
	Tuner_Text_Count=0; /*滚屏显示从第0 个字符开始显示*/
	MsgSetVfdShow(RADIO_ID,0);
}


void SaveRadioStatus(uint8 *pData)
{
	uint8 type;
	
	type = *(pData+1);
	Tuner_Freq_H = *(pData+4);
	Tuner_Freq_L = *(pData+3);	


	if(type == GET_STATUS_ANALOG_ONLY)
	{
		memcpy(&currSeekLevel, (pData+5), 4);
	}
	else if(type == GET_STATUS_DIGITAL_ONLY)
	{
		currProgram = *(pData+5);
		memcpy(&CD_NO, pData+8, 4);
		da_status = *(pData+6);
		Tuner_Signal_Quality = *(pData+7);
	}
	else if(type == GET_STATUS_ALL)
	{
		memcpy(&currSeekLevel, (pData+5), 4);
		currProgram = *(pData+18);
		da_status = *(pData+19);
		Tuner_Signal_Quality = *(pData+20);
	}

    if (currProgram > 8)
    {
        currProgram = 0;
    }
}


void AcqStatusSave (uint8 * pAcqData)
{
	Tuner_Signal_Quality = *(pAcqData + 22);

}

uint8 GetCurrProgram(void)
{
	return currProgram;
}


extern pSTRUCT_QUEUE tuner_queue_pointer;


#if 0
void C11_Tuner_SIS_Change(void)
{
	static unsigned char msg_status = MSG_IDLE, byte_count;
	static unsigned char comm_data;

	if(IsQueueEmpty(tuner_queue_pointer) == QUEUE_NORMAL)	/*队列有数据*/
		{
		DeQueue(tuner_queue_pointer,&comm_data);	
#ifdef _DEBUG_RADIO_TASK		
		debug_printf(" %02X",comm_data);
		//echo(comm_data); /*发接收到的数据到串口打印出来*/
#endif

		if(comm_data == Tuner_MSG_Head1) //0x5A
			{
			msg_status = MSG_Head1;
			}

		switch(msg_status)
			{
			case MSG_Head1:
				if(comm_data == Tuner_MSG_Head1)	/*当第一个字节为0X5A 就可以开始接数据*/
					{
					msg_status = MSG_Head2;
					byte_count = 0;
					}
				else /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK							
					debug_printf("\nMSG_Idle1\n");
#endif
					msg_status=MSG_Idle;
					}
				break;
				
			case MSG_Head2:
				if(comm_data==Tuner_MSG_Head2) /*当第二个字节为0XA5*/
					{
					byte_count++;
					msg_status=MSG_Com_Leg_H;
					}
				else  /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK												
					debug_printf("\nMSG_Idle2\n");
#endif
					msg_status=MSG_Idle;
					}
				break;
				
			case MSG_Com_Leg_H:
				if(comm_data==Tuner_MSG_SIS_Com_Leg_H) /*当第三个字节为0x00*/
					{
					byte_count++;
					msg_status=MSG_Com_Leg_L;
					}
				else /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK																	
					debug_printf("\nMSG_Idle3\n");
#endif
					msg_status=MSG_Idle;
					}
				break;
				
			case MSG_Com_Leg_L:
				if(comm_data==Tuner_MSG_SIS_Com_Leg_L)  /*当第四个字节为0x10*/
					{
					byte_count++;
					msg_status=MSG_Com_Byte;
					}
				else /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK																						
					debug_printf("\nMSG_Idle4\n");
#endif
					msg_status=MSG_Idle;
					}
				break;

			case MSG_Com_Byte:
				if(comm_data==Tuner_MSG_SIS_Com_Byte)  /*当第五个字节为0x10*/
					{
					byte_count++;
					msg_status=MSG_Reply_Pack;
					} 
				else /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK																											
					debug_printf("\nMSG_Idle5\n");
#endif
					msg_status=MSG_Idle;
					}
				break;
				
			case MSG_Reply_Pack:
				byte_count++;
				if(byte_count==6)
					{
#ifdef _DEBUG_RADIO_TASK																																
					//debug_printf("\nFre_H_0=%02X\n",comm_data);
#endif
					Tuner_Freq_H=comm_data;
					}
				if(byte_count==7)
					{
#ifdef _DEBUG_RADIO_TASK																																					
					//debug_printf("\nFre_L_0=%02X\n",comm_data);
#endif
					Tuner_Freq_L=comm_data;
					}
				if(byte_count==11)
					{
#ifdef _DEBUG_RADIO_TASK																																				
					//debug_printf("\nDigital Signal Quality=%02X\n",comm_data);
#endif
					Tuner_Signal_Quality=comm_data;

					}

				if(byte_count==20)
					{
					Tuner_Reply_Flag=0;
					Tuner_Req_Radio_Monitor_Command(1,0,0); /*接收完后清零标志位*/		
					msg_status = MSG_IDLE; /*所有数据接收完毕*/
					}
				break;
			}		
		}
}
#else
void C11_Tuner_SIS_Change(void)
{

	Tune_GetStatus(GET_STATUS_ALL);
}
#endif

unsigned int rec_reply_count;

#if 0
void C11_Tuner_PSD_Change(void)
{
	static unsigned char msg_status = MSG_IDLE, byte_count;
	static unsigned char comm_data;

	if(IsQueueEmpty(tuner_queue_pointer) == QUEUE_NORMAL)	/*队列有数据*/
		{
		DeQueue(tuner_queue_pointer,&comm_data);
		//Uart0Send(0x20);  
#ifdef _DEBUG_RADIO_TASK		
		debug_printf(" %02X",comm_data);      
		//echo(comm_data); /*发接收到的数据到串口打印出来*/
#endif

		if(comm_data == Tuner_MSG_Head1) //0x5A
			{
			msg_status = MSG_Head1;
			}

		switch(msg_status)
			{
			case MSG_Head1:
				if(comm_data == Tuner_MSG_Head1)	/*当第一个字节为0X5A 就可以开始接数据*/
					{
					msg_status = MSG_Head2;
					byte_count = 0;
					}
				else /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK							
					debug_printf("\nMSG_Idle1\n");
#endif
					msg_status=MSG_Idle;
					}
				break;
				
			case MSG_Head2:
				if(comm_data==Tuner_MSG_Head2) /*当第二个字节为0XA5*/
					{
					byte_count++;
					msg_status=MSG_Com_Leg_H;
					}
				else  /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK												
					debug_printf("\nMSG_Idle2\n");
#endif
					msg_status=MSG_Idle;
					}
				break;
				
			case MSG_Com_Leg_H:
				if(comm_data==Tuner_MSG_PSD_Com_Leg_H) /*当第三个字节为0x00*/
					{
					byte_count++;
					msg_status=MSG_Com_Leg_L;
					}
				else /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK																	
					debug_printf("\nMSG_Idle3\n");
#endif
					msg_status=MSG_Idle;
					}
				break;
				
			case MSG_Com_Leg_L:
				if(comm_data==Tuner_MSG_PSD_Com_Leg_L)  /*当第四个字节为0x51*/
					{
					byte_count++;
					msg_status=MSG_Com_Byte;
					}
				else /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK																						
					debug_printf("\nMSG_Idle4\n");
#endif
					msg_status=MSG_Idle;
					}
				break;

			case MSG_Com_Byte:
				if(comm_data==Tuner_MSG_PSD_Com_Byte)  /*当第五个字节为0xF2*/
					{
					byte_count++;
					msg_status=MSG_Reply_Pack;
					} 
				else /*否则重新接收*/
					{
#ifdef _DEBUG_RADIO_TASK																											
					debug_printf("\nMSG_Idle5\n");
#endif
					msg_status=MSG_Idle;
					}
				break;
				
			case MSG_Reply_Pack:
				if((byte_count>=5) ||(byte_count<=84))
					{
					Tuner_Text_Rec_Buffer[byte_count-5]=comm_data;
					byte_count++;
					}
				if(byte_count>=85)
					{
#ifdef _DEBUG_RADIO_TASK	
					//debug_printf("\nrec_reply_count=%d\n",rec_reply_count);
#endif
					Tuner_Reply_Flag=0;
					msg_status = MSG_IDLE; /*所有数据接收完毕*/
					}
				break;
			default :
				break;
			}		
		}
}
#else
typedef enum
{
	hdr_1_search,
	hdr_2_search,
	seq_search,
	instance_search,
	lm_lsb_len_search,
	lm_msb_len_search,
	opcode_search,
	parse_complete
} PARSE_STATE;


#define IDM_HDR                 0xA5
#define MIN_LM_LEN              13      // 13 byte overhead with no data
#define MIN_LM_BYTES_TO_PARSE   7
#define SERIAL_TIMEOUT			15000

void C11_Tuner_PSD_Change(void)
{

#if 0
	static unsigned char msg_status = MSG_IDLE, byte_count;
	static unsigned char comm_data;
	int lm_byte_length;
	uchar msb,lsb;
	BOOL valid;
	
	if(IsQueueEmpty(tuner_queue_pointer) == QUEUE_NORMAL)	/*队列有数据*/
		{
		DeQueue(tuner_queue_pointer,&comm_data);
		//Uart0Send(0x20);  
#ifdef _DEBUG_RADIO_TASK		
		debug_printf(" %02X",comm_data);      
		//echo(comm_data); /*发接收到的数据到串口打印出来*/
#endif

		if(comm_data == IDM_HDR) //0xA5
			{
			msg_status = hdr_1_search;
			}

		switch(msg_status)
			{
			case hdr_1_search:
				if (comm_data == IDM_HDR)
				{
				//	buffidx = 0;
					/** start parsing **/
				///	buffer[buffidx] = comm_data;
					byte_count = 0;
					msg_status = hdr_2_search;
					
				}
				break;

			case hdr_2_search:
				if (comm_data == IDM_HDR)
				{
					byte_count++;
					msg_status = seq_search;
				}
				else
				{
					msg_status = hdr_1_search;
				}
				break;

			case seq_search:
				byte_count++;
				msg_status = instance_search;
				break;

			case instance_search:
				if (comm_data == 1)
				{
					byte_count++;
					msg_status = lm_lsb_len_search;
				}
				else
				{
					msg_status = hdr_1_search;
				}
				break;

			case lm_lsb_len_search:
				byte_count++;
				lsb = comm_data;
				msg_status = lm_msb_len_search;
				break;

			case lm_msb_len_search:
			//	msb = comm_data;
				lm_byte_length = (msb << 8) & 0x0000ff00;
				lm_byte_length |= lsb;
				byte_count++;
				if (lm_byte_length < MIN_LM_LEN)
				{
					msg_status = hdr_1_search;
				}
				else
				{
					msg_status = opcode_search;
				}
				/* Make sure the packet isn't too big */
			//	if (lm_byte_length > CMD_BUFFER_SIZE)
			//		lm_byte_length = CMD_BUFFER_SIZE;

				break;

			case opcode_search:
			//	if (comm_data == opcode)
				{				
					valid = TRUE;
					byte_count++;
				}
				msg_status = parse_complete;
				break;

			case parse_complete:
				if((byte_count>=6) ||(byte_count<=85))
					{
					Tuner_Text_Rec_Buffer[byte_count-5]=comm_data;
					byte_count++;
					}
				if(byte_count>=86)
					{
#ifdef _DEBUG_RADIO_TASK	
					//debug_printf("\nrec_reply_count=%d\n",rec_reply_count);
#endif
					Tuner_Reply_Flag=0;
					msg_status = MSG_IDLE; /*所有数据接收完毕*/
					}				
				break;

			default :
				break;
			}		
		}
	#endif
}
#endif

KEYT PreProcSaveKey(KEYT key)
{
	switch(key)
		{
		case _REMOTE_MEMORY_KEY:
			SaveExit();
			break;
		case _REMOTE_ENTER_KEY:
			MsgSetRadioSavePreset(current_save_position,system_freq);
			SaveExit();
			break;
		case _DIGI_KEY:
			if(IsDigiKeyValid)
				{
				ClearDigiKeyValid;
				current_save_position = CalcRadioPosition(digi_key_value,current_radio_mode,MODE_FIXED);
				MsgSetRadioSavePreset(current_save_position,system_freq);
				SaveExit();
				}
			else
				{
				current_save_position = digi_key_value;
				UpdateSaveVfd(current_save_position | 0x80);
				}
			break;
		case _PANEL_PRESET_UP_KEY:
		case _REMOTE_ARROW_UP_KEY:
			current_save_position &= 0x7F;
			current_save_position = CalcRadioPosition(current_save_position,current_radio_mode,MODE_PLUS);
			UpdateSaveVfd(current_save_position);
			break;
		case _PANEL_PRESET_DN_KEY:
		case _REMOTE_ARROW_DOWN_KEY:
			current_save_position &= 0x7F;
			current_save_position = CalcRadioPosition(current_save_position,current_radio_mode,MODE_MINUS);
			UpdateSaveVfd(current_save_position);
			break;
		}
	return _NO_KEY;
}

KEYT PreProcTextDisplayKey(KEYT key)
{
	switch(key)
		{
		case _REMOTE_TXT_DISPLAY_KEY: /*按下Text Display 按键,退出TextDisplay 显示*/
#ifdef _DEBUG_RADIO_TASK
			//debug_printf("\nText Display Key Up\n");
#endif			
			SetRadioStatus(last_radio_status);
			TextDisplayExit();
			return _NO_KEY;

		case _DIGI_KEY: /*数字按键处理*/
#ifdef _DEBUG_RADIO_TASK
			//debug_printf("\nDigital Key Up\n");
#endif				
			SetRadioStatus(RADIO_IDLE);
			Tuner_Text_Disp_Flag=0; /*退出Text Display显示,恢复原来显示*/
			Tuner_Text_Count=0; /*滚屏显示从第0 个字符开始显示*/
			return _DIGI_KEY;
			
		default: /*其它按键退出TextDisplay,并进行相应按键的处理*/
#ifdef _DEBUG_RADIO_TASK
			//debug_printf("\nOther Key Up--Key=%d\n",key);
#endif				
			SetRadioStatus(RADIO_IDLE);
			TextDisplayExit();
			return key;
		}
}

extern unsigned char HDRadio_FM_Flag;
extern unsigned char HDRadio_HDFM_Flag;
extern unsigned char Pre_value;
KEYT RadioKeyService(KEYT key)
{
	key = PreProcDigitKey(key); /*Process Digital Key*/
	
	if(GetRadioStatus() == RADIO_SAVING) /*电台保存时的按键处理*/
		key = PreProcSaveKey(key);
	if(GetRadioStatus() == RADIO_TEXTING) /*Text Display 下的按键处理*/
		key=PreProcTextDisplayKey(key);

	switch(key)
		{
		case _REMOTE_FM_MODE_KEY:
			if(temp_radio_time)
				{
				if(stereo_mode)
					{
					stereo_mode = 0; /*force mono*/
					}
				else
					{
					stereo_mode = 1; /*auto*/
					}
				}
#ifdef _DEBUG_RADIO_TASK
			//debug_printf("stereo_mode:%d\n",stereo_mode);
#endif
			temp_radio_time = 50;
			VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);
			if(stereo_mode)
				{
				VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"stereo mode: Auto");
				}
			else
				{
				VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"stereo mode: Mono");
				}
			break;
			
		case _REMOTE_TUNER_SCAN_KEY:
			MsgSetRadioSearch(RADIO_SCAN,1); /*send message to go on scaning*/
			break;
			
		case _REMOTE_TUNER_SEEK_UP_KEY:
			MsgSetRadioSearch(RADIO_SEARCH_UP,1);	/*send message to go on searching*/
			break;
		case _REMOTE_TUNER_SEEK_DOWN_KEY:
			MsgSetRadioSearch(RADIO_SEARCH_DN,1);	/*send message to go on searching*/
			break;
		case _REMOTE_FM_KEY:
			if(current_radio_mode == _FM_MODE)
				break;
			else
				RadioInit(_FM_MODE);
			break;
		case _REMOTE_AM_KEY:
			if(current_radio_mode==_AM_MODE)
				break;
			else
				RadioInit(_AM_MODE);				
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
		case _REMOTE_HD_FM_KEY:
			if(current_radio_mode==_HD_FM_MODE)
				break;
			else
				RadioInit(_HD_FM_MODE);		
			break;
		case _REMOTE_HD_AM_KEY:
			if(current_radio_mode==_HD_AM_MODE)
				break;
			else
				RadioInit(_HD_AM_MODE);
			break;
			
		case _REMOTE_ARROW_LEFT_KEY:
		case _PANEL_ARROW_LEFT_KEY:
			MsgSetRadioSearch(RADIO_SEEK_DN,0);
			break;
		case _REMOTE_ARROW_RIGHT_KEY:
		case _PANEL_ARROW_RIGHT_KEY:
			MsgSetRadioSearch(RADIO_SEEK_UP,0);
			break;
		case _PANEL_PRESET_UP_KEY:
		case _REMOTE_ARROW_UP_KEY:
			system_position = CalcRadioPosition(system_position,current_radio_mode,MODE_PLUS);
			MsgSetRadioLoadPreset(system_position);
			MsgSetVfdShow(RADIO_ID,0);
			break;
		case _PANEL_PRESET_DN_KEY:
		case _REMOTE_ARROW_DOWN_KEY:
			system_position = CalcRadioPosition(system_position,current_radio_mode,MODE_MINUS);
			MsgSetRadioLoadPreset(system_position);
			MsgSetVfdShow(RADIO_ID,0);
			break;
			
		case _REMOTE_TXT_DISPLAY_KEY:
			if((current_radio_mode==_FM_MODE)||(current_radio_mode==_HD_FM_MODE)) /*只有FM 和HDFM 有这个功能*/
				{
				MsgTextDisplay(1); /*mode=1--显示Text Display; mode=0 --退出Text Display*/
				}
			break;
			
		case _DIGI_KEY:
			if(IsDigiKeyValid)
				{
				ClearDigiKeyValid;
				system_position = CalcRadioPosition(digi_key_value,current_radio_mode,MODE_FIXED);
				MsgSetRadioLoadPreset(system_position);
				MsgSetVfdShow(RADIO_ID,0);
				}
			else
				{
				RadioDisplay(system_freq,current_radio_mode,digi_key_value | 0x80);
				}
			break;
		case _REMOTE_MEMORY_KEY:
			SaveInit();
			break;
		case _REMOTE_MUTE_KEY:
			MsgSetMute(ZONE1_ID,MODE_TOGGLE,0);
			MsgSetVfdShow(RADIO_ID,0);
			break;
		case _PANEL_INPUT1_KEY:
		case _REMOTE_INPUT1_KEY:
		case _PANEL_INPUT2_KEY:
		case _REMOTE_INPUT2_KEY:
		case _PANEL_INPUT3_KEY:
		case _REMOTE_INPUT3_KEY:
		case _PANEL_INPUT4_KEY:
		case _REMOTE_INPUT4_KEY:
		case _PANEL_INPUT5_KEY:
		case _REMOTE_INPUT5_KEY:
		case _PANEL_INPUT6_KEY:
		case _REMOTE_INPUT6_KEY:
		case _PANEL_INPUT7_KEY:
		case _REMOTE_INPUT7_KEY:
		case _PANEL_INPUT8_KEY:
		case _REMOTE_INPUT8_KEY:
		case _PANEL_7CH1_INPUT_KEY:
		case _REMOTE_7CH1_IN_KEY:			
		case _REMOTE_POWER_OFF_KEY:
		case _PANEL_POWER_KEY:
			RadioExit();
			return key;	
		case _PANEL_HDMI1_KEY:	
		case _REMOTE_HDMI1_KEY:
		case _PANEL_HDMI2_KEY:	
		case _REMOTE_HDMI2_KEY:			
		case _PANEL_HDMI3_KEY:	
		case _REMOTE_HDMI3_KEY:
		case _PANEL_HDMI4_KEY:	
		case _REMOTE_HDMI4_KEY:			

		case _PANEL_TVSOUND_KEY:	 // jian add in 10-10-13

			RadioExit();
			Pre_value=8; /*DSP 重新复位*/
			SetDspPower(0); /*DSP 保持复位状态*/	
			return key;
			
		case _PANEL_ENCODER_MINUS_KEY:
		case _PANEL_ENCODER_PLUS_KEY:
		case _REMOTE_VOLUME_UP_KEY:
		case _REMOTE_VOLUME_DOWN_KEY:
		case _PANEL_ENTER_KEY:
		case _REMOTE_SETUP_KEY:
		case _REMOTE_BRIGHT_KEY:
			return key;

		case _REMOTE_PLIIX_KEY:
		case _PANEL_PLIIX_KEY:
		case _REMOTE_SUR_MODE_KEY:  
		case _PANEL_SUR_MODE_KEY:
		case _REMOTE_NEO96_24_KEY:
		case _PANEL_NEO6_KEY:
		case _REMOTE_CES7_1_KEY:
		case _PANEL_CES7_1_KEY:
			MsgSetVfdShow(RADIO_ID,0); /*用于刷新数据流信息*/
			return key;		
		}
	return _NO_KEY;
}

TASK_RESULT RadioProcMessage(unsigned char *buffer)
{
	FREQT temp_freq;
	KEYT comm_key;
	uchar seek_Status;
	
	switch(buffer[0])
		{
		case MSG_KeyPress:
			ProcDimerOff(buffer[1]);		
			SetVfdOwner(1);  	
			if((GetRadioStatus() == RADIO_SEARCHING) || (GetRadioStatus() == RADIO_SCANING))	/*如果radio在SEARCHING状态，要停止SEARCHING*/
				{
				SetRadioStatus(RADIO_IDLE);	/*exit SEARCHING, return to IDLE*/
				MsgSetRadioSearch(RADIO_STOP,0); /*send to top of message queue*/
				return TRUE;
				}

			comm_key = buffer[1];
			comm_key = RadioKeyService(comm_key); /*key for zone1*/
			
			if(comm_key != _NO_KEY)
				{
					return default_proc(buffer);
				}
			return TRUE;
		case MSG_Show:
			//if(GetCurFocusTid() != RADIO_ID) //Dele by cjm 2009-03-30
			//	return FALSE;

			switch(buffer[1])
				{
				case 0: /*General Display*/
					RadioDisplay(system_freq,current_radio_mode,system_position);
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);
					VfdUpdateStreamType(sys_flag.stream_type,sys_flag.stream_subtype);
					return TRUE;
				case 1: /*Text Display*/
					GetTextDispBuffer(Tuner_Text_Count);
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2); /*在第二行显示text diplay*/
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"%s",Tuner_Text_Disp_Buffer);
					break;
				}
		case MSG_RadioSearch:			
			//if(GetCurFocusTid() != RADIO_ID) //Dele by cjm 2009-03-30
			//	return FALSE;  

			switch(buffer[1])
				{
				case RADIO_SEARCH_UP:							
#ifdef _DEBUG_RADIO_TASK
					debug_printf("\nRADIO_SEARCH_UP");
#endif					
					if(buffer[2] == 1)	/*Init to search mode*/
						{
						if(GetRadioStatus() == RADIO_SEARCHING)
							return FALSE;

						//Tuner_Mute_Command(Tune_Mute);
						SetRadioStatus(RADIO_SEARCHING);
						}

					if(GetRadioStatus() != RADIO_SEARCHING)
						return FALSE;

					if((current_radio_mode==_FM_MODE) ||(current_radio_mode==_AM_MODE))
						{
						if(Tuner_Seek_Command(Seek_Up) == 0)
							seek_Status = 0;
						else
							seek_Status = 1;

						}
					else
						{
						if(Tuner_HDSEEK_Command(Seek_Up) == 0)
							seek_Status = 0;
						else
							seek_Status = 1;
						}
				//	DelayMs(50);		
					
				//	Tuner_Reply_Flag=1;				
				//	Tuner_Req_Radio_Monitor_Command(1,1,0); /*向HD Radio 发送命令设置标志位读取SIS CHANGE 包*/	
				//	DelayMs(100);
				//	do{
				//		C11_Tuner_SIS_Change();
				//		}while(Tuner_Reply_Flag);				

					if((current_radio_mode==_FM_MODE) ||(current_radio_mode==_AM_MODE))
						{
						system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);
						Tuner_Write_Freq_Command(current_radio_mode,system_freq);	
						MsgSetVfdShow(RADIO_ID,0);
						if(seek_Status)
							MsgSetRadioSearch(RADIO_STOP,0);
						else
							MsgSetRadioSearch(RADIO_SEARCH_UP,0);
						}
					else /*HDAM/HDFM*/
						{
						//if(Tuner_Signal_Quality!=0) /*HDAM/HDFM信号质量不为0表示有电台*/
						//	{
						//	Tuner_Signal_Quality=0;
						system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);
						Tuner_Write_Freq_Command(current_radio_mode,system_freq);	  									
						//	}	
						MsgSetVfdShow(RADIO_ID,0);  
						if(seek_Status)						
							MsgSetRadioSearch(RADIO_STOP,0);			
						else
							MsgSetRadioSearch(RADIO_SEARCH_UP,0);

						}
					break;
				case RADIO_SEARCH_DN:		
#ifdef _DEBUG_RADIO_TASK
					debug_printf("\nRADIO_SEARCH_DN");  
#endif	
					if(buffer[2] == 1)	/*Init to search mode*/
						{
						if(GetRadioStatus() == RADIO_SEARCHING)
							return FALSE;
						
						//Tuner_Mute_Command(Tune_Mute);
						SetRadioStatus(RADIO_SEARCHING);
						}

					if(GetRadioStatus() != RADIO_SEARCHING)
						return FALSE;

					if((current_radio_mode==_FM_MODE) ||(current_radio_mode==_AM_MODE))
						{
						if(Tuner_Seek_Command(Seek_Down) == 0)
							seek_Status = 0;
						else
							seek_Status = 1;
						}
					else
						{
						if(Tuner_HDSEEK_Command(Seek_Down) == 0)
							seek_Status = 0;
						else
							seek_Status = 1;
						}
					
				//	DelayMs(50);

				//	Tuner_Reply_Flag=1;
				//	Tuner_Req_Radio_Monitor_Command(1,1,0); /*想HD Radio 发送命令设置标志位读取SIS CHANGE 包*/
				//	DelayMs(100);
				//	do{
				//		C11_Tuner_SIS_Change();
				//		}while(Tuner_Reply_Flag);	
					
					if((current_radio_mode==_FM_MODE) ||(current_radio_mode==_AM_MODE))
						{
						system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);
						Tuner_Write_Freq_Command(current_radio_mode,system_freq);	
						MsgSetVfdShow(RADIO_ID,0);

						if(seek_Status)						
							MsgSetRadioSearch(RADIO_STOP,0);			
						else
							MsgSetRadioSearch(RADIO_SEARCH_DN,0);
						}
					else /*HDAM/HDFM*/
						{
						//if(Tuner_Signal_Quality!=0) /*HDAM/HDFM信号质量不为0表示有电台*/
						//	{
						//	Tuner_Signal_Quality=0;
						system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);
						Tuner_Write_Freq_Command(current_radio_mode,system_freq);											
						//	}  
						MsgSetVfdShow(RADIO_ID,0);  

						if(seek_Status)						
							MsgSetRadioSearch(RADIO_STOP,0);			
						else
							MsgSetRadioSearch(RADIO_SEARCH_DN,0);		
						}
					break;
				case RADIO_SEEK_UP:
					Tuner_Req_Tune_Up(0); /*Solve stuck on 103.4 station question */
					system_freq = CalcRadioFreq(system_freq,current_radio_mode,1);	/*seek up*/
					Tuner_Write_Freq_Command(current_radio_mode,system_freq);
					MsgSetVfdShow(RADIO_ID,0);
					break;
				case RADIO_SEEK_DN:
					Tuner_Req_Tune_Down(0); /*Solve stuck on 103.4 station question */     
					system_freq = CalcRadioFreq(system_freq,current_radio_mode,0);	/*seek down*/
					Tuner_Write_Freq_Command(current_radio_mode,system_freq);
					MsgSetVfdShow(RADIO_ID,0);
					break;
				case RADIO_SCAN:					
					if(buffer[2] == 1)	/*init*/
						{
						if(GetRadioStatus() == RADIO_SCANING)
							return FALSE;
						
						SetRadioStatus(RADIO_SCANING);
						switch(current_radio_mode)
							{
							case _FM_MODE:
								system_freq = MIN_FM_FREQ;
								break;
							case _AM_MODE:
								system_freq = MIN_AM_FREQ;
								break;
							case _HD_FM_MODE:
								system_freq=MIN_HDFM_FREQ;
								break;
							case _HD_AM_MODE:
								system_freq=MIN_HDAM_FREQ;
								break;						
							}					
						system_position = 0;
						}
					if(GetRadioStatus() != RADIO_SCANING)
						return FALSE;

					Tuner_Write_Freq_Command(current_radio_mode,system_freq);
				//	DelayMs(50);
					RadioDisplay(system_freq,current_radio_mode,system_position);

					if((current_radio_mode==_FM_MODE) ||(current_radio_mode==_AM_MODE))
						{
						if(Tuner_Seek_Command(Seek_Up) == 0)
							seek_Status = 0;
						else
							seek_Status = 1;
						}
					else
						{
						 if(Tuner_HDSEEK_Command(Seek_Up) == 0)
						 	seek_Status = 0;
						else
							seek_Status = 1;
						}
					
					system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);
					RadioDisplay(system_freq,current_radio_mode,system_position);					
					
					if(seek_Status == 0)
						{

						MsgSetRadioSearch(RADIO_SCAN,0);
						break;
						}
					

					
					if((current_radio_mode==_FM_MODE) ||(current_radio_mode==_AM_MODE))
						{
					//	system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);
						SaveFmPreset(system_position,system_freq,current_radio_mode);
						DelayMs(500);
						system_position += 1;
						}
					else
						{
						if(Tuner_Signal_Quality!=0)
							{
							Tuner_Signal_Quality=0;
						//	system_freq=CalcCurrenTunerFreq(current_radio_mode,Tuner_Freq_H, Tuner_Freq_L);
							SaveFmPreset(system_position,system_freq,current_radio_mode);
							DelayMs(500);
							system_position += 1;							
							}
						else
							{
							system_freq = CalcRadioFreq(system_freq,current_radio_mode,1);
							MsgSetRadioSearch(RADIO_SCAN,0);
							DelayMs(30);
							}
						}
						
					if(ISValidTunerPostion(current_radio_mode)==TRUE)
						{
						//system_freq = CalcRadioFreq(system_freq,current_radio_mode,1);
						MsgSetRadioSearch(RADIO_SCAN,0);
						DelayMs(50);
						}
					else
						{
						system_position = 0;
						LoadRadioPreset(system_position,&system_freq,current_radio_mode);
						Tuner_Write_Freq_Command(current_radio_mode,system_freq);
						SetRadioStatus(RADIO_IDLE);
						MsgSetVfdShow(RADIO_ID,0);
						}				
					break;
					
				case RADIO_STOP:
					SetRadioStatus(RADIO_IDLE);	/*exit SEARCHING, return to IDLE*/
					Tuner_Mute_Command(Tune_Unmute);
					break;
				}			
			break;
		case MSG_RadioPreset:
			switch(buffer[1])
				{
				case 0:	/*load preset*/
					LoadRadioPreset(buffer[2],&temp_freq,current_radio_mode);
					system_position = buffer[2];
#ifdef _DEBUG_RADIO_TASK
					debug_printf("\nsystem_position=%d",system_position);  
#endif					
					switch(current_radio_mode)
						{
						case _FM_MODE:
							fm_position = system_position;
							break;
						case _AM_MODE:
							am_position = system_position;
							break;
						case _HD_FM_MODE:    
							HDFM_position=system_position;
							break;
						case _HD_AM_MODE:
							HDAM_position=system_position;
							break;
						}
					system_freq = temp_freq;
					Tuner_Write_Freq_Command(current_radio_mode,system_freq);
					break;
				 case 1:	/*save preset*/					
					temp_freq = get_freq(buffer + 3);
					system_position = CalcRadioPosition(buffer[2],current_radio_mode,MODE_FIXED);
					SaveFmPreset(system_position,temp_freq,current_radio_mode);
#ifdef _DEBUG_RADIO_TASK
					//debug_printf("save:%d->%d\n",buffer[2],temp_freq);
#endif
					break;
				}
			break;
		case MSG_RadioFreq:			
			//if(GetCurFocusTid() != RADIO_ID) //Dele by cjm 2009-03-30
			//	return FALSE;
			switch(buffer[1])
				{
				case _FM_MODE:
					temp_freq = get_freq(buffer + 2);
					if(IsFreqValid(temp_freq, _FM_MODE) == TRUE)
						{
						if(current_radio_mode == _FM_MODE)
							{
							system_freq = temp_freq;
							Tuner_Write_Freq_Command(current_radio_mode,system_freq);							
							}
						//else
						//	{
							//fm_freq = temp_freq;
							//RadioChangeMode(_FM_MODE);							
						//	}
						MsgSetVfdShow(RADIO_ID,0);
						}
					break;				
				case _AM_MODE:
					temp_freq = get_freq(buffer + 2);
					if(IsFreqValid(temp_freq, _AM_MODE) == TRUE)
						{
						if(current_radio_mode == _AM_MODE)
							{
							system_freq = temp_freq;
							Tuner_Write_Freq_Command(current_radio_mode,system_freq);						
							}
						//else
						//	{
							//am_freq = temp_freq;
							//RadioChangeMode(_AM_MODE);							
						//	}
						MsgSetVfdShow(RADIO_ID,0);
						}
					break;
				case _HD_FM_MODE:	
					temp_freq = get_freq(buffer + 2);
					if(IsFreqValid(temp_freq, _HD_FM_MODE) == TRUE)
						{
						if(current_radio_mode == _HD_FM_MODE)
							{
							system_freq=temp_freq;
							Tuner_Write_Freq_Command(current_radio_mode,system_freq);							
							}
						//else
						//	{
							//HDFM_freq=temp_freq;
							//RadioChangeMode(_HD_FM_MODE);							
						//	}
						MsgSetVfdShow(RADIO_ID,0);
						}
					break;
				case _HD_AM_MODE:			
					temp_freq = get_freq(buffer + 2);
					if(IsFreqValid(temp_freq, _HD_AM_MODE) == TRUE)
						{
						if(current_radio_mode == _HD_AM_MODE)
							{
							system_freq=temp_freq;
							Tuner_Write_Freq_Command(current_radio_mode,system_freq);							
							}
						//else
						//	{
							//HDAM_freq=temp_freq;
							//RadioChangeMode(_HD_AM_MODE);							
						//	}
						MsgSetVfdShow(RADIO_ID,0);
						}					
					break;
				}
			break;			

		case MSG_HDRADIO_Text:
#ifdef _DEBUG_RADIO_TASK
			debug_printf("\nKey Press Text Display:\n");
#endif
			key_enable = 0; /*按键不起作用*/
			if(buffer[1])  
				{
				last_radio_status=GetRadioStatus(); /*进入Text Display前保存当前的Radio Status*/
				SetRadioStatus(RADIO_TEXTING); /*设置RADIO 状态*/
				
				VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2); /*清空第二行显示*/
				VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"     Update...      ");
				DelayMs(2000); /*延时2S 等待电台信号稳定下来才开始读取Text 信息*/

				#if 0
				Tuner_Reply_Flag=1;	/*准备接收PSD CHANGE 包*/
				Tuner_Req_Radio_Monitor_Command(0,0,1); /*向HD Radio 发送命令设置标志位读取PSD CHANGE 包*/
				do{
					C11_Tuner_PSD_Change();  
					}while(Tuner_Reply_Flag);    
				Tuner_Req_Radio_Monitor_Command(1,0,0); /*接收完后清PSD 包标志位*/	
				#endif
				
				Radio_Text_Display();
				Tuner_Text_Disp_Flag=1; /*循环显示Text Display 标志位*/
				}
			   key_enable = 1; /*按键起作用*/      
			   break;
		}
	return FALSE;
}



void RadioLoop(void)
{
	uchar st_mode;
	static uchar old_st_mode,cnter = 0;
//        static uchar radio_getstatus_cnt;
	
	if(GetCurFocusTid() == RADIO_ID)
		{
		if(radio_time_tick)
			{
			radio_time_tick = 0;
//			if(radio_getstatus_cnt == 10)
				{
	//			radio_getstatus_cnt = 0;
				if(GetRadioStatus() != RADIO_SEARCHING)				
					Radio_GetStatus();// jian add in 11-08-18
				}
	//		else
		//		radio_getstatus_cnt ++;		
			

			if(temp_radio_time)
				{
				temp_radio_time --;
				if(temp_radio_time == 0)
					{
					//current_st_mode = SenseTunerSt();
					MsgSetVfdShow(RADIO_ID,0);
					}
				}
			else
				{
				//st_mode = SenseTunerSt();
				if(old_st_mode != st_mode)
					{
					cnter ++;
					if(cnter==10)
						{
						old_st_mode = st_mode;
						current_st_mode = st_mode;
						MsgSetVfdShow(RADIO_ID,0);
						}
					if(cnter > 10)
						cnter = 11;
					}
				else
					{
					cnter = 0;
					}
				}
			}
		}
}

