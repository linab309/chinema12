#include "macro.h"
#include "task.h"
#include "debug.h"
#include "message.h"
#include "keydefine.h"
#include "setup_func.h"
#include "setup_menu.h"

#include "vfd_disp.h"
#include "radio_task.h"

#include "..\driver\channel.h"
#include "..\driver\volume.h"
#include "..\driver\tuner.h"
#include "..\driver\ioport.h"
#include "..\driver\Eeprom.h"
#include "..\driver\Max335.h"
#include "..\driver\Hdmi.h"

#include <stdio.h>
#include "..\cs495300\Dspdrv.h"

#define TEMP_DISPLAY_TIME 30 

#ifdef _DEBUG
#define _DEBUG_ZONE2_TASK
#endif

char * Zone2SrcName;

extern unsigned char HD_Radio_Power_Flag;
extern const unsigned int zone2_analog_in_table[];  
extern unsigned char system_position;	/*当前radioPRESET，可以是AM或者FM*/
extern FREQT system_freq;
extern unsigned char current_radio_mode;	/*当前radio模式，FM或者AM*/

extern unsigned char CalcRadioPosition(unsigned char posi,unsigned char radio_mode,unsigned char mode);
extern BOOL MsgSetRadioLoadPreset(unsigned char position);

BOOL IsZone2PowerOn()
{
	if(sys_flag.system_zone2_status == POWER_ON)
		return TRUE;
	return FALSE;
}

BOOL IsZone2VolumeValid(short volume)
{
	if((volume <= ZONE2_MAX_VOLUME) && (volume >= ZONE2_MIN_VOLUME))
		return TRUE;
	return FALSE;
}

void SetZ2MasterVolumeValue(unsigned char volume)
{
	EepromWriteByte(Master_Volume_Level,volume);   
	sys_flag.zone2_volume = volume_table[volume];
}

void ProcZone2PowerOn()
{
#ifdef _DEBUG_ZONE2_TASK
	debug_printf("zone2 power on\n");
#endif

	if(sys_flag.system_status == POWER_OFF)
		{
		SystemPowerOn();
		}

	if(sys_flag.system_zone2_status != POWER_ON)
		{
		sys_flag.zone2_volume = POWER_ON_DEFAULT_VOLUME;	
		sys_flag.zone2_channel=INPUT_1; //add by cjm 
		DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000000); /*Downmix 使能,从DA02_Channel 1/2 输出*/					
		Zone2SelectChannel(sys_flag.zone2_channel); 
		Zone2SrcName = GetSrcName(sys_flag.zone2_channel);
		MsgSetVfdShow(ZONE2_ID,0);
		SetZ2MasterVolumeValue(EepromReadByte(Z2_Master_Volume_Level)); /*开机按照客户的设置音量调节*/
		MsgSetVolume(ZONE2_ID,MODE_FIXED,sys_flag.zone2_volume);
		MsgSetMute(ZONE2_ID, MODE_FIXED, OFF);
//		if((SystemTriggerMode == 1) ||(SystemTriggerMode == 2))
		if(SystemTriggerMode == 2)
			{
			SetTrigerOut(1);	//Triger out 1
			}			
		if(SystemTriggerMode1 == 2)
			{
			SetTrigerOut1(1);	//Triger out 1
			}			
		if(SystemTriggerMode2 == 2)
			{
			SetTrigerOut2(1);	//Triger out 1
			}			

		}
	sys_flag.system_zone2_status = POWER_ON;
}

void ProcZone2PowerOff()
{
#ifdef _DEBUG_ZONE2_TASK
	debug_printf("zone2 power off\n");
#endif

	if(sys_flag.system_zone2_status != POWER_OFF)
		{
		Zone2Mute(ON);
//		if((SystemTriggerMode == 1) ||(SystemTriggerMode == 2))
	//	if(SystemTriggerMode == 2)
			{
			SetTrigerOut(0);	//Triger out 1
			}			
	//	if(SystemTriggerMode1 == 2)
			{
			SetTrigerOut1(0);	//Triger out 1
			}			
	//	if(SystemTriggerMode2 == 2)
			{
			SetTrigerOut2(0);	//Triger out 1
			}			

		}

	if(!IsCurFocusTid(RADIO_ID)) /*Zone1 没开启HD Radio 功能时,如果Zone2开启了,在关机时关掉HD Radio*/
		{
		if(HD_Radio_Power_Flag==1)
			{
			HD_Radio_Power_Flag=0;
			HDRadioPower(OFF); /*关闭HD Radio 电源*/
			}
		}	

	if(sys_flag.system_zone1_status != POWER_ON)
		{
		SetVfdOwner(1);
		SystemPowerOff();
		}
	else
		{
		VfdTempDispBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);
		VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE_ALL,10,"Z2:    -OFF-        ");
		}
	sys_flag.system_zone2_status = POWER_OFF;
}

void Zone2ProcInputSelect(unsigned char direction)
{
	if(!IsCurFocusTid(RADIO_ID)) /*Zone1 没开启HD Radio 功能时,再切换到其它输入时,关闭HD Radio*/
		{
		if(HD_Radio_Power_Flag==1)
			{
			HD_Radio_Power_Flag=0;
			HDRadioPower(OFF); /*关闭HD Radio 电源*/
			}
		}

	sys_flag.zone2_channel = direction;	
	EepromWriteByte(ZONE2_OUT_POSITION, sys_flag.zone2_channel);   
	Zone2SrcName = GetSrcName(sys_flag.zone2_channel);
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000000); /*Downmix 使能,从DA02_Channel 1/2 输出*/					
	Zone2SelectChannel(sys_flag.zone2_channel);
	MsgSetVfdShow(ZONE2_ID,0);
}

extern unsigned char HD_Radio_Power_Flag;

#define MAX335_CHIP8 0x0700 
#define INPUT_SELECT_Zone2_Downmix   	(MAX335_CHIP8 | 0X06) //BIT2/BIT1-Zone2 Downmix 

BOOL Zone2RadioMode(unsigned char mode)
{
	if(HD_Radio_Power_Flag==0)
		{
		HD_Radio_Power_Flag=1;
		HDRadioPower(ON); /*HD Radio 上电*/
		VfdTempDispBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);
		VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,TEMP_DISPLAY_TIME," Power On HD Radio  ");
		VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,TEMP_DISPLAY_TIME,"    Please Wait!    ");		
		DelayMs(3000); /*显示3 秒--这个延时是为了HD Radio上电初始化时间*/
		}

	switch(mode)
		{
			case _FM_MODE:
				current_radio_mode = _FM_MODE;
				system_position = fm_position;
				system_freq = fm_freq; 
				break;
			case _AM_MODE:
				current_radio_mode = _AM_MODE;
				system_position = am_position;
				system_freq = am_freq; 
				break;
			case _HD_FM_MODE:
				current_radio_mode=_HD_FM_MODE;
				system_position=HDFM_position;
				system_freq = HDFM_freq; 
				break;
			case _HD_AM_MODE:
				current_radio_mode=_HD_AM_MODE;
				system_position=HDAM_position;		
				system_freq = HDAM_freq;
				break;
		}	

	InitTuner(current_radio_mode);
	if((current_radio_mode==_FM_MODE)||(current_radio_mode==_AM_MODE))
		{
		LoadRadioPreset(system_position,&system_freq,current_radio_mode);
		Tuner_Write_Freq_Command(current_radio_mode,system_freq);		  		
		}
	Tuner_Mute_Command(Tune_Unmute);
	Tuner_Volume_Init(); /*Set HD Radio volume*/	
	
	return TRUE;
}

static void Zone2ProcessKey(KEYT key)
{
	unsigned char value;
	if(sys_flag.system_zone2_status == POWER_OFF) /*Power off key process*/
		{
		switch(key)
			{
			case _REMOTE_Z2_POWER_ON_KEY:
				MsgSetPower(ZONE2_ID,1); /*Set power on*/
				break;
			default:
				break;
			}
		}
	else	 /*Power on key process*/
		{
		switch(key)
			{
			case _REMOTE_Z2_POWER_OFF_KEY:
				MsgSetPower(ZONE2_ID,0); /*Set power off*/
				break;
			case _REMOTE_Z2_AM_KEY:
				sys_flag.zone2_channel=INPUT_TUNER;
				if(!IsCurFocusTid(RADIO_ID))
					{
					Zone2RadioMode(_AM_MODE);
					}
				Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
				SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
				UpdateMax335();	/*Update MAX335*/
				DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/
				MsgSetVfdShow(ZONE2_ID,0);		
				break;
			case _REMOTE_Z2_FM_KEY:	
				sys_flag.zone2_channel=INPUT_TUNER;
				if(!IsCurFocusTid(RADIO_ID))
					{
					Zone2RadioMode(_FM_MODE);    
					}
				Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
				SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
				UpdateMax335();	/*Update MAX335*/
				DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/
				MsgSetVfdShow(ZONE2_ID,0);			
				break;
			case _REMOTE_Z2_HD_AM_KEY:  
				sys_flag.zone2_channel=INPUT_TUNER;
				if(!IsCurFocusTid(RADIO_ID))
					{
					Zone2RadioMode(_HD_AM_MODE);  
					}
				Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
				SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
				UpdateMax335();	/*Update MAX335*/
				DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/				
				MsgSetVfdShow(ZONE2_ID,0);
				break;
			case _REMOTE_Z2_HD_FM_KEY:
				sys_flag.zone2_channel=INPUT_TUNER;
				if(!IsCurFocusTid(RADIO_ID)) 
					{
					Zone2RadioMode(_HD_FM_MODE);  
					}
				Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
				SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
				UpdateMax335();	/*Update MAX335*/
				DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/				
				MsgSetVfdShow(ZONE2_ID,0);
				break;				  
			case _REMOTE_Z2_PRESET_UP_KEY:							
				if(!IsCurFocusTid(RADIO_ID))
					{
					if(sys_flag.zone2_channel == INPUT_TUNER)
						{
						system_position = CalcRadioPosition(system_position,current_radio_mode,MODE_PLUS);
						MsgSetRadioLoadPreset(system_position);
						}
					MsgSetVfdShow(ZONE2_ID,0);
					}			
				break;
			case _REMOTE_Z2_PRESET_DOWN_KEY:			
				if(!IsCurFocusTid(RADIO_ID))
					{
					if(sys_flag.zone2_channel == INPUT_TUNER)
						{
						system_position = CalcRadioPosition(system_position,current_radio_mode,MODE_MINUS);
						MsgSetRadioLoadPreset(system_position);
						}
					MsgSetVfdShow(ZONE2_ID,0);  
					}			
				break;
			case _REMOTE_Z2_MUTE_KEY:
				MsgSetMute(ZONE2_ID, MODE_TOGGLE, 0);
				break;
			case _REMOTE_Z2_VOLUME_UP_KEY:
				MsgSetVolume(ZONE2_ID,MODE_PLUS,0);
				break;
			case _REMOTE_Z2_VOLUME_DOWN_KEY:
				MsgSetVolume(ZONE2_ID,MODE_MINUS,0);
				break;
			case _REMOTE_Z2_INPUT1_KEY:
			case _REMOTE_Z2_INPUT2_KEY:
			case _REMOTE_Z2_INPUT3_KEY:
			case _REMOTE_Z2_INPUT4_KEY:
			case _REMOTE_Z2_INPUT5_KEY:
			case _REMOTE_Z2_INPUT6_KEY:
			case _REMOTE_Z2_INPUT7_KEY:
			case _REMOTE_Z2_INPUT8_KEY:
			case _REMOTE_Z2_7CH1_IN_KEY:
				MsgSetInputSource(ZONE2_ID, key - _REMOTE_Z2_INPUT1_KEY);
				break;
			case _REMOTE_Z2_HDMI1_KEY:
					sys_flag.zone2_channel = INPUT_HDMI;
				if((sys_flag.zone1_channel >= INPUT_HDMI1)&&(sys_flag.zone1_channel <= INPUT_HDMI4))						
					{
					value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
					if(value==5) /*当为6表示Zone1的HDMI 流已经稳定*/
						{
						Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
						DigiSendByte(ADDR_ZONE2,0x00);/*通道切到HDMI口*/
						#if 0
						if(sys_flag.zone1_channel == INPUT_HDMI1)
							{
							sys_flag.zone2_channel=INPUT_HDMI1;
							Zone2SrcName = GetSrcName(INPUT_HDMI1);
							}
						else
							{
							sys_flag.zone2_channel=INPUT_HDMI3;
							Zone2SrcName = GetSrcName(INPUT_HDMI3);
							} // jian add in 10-11-12
						#endif
						/*debug_printf("_REMOTE_Z2_HDMI1_KEY\n");*/
						DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix 使能,从DA02_Channel 1/2 输出*/													
						SetMax335Switch(INPUT_SELECT_Zone2_Downmix,1);/*打开downmix开关*/
						UpdateMax335();	/*Update MAX335*/

						}

					}

					Zone2SrcName = "HDMI";
					MsgSetVfdShow(ZONE2_ID,0);

				break;
			case _REMOTE_Z2_HDMI2_KEY:
				if(sys_flag.zone1_channel == ARC_CHANNEL)
					{
					value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
					if(value==5) /*当为6表示Zone1的HDMI 流已经稳定*/
						{
						Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
						#if 0
						if(sys_flag.zone1_channel == INPUT_HDMI2)
							{
							sys_flag.zone2_channel=INPUT_HDMI2;
							Zone2SrcName = GetSrcName(INPUT_HDMI2);
							}
						else
							{
							sys_flag.zone2_channel=INPUT_HDMI4;
							Zone2SrcName = GetSrcName(INPUT_HDMI4);
							}
						#endif	
						// jian add in 10-11-12 

						DigiSendByte(ADDR_ZONE2,0X00);/*通道切到HDMI口*/
						SetMax335Switch(INPUT_SELECT_Zone2_Downmix,1);/*打开downmix开关*/
						}
					}
						Zone2SrcName = GetSrcName(ARC_CHANNEL);
						sys_flag.zone2_channel=ARC_CHANNEL;
						MsgSetVfdShow(ZONE2_ID,0);	  									
				break;
			}
		}
}

TASK_RESULT Zone2ProcMessage(unsigned char *buffer)
{
	short temp_volume;
	KEYT zone2_key;
	switch(buffer[0])
		{
		case MSG_KeyPress:
			zone2_key = buffer[1];
			ProcDimerOff(zone2_key);
			Zone2ProcessKey(zone2_key);
			break;
		case MSG_Show:
			if(IsZone2PowerOn())
				{
				VfdTempDispBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);
				if(sys_flag.zone2_channel == INPUT_TUNER)
					{
					switch(current_radio_mode)
						{
						case _FM_MODE:
							VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,TEMP_DISPLAY_TIME,"Z2: FM %3d.%01dMHz  ",((system_freq / 100)/10),((system_freq/100) % 10));
							break;
						case _AM_MODE:
							VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,TEMP_DISPLAY_TIME,"Z2: AM %4dKHz   ",system_freq);
							break;
						case _HD_FM_MODE:
							VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,TEMP_DISPLAY_TIME,"Z2: HDFM %3d.%01dMHz  ",((system_freq / 100)/10),((system_freq/100) % 10));
							break;
						case _HD_AM_MODE:
							VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,TEMP_DISPLAY_TIME,"Z2: HDAM %4dKHz   ",system_freq);
							break;
						}
					}
				else
					VfdTempDisp(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,TEMP_DISPLAY_TIME,"Z2: %s",Zone2SrcName);
					
				if(sys_flag.zone2_mute_flag == ON)
					{
					VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_NONE,TEMP_DISPLAY_TIME,"Mute");
					}
				VfdTempDisp(VFD_LINE1_START_POSITION+17,VFD_UPDATE_LINE_ALL,TEMP_DISPLAY_TIME,"%3d",sys_flag.zone2_volume);
				}
			break;
		case MSG_Power:
			if(buffer[1])
				ProcZone2PowerOn(); /*zone2 power on*/
			else
				ProcZone2PowerOff(); /*zone2 power off*/
			break;
		case MSG_InputSource:
			if(IsZone2PowerOn())
				{
				Zone2ProcInputSelect(buffer[1]);
				}
			break;
		case MSG_Mute:
			if(IsZone2PowerOn())
				{
				switch(buffer[1])
					{
					case MODE_TOGGLE:
						if(sys_flag.zone2_mute_flag == ON)
							sys_flag.zone2_mute_flag = OFF;
						else
							sys_flag.zone2_mute_flag = ON;
						break;
					case MODE_FIXED:
						sys_flag.zone2_mute_flag = buffer[2];
						break;
					}
				Zone2Mute(sys_flag.zone2_mute_flag);
				MsgSetVfdShow(ZONE2_ID,0);
				}
			break;
		case Msg_Volume:
			if(IsZone2PowerOn())
				{
				switch(buffer[1])
					{
					case MODE_PLUS:
						if(sys_flag.zone2_volume < ZONE2_MAX_VOLUME)
						{
							sys_flag.zone2_volume += ZONE2_VOLUME_STEP;
						}
						break;
					case MODE_MINUS:
						if(sys_flag.zone2_volume > ZONE2_MIN_VOLUME)
						{
							sys_flag.zone2_volume -= ZONE2_VOLUME_STEP;
						}
						break;
					case MODE_FIXED:
						temp_volume = (buffer[2] << 8) | buffer[3];
						if(IsZone2VolumeValid(temp_volume) == TRUE)
						{
							sys_flag.zone2_volume = temp_volume;
						}
						break;
					}
				SetZone2Volume(sys_flag.zone2_volume);
				MsgSetVfdShow(ZONE2_ID,0);
				}  
			break;
		default:
			return TRUE;
		}
	return TRUE;
}

