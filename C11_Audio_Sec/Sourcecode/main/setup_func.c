#include "macro.h"
#include "setup_menu.h"
#include "setup_func.h"
#include "task.h"
#include "c11_msg_handler.h"
#include "debug.h"
#include "keydefine.h"
#include "key_task.h"
#include "vfd_disp.h"
#include "radio_task.h"
#include "debug.h"

#include "..\driver\eeprom.h"
#include "..\driver\vfddriver.h"
#include "..\driver\channel.h"
#include "..\driver\ioport.h"
#include "..\driver\dac.h"
#include "..\driver\volume.h"
#include "..\driver\hdmi.h"
#include "..\driver\Max335.h"

#include "..\cs495300\dsp_protocol.h"
#include "..\cs495300\dspdrv.h"
#include "..\cs495300\Eq_Manager.h"
#include "..\cs495300\BassManager.h"
#include "..\cs495300\SignalGenerator.h"
#include "..\cs495300\DelayManager.h"
#include "..\cs495300\DSP_API.h"

#include <string.h>

#ifdef _DEBUG
#define DEBUG_SETUP_FUNC
#endif 

extern char strFl[];
extern char strCen[];
extern char strFr[];
extern char strSr[];
extern char strSbr[];
extern char strSbl[];
extern char strSl[];
extern char strSw[];

extern unsigned char system_spk_mode[8];

struct ID_NAME spk_name_fl = {CH_L,SPK_FL_BIT,strFl};
struct ID_NAME spk_name_c = {CH_C,SPK_CEN_BIT,strCen};
struct ID_NAME spk_name_fr = {CH_R,SPK_FR_BIT,strFr};
struct ID_NAME spk_name_sl = {CH_LS,SPK_SL_BIT,strSl};
struct ID_NAME spk_name_sr = {CH_RS,SPK_SR_BIT,strSr};
struct ID_NAME spk_name_sbl = {CH_LB,SPK_SBL_BIT,strSbl};
struct ID_NAME spk_name_sbr = {CH_RB,SPK_SBR_BIT,strSbr};
struct ID_NAME spk_name_sw = {CH_SW,SPK_SW_BIT,strSw};

SPKSTATUS spk_fl = {0,0,0,0,{0},{0},&spk_name_fl};
SPKSTATUS spk_fr = {0,0,0,0,{0},{0},&spk_name_fr};
SPKSTATUS spk_sl = {0,0,0,0,{0},{0},&spk_name_sl};
SPKSTATUS spk_sr = {0,0,0,0,{0},{0},&spk_name_sr};
SPKSTATUS spk_sbl = {0,0,0,0,{0},{0},&spk_name_sbl};
SPKSTATUS spk_sbr = {0,0,0,0,{0},{0},&spk_name_sbr};
SPKSTATUS spk_cen = {0,0,0,0,{0},{0},&spk_name_c};
SPKSTATUS spk_sw = {0,0,0,0,{0},{0},&spk_name_sw};

SPKSTATUS * speaker_table[] = 
{
	&spk_fl,
	&spk_cen,
	&spk_fr,
	&spk_sr,
	&spk_sbr,
	&spk_sbl,
	&spk_sl,
	&spk_sw,
	NULL,
};

extern MENU_ITEM * pMenuCurItem;
extern unsigned char Movie_Music_Speaker_Flag; 

void SetSpeakerMode(void)
{
	unsigned char i=0;
	unsigned char bValue=0;

	for(i=0;i<8;i++)
		{					
		if(Movie_Music_Speaker_Flag==0)  
			bValue = EepromReadByte(SPK_MODE_POSITION + i);
		else
			bValue = EepromReadByte(SPK_MODE_POSITION_MUSIC + i);				

		system_spk_mode[i]=bValue;
		}	
}

void SetDistanceMode(void)
{
	unsigned char i=0;
	unsigned char bValue=0;

	for(i=0;i<8;i++)
		{
		switch(i)
			{
			case CH_L:
				if(Movie_Music_Speaker_Flag==0)
					bValue=dsp_delay_time.spk_fl_dtime;
				else
					bValue=dsp_delay_music_time.spk_fl_dtime;
				DSP_Delay_Channel_Setting(CH_L,bValue);
				break;
			case CH_C:
				if(Movie_Music_Speaker_Flag==0)
					bValue=dsp_delay_time.spk_c_dtime;
				else
					bValue=dsp_delay_music_time.spk_c_dtime;		
				DSP_Delay_Channel_Setting(CH_C,bValue);
				break;
			case CH_R:
				if(Movie_Music_Speaker_Flag==0)
					bValue=dsp_delay_time.spk_fr_dtime;
				else
					bValue=dsp_delay_music_time.spk_fr_dtime;			
				DSP_Delay_Channel_Setting(CH_R,bValue);
				break;
			case CH_LS:
				if(Movie_Music_Speaker_Flag==0)
					bValue=dsp_delay_time.spk_ls_dtime;
				else
					bValue=dsp_delay_music_time.spk_ls_dtime;			
				DSP_Delay_Channel_Setting(CH_LS,bValue);
				break;
			case CH_RS:
				if(Movie_Music_Speaker_Flag==0)
					bValue=dsp_delay_time.spk_rs_dtime;	
				else
					bValue=dsp_delay_music_time.spk_rs_dtime;	
				DSP_Delay_Channel_Setting(CH_RS,bValue);
				break;
			case CH_LB:
				if(Movie_Music_Speaker_Flag==0)
					bValue=dsp_delay_time.spk_sbl_dtime;
				else
					bValue=dsp_delay_music_time.spk_sbl_dtime;	
				DSP_Delay_Channel_Setting(CH_LB,bValue);
				break;
			case CH_RB:
				if(Movie_Music_Speaker_Flag==0)
					bValue=dsp_delay_time.spk_sbr_dtime;
				else
					bValue=dsp_delay_music_time.spk_sbr_dtime;			
				DSP_Delay_Channel_Setting(CH_RB,bValue);
				break;
			case CH_SW:
				if(Movie_Music_Speaker_Flag==0)
					bValue=dsp_delay_time.spk_lfe_dtime;
				else
					bValue=dsp_delay_music_time.spk_lfe_dtime;			
				DSP_Delay_Channel_Setting(CH_SW,bValue);
				break;
			default:
				break;			
				}
		}	
}

void SetBalanceMode(void)
{
	unsigned char i=0;
	unsigned char bValue=0;

	for(i=0;i<8;i++)
		{
			if(Movie_Music_Speaker_Flag==0)
				bValue = EepromReadByte(BALANCE_FL_POSITION + i);
			else
				bValue = EepromReadByte(BALANCE_FL_POSITION_MUSIC + i);

			SetBalanceVolume(i,balance_table[bValue]);
		}
}

void SetCrossoverPointsMode(void)
{
	unsigned char i=0;
	unsigned char bValue=0;

	for(i=0;i<8;i++)
		{
		switch(i)
			{
			case CH_L:
				if(Movie_Music_Speaker_Flag==0)
					bValue = EepromReadByte(CROSS_FL_POSITION + i);
				else
					bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC +i);
				dsp_bass_corner.fl_corner=bValue;
				break;
			case CH_C:
				if(Movie_Music_Speaker_Flag==0)
					bValue = EepromReadByte(CROSS_FL_POSITION + i);
				else
					bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC +i);	
				dsp_bass_corner.cen_corner=bValue;
				break;
			case CH_R:
				if(Movie_Music_Speaker_Flag==0)
					bValue = EepromReadByte(CROSS_FL_POSITION + i);
				else
					bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC +i);
				dsp_bass_corner.fr_corner=bValue;
				break;
			case CH_LS:
				if(Movie_Music_Speaker_Flag==0)
					bValue = EepromReadByte(CROSS_FL_POSITION + i);
				else
					bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC +i);
				dsp_bass_corner.sl_corner=bValue;
				break;
			case CH_RS:
				if(Movie_Music_Speaker_Flag==0)
					bValue = EepromReadByte(CROSS_FL_POSITION + i);
				else
					bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC +i);	
				dsp_bass_corner.sr_corner=bValue;
				break;
			case CH_LB:
				if(Movie_Music_Speaker_Flag==0)
					bValue = EepromReadByte(CROSS_FL_POSITION + i);
				else
					bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC +i);
				dsp_bass_corner.sbl_corner=bValue; 
				break;
			case CH_RB:
				if(Movie_Music_Speaker_Flag==0)
					bValue = EepromReadByte(CROSS_FL_POSITION + i);
				else
					bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC +i);
				dsp_bass_corner.sbr_corner=bValue;
				break;
			case CH_SW:
				if(Movie_Music_Speaker_Flag==0)
					bValue = EepromReadByte(CROSS_FL_POSITION + i);
				else
					bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC +i);
				
				dsp_bass_corner.sum1_corner = bValue;
				dsp_bass_corner.sum2_corner = bValue;
				dsp_bass_corner.sum3_corner = bValue;
				dsp_bass_corner.sum4_corner = bValue;
				dsp_bass_corner.sum5_corner = bValue;				
				break;
			default:
				break;
			}
 	}	
	SetBassManagerSpeaker();
}

void SetSpeakerPhaseMode(void)
{
	unsigned char i=0;
	unsigned char bValue=0;

	for(i=0;i<8;i++)
		{
		if(Movie_Music_Speaker_Flag==0)
			bValue = EepromReadByte(SPK_PHASE_POSITION+i);
		else
			bValue = EepromReadByte(SPK_PHASE_POSITION_MUSIC+i);	
		SetSpkPhase(i,bValue);
		}
}

void SetPanoramaMode(void)
{
/*
	unsigned char i=0;
	unsigned char bValue=0;

	//当为dolby 2 channel 流且为Prologic2 Surround 时起效果
	if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))
	{
		bValue = EepromReadByte(SPK_PANORAMA_DIMENSION_MUSIC);
		Set_Prologic2Dimension(bValue);

		bValue = EepromReadByte(SPK_PANORAMA_PANORAMA_MUSIC);
		Set_Prologic2Panorama_On_Or_Off(bValue);

		bValue = EepromReadByte(SPK_PANORAMA_WIDTH_MUSIC);
		Set_Prologic2Width(bValue);
	
		for(i=0;i<8;i++)
		{
			bValue = EepromReadByte(SPK_PANORAMA_DELAY_MUSIC + i);
			SetSpkDistance(i,bValue);
		}
	}
*/	
}

void SetPanoramaToDefault(void)
{
/*
	if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))
		{
		//Dspabprologic2control(0x01);
		DspabPrologic2ControlRegister(0x02); //Panorama disable
		DspabPrologic2DecodeMode(0x03); //default Movie mode
		Set_Prologic2Dimension(0x04); //Dimension default value
		Set_Prologic2Width(0x00); //width default value
		}
*/	
}

extern unsigned char PreDspMode;
void ChangeListenProfileMode(void)
{
#ifdef DEBUG_SETUP_FUNC
	debug_printf("\nChangeListenProfileMode");
#endif

	DacMute(1); 
	SetSpeakerMode();
	SetDistanceMode();
	SetBalanceMode();
	SetCrossoverPointsMode();
	if(PreDspMode==3)/*Analog 2 channel +NEO6/PL2X时没办法加载APP,故做此条件判断*/
		{
		if(sys_flag.decode_mode.pcm_decode_mode==PCM_MODE_NORMAL)
			{
			DSP_EQ_Firmware_Init();
			}
		}
	else
		{
		DSP_EQ_Firmware_Init();  
		}
	SetSpeakerPhaseMode();
	//if(Movie_Music_Speaker_Flag==1)
	//	SetPanoramaMode();
	//else
	//	SetPanoramaToDefault();	
	DacMute(0);
}

SETUP_T AssignedInputSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			if(sys_flag.system_status == POWER_ON) 
				{
				if(sys_flag.zone1_channel == resValue)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				}
			break;
			
		case READ_VALUE_MODE:
			bValue = EepromReadByte(ASSIGNED_INPUT1 + resValue);
			return bValue;
			
		case WRITE_VALUE_MODE:
			EepromWriteByte(ASSIGNED_INPUT1 + resValue, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			if(resValue==INPUT_1) //Change by cjm 2009-03-31
				return 1;
			else
				return 0;
		}
	return 0;
}

SETUP_T SpkDimensionSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			Set_Prologic2Dimension(bValue); 
			break;
			
		case READ_VALUE_MODE:
			bValue = EepromReadByte(SPK_PANORAMA_DIMENSION_MUSIC + resValue);
			return bValue;
			
		case WRITE_VALUE_MODE:
			EepromWriteByte(SPK_PANORAMA_DIMENSION_MUSIC + resValue, bValue);
			break;
			
		case LOAD_DEFAULT_MODE:	
			return 3;
		}
	return 0;
}

SETUP_T SpkPanoramaSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			Set_Prologic2Panorama_On_Or_Off(bValue); 
			break;
			
		case READ_VALUE_MODE:
			bValue = EepromReadByte(SPK_PANORAMA_PANORAMA_MUSIC + resValue);
			return bValue;
			
		case WRITE_VALUE_MODE:
			EepromWriteByte(SPK_PANORAMA_PANORAMA_MUSIC + resValue, bValue);
			break;
			
		case LOAD_DEFAULT_MODE:	
			return 0;
		}
	return 0;
}

SETUP_T SpkWidthSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			Set_Prologic2Width(bValue);
			break;
			
		case READ_VALUE_MODE:
			bValue = EepromReadByte(SPK_PANORAMA_WIDTH_MUSIC + resValue);
			return bValue;
			
		case WRITE_VALUE_MODE:
			EepromWriteByte(SPK_PANORAMA_WIDTH_MUSIC + resValue, bValue);
			break;
			
		case LOAD_DEFAULT_MODE:
			return 3;
		}
	return 0;
}

SETUP_T SpkDelaySetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{	
	switch(bMode)
		{
		case EXECUTE_MODE:
			if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))
				SetSpkDistance(resValue,bValue);
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(SPK_PANORAMA_DELAY_MUSIC + resValue);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(SPK_PANORAMA_DELAY_MUSIC + resValue, bValue);
			break;

		case LOAD_DEFAULT_MODE:		
			return 0;
		}
	return 0;
}

SETUP_T SpkPhaseSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			Set7CH1Mute(Mute);
			SetSpkPhase(resValue,bValue);
			Set7CH1Mute(UnMute); 
			break;

		case READ_VALUE_MODE:			
			if(Movie_Music_Speaker_Flag==0)
				bValue = EepromReadByte(SPK_PHASE_POSITION+resValue);
			else
				bValue = EepromReadByte(SPK_PHASE_POSITION_MUSIC+resValue); 

			if(bValue)
				return 1;
			return 0;

		case WRITE_VALUE_MODE:		
			if(Movie_Music_Speaker_Flag==0)
				EepromWriteByte(SPK_PHASE_POSITION+resValue, bValue);
			else
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+resValue, bValue);  
			break;
  
		case LOAD_DEFAULT_MODE:
			return 0;
		}
	return 0;
}

SETUP_T SpkEqSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	unsigned char channel,band;

	channel = resValue & 0x0F;
	band = (resValue & 0xF0) >> 4; 
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d:%d,%f\n","SpkEqSetup",channel,band,balance_table[bValue]);
#endif
			if(Movie_Music_Speaker_Flag==0)
				dsp_eq_table[channel][band] = balance_table[bValue];
			else
				dsp_eq_music_table[channel][band] = balance_table[bValue];
			EqModuleconfig(channel,band); /*设置EQ 各寄存器值*/
			break;

		case READ_VALUE_MODE:		
			if(Movie_Music_Speaker_Flag==0)
				bValue = EepromReadByte(EQ_FL_POSITION + (channel * EQ_POSITION_LENGTH) + band);
			else
				bValue = EepromReadByte(EQ_FL_POSITION_MUSIC+ (channel * EQ_POSITION_LENGTH) + band); //add cjm 2007-7-17
			return bValue;

		case WRITE_VALUE_MODE:
			if(Movie_Music_Speaker_Flag==0)
				EepromWriteByte(EQ_FL_POSITION + (channel * EQ_POSITION_LENGTH) + band, bValue);
			else
				EepromWriteByte(EQ_FL_POSITION_MUSIC+ (channel * EQ_POSITION_LENGTH) + band, bValue); //add cjm 2007-7-17		
			break;

		case LOAD_DEFAULT_MODE:	
			return 30;
		}
	return 0;
}

SETUP_T SpkCrossoverSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","SpkCrossoverSetup",bValue);
#endif
			SetDspCorner(resValue,bValue);
			break;

		case READ_VALUE_MODE:  
			if(Movie_Music_Speaker_Flag==0)
				bValue = EepromReadByte(CROSS_FL_POSITION + resValue);
			else
				bValue = EepromReadByte(CROSS_FL_POSITION_MUSIC + resValue);
			return bValue;

		case WRITE_VALUE_MODE:
			if(Movie_Music_Speaker_Flag==0)
				EepromWriteByte(CROSS_FL_POSITION + resValue, bValue);
			else
				EepromWriteByte(CROSS_FL_POSITION_MUSIC+ resValue, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return 0;
		}
	return 0;
}

void SetSpkDistance(unsigned char channel,unsigned char distance)
{
	unsigned char i;
	unsigned char distance_buf[8];

	switch(channel)
		{
		case CH_L:	/*各个通道分别延时*/
		case CH_R:
		case CH_C:
		case CH_LS:
		case CH_RS:
		case CH_LB:
		case CH_RB:
		case CH_SW:
			distance_buf[channel] = distance;
			break;
		default:
			break;
		} 

	//for(i=CH_L;i<=CH_RB;i++) //Dele by cjm 2008-10-04
		//{
		//SetDspDelay(i,distance_buf[i]);
		//}
	SetDspDelay(i,distance_buf[i]);
}

void Phase_Relay(unsigned char channel,unsigned char Phase)
{
	switch(channel)
		{
		case CH_L: //0
		case CH_R: // 2	
			if(Phase==1)
				SetCH1RELAY(1);
			else
				SetCH1RELAY(0);
			break;
		case CH_LS: // 3
		case CH_RS: // 4
			if(Phase==1)
				SetCH2RELAY(1);
			else
				SetCH2RELAY(0);
			break;
		case CH_C: // 1
		case CH_SW:	// 5
			if(Phase==1)
				SetCH3RELAY(1);
			else
				SetCH3RELAY(0);
			break;
		case CH_LB: // 6
		case CH_RB: // 7
			if(Phase==1)
				SetCH4RELAY(1);
			else
				SetCH4RELAY(0);
			break;
		default:
			break;
		}
}

void SetSpkPhase(unsigned char channel,unsigned char Phase)
{
	unsigned char value;
	
	//debug_printf("\nchannel=%d",channel);
	//debug_printf("\nPhase=%d",Phase);
	if(sys_flag.system_status == POWER_ON)
		{
		switch(channel)
			{
			case CH_L:  //0
			case CH_LS: // 3
			case CH_C: // 1
			case CH_LB: // 6
				DacInvert(Phase,channel);   
				if(Phase)
					Phase_Relay(channel,1);   
				else
					Phase_Relay(channel,0);   
				break;
				
			case CH_R: // 2
				value = EepromReadByte(SPK_PHASE_POSITION+CH_L);
				if(value) //CH_L 反向
					{
					if(Phase==1)
						Phase_Relay(channel,0); 
					else
						Phase_Relay(channel,1); 						
					}
				else //CH_L 正向
					{  
					if(Phase==1)
						Phase_Relay(channel,1); 
					else
						Phase_Relay(channel,0); 	
					}
				break;
			case CH_RS:// 4
				value = EepromReadByte(SPK_PHASE_POSITION+CH_LS);
				if(value) //CH_L 反向
					{
					if(Phase==1)
						Phase_Relay(channel,0); 
					else
						Phase_Relay(channel,1); 						
					}
				else //CH_L 正向
					{
					if(Phase==1)
						Phase_Relay(channel,1); 
					else
						Phase_Relay(channel,0); 						
					}				
				break;
			case CH_SW: // 5
				value = EepromReadByte(SPK_PHASE_POSITION+CH_C);
				if(value) //CH_L 反向
					{
					if(Phase==1)
						Phase_Relay(channel,0); 
					else
						Phase_Relay(channel,1); 							
					}
				else //CH_L 正向
					{
					if(Phase==1)
						Phase_Relay(channel,1); 
					else
						Phase_Relay(channel,0); 						
					}				
				break;
			case CH_RB: // 7
				value = EepromReadByte(SPK_PHASE_POSITION+CH_LB);
				if(value) //CH_L 反向
					{
					if(Phase==1)
						Phase_Relay(channel,0); 
					else
						Phase_Relay(channel,1); 							
					}
				else //CH_L 正向
					{
					if(Phase==1)
						Phase_Relay(channel,1); 
					else
						Phase_Relay(channel,0); 						
					}
				break;
			}			
		}
}

SETUP_T AvSyncDelaySetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("\n%s->%d\n","AvSyncDelaySetup",bValue);
#endif
 			DSP_AV_Delay(bValue);
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(AV_SYNC_POSITION);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(AV_SYNC_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:
			return DEFAULT_AVSYNC_MODE;
		}
	return 0;
}  

uchar balance_mode = 0;
SETUP_T SpkBlanceTestMode(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	uchar i;
	switch(bMode)
		{
		case EXECUTE_MODE:
			if(resValue == 1) /*强制关掉粉红噪声测试*/
				{
				if(balance_mode) /*Manual mode*/
					{
					balance_mode = 0;
					SetDspSignal(Signal_Stop,0,0);
#ifdef DEBUG_SETUP_FUNC
					//debug_printf("Exit Balance test\n");
					//debug_printf("\nsys_flag.zone1_sub_channel=%d",sys_flag.zone1_sub_channel);
#endif
					}
				
				if(Movie_Music_Speaker_Flag==0)
					EepromWriteByte(BALANCE_TEST_MODE_POSITION, 0); /*OFF Mode*/
				else
					EepromWriteByte(BALANCE_TEST_MODE_POSITION_MUSIC, 0); 
				
				break;
				}
			switch(bValue)
				{
				case 0:	/*OFF Mode*/
					balance_mode = 0;
#ifdef DEBUG_SETUP_FUNC
					//debug_printf("Balance Off\n");
#endif
					break;
				case 1:	/*MANUAL Mode*/
#ifdef DEBUG_SETUP_FUNC
					//debug_printf("Balance Manual\n");
#endif					
					//Set7Ch1MainVolume(-20);
					SoftMute(0);
					balance_mode = 1;
					break;
				case 2:	/*AUTO mode*/  
					balance_mode = 1;
#ifdef DEBUG_SETUP_FUNC
					//debug_printf("Balance Auto\n");
#endif
					key_enable = 0; /*按键失能*/
					//Set7Ch1MainVolume(-20); 
					SoftMute(0); 
					Change_DSP_Decoder_Code(1,15);/*加载SGEN Code*/  
					DelayMs(50); /*加载完固件后延时下等DSP 稳定*/
					
					for(i = 0;speaker_table[i];i++)
						{
						SetDspSignal(Signal_White_Noise,speaker_table[i]->channel->spk_index,0);
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_NONE,"Test Mode       AUTO");
						VfdDisplay(VFD_LINE2_START_POSITION + 10,VFD_UPDATE_LINE2,speaker_table[i]->channel->name);
						DelayMs(5000);
						}		
					
					if(Movie_Music_Speaker_Flag==0)
						EepromWriteByte(BALANCE_TEST_MODE_POSITION, 0); /*Balance Auto 测试结束设置为OFF Mode*/
					else
						EepromWriteByte(BALANCE_TEST_MODE_POSITION_MUSIC, 0); 

					DelayMs(50);
					SetDspSignal(Signal_Stop,0,0); /*粉红噪声失能*/
					key_enable = 1; /*按键使能*/
					balance_mode = 0;			
					break;
				}
			break;

		case READ_VALUE_MODE:
			if(Movie_Music_Speaker_Flag==0)
				bValue = EepromReadByte(BALANCE_TEST_MODE_POSITION);
			else
				bValue = EepromReadByte(BALANCE_TEST_MODE_POSITION_MUSIC); 
			return bValue;

		case WRITE_VALUE_MODE:
			if(Movie_Music_Speaker_Flag==0)
				EepromWriteByte(BALANCE_TEST_MODE_POSITION, bValue);
			else
				EepromWriteByte(BALANCE_TEST_MODE_POSITION_MUSIC, bValue); 
			break;

		case LOAD_DEFAULT_MODE:
			return 0;
		}
	return 0;
}

void SetBalanceTestSignal(uchar channel,uchar on)
{
#ifdef DEBUG_SETUP_FUNC
	//debug_printf("SetBalanceTestSignal(%d,%d),%d\n",channel,on,balance_mode);
#endif

	if(balance_mode == 1)
		{
		if(on)
			{
			SetDspSignal(Signal_White_Noise,channel,0);
			}
		else
			{
			SetDspSignal(Signal_Stop,0,0);
			}
		}
}

SETUP_T SpkBlanceSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SetBalanceVolume(resValue,balance_table[bValue]);
			break;

		case READ_VALUE_MODE:
			if(Movie_Music_Speaker_Flag==0)
				bValue = EepromReadByte(BALANCE_FL_POSITION + resValue);
			else
				bValue = EepromReadByte(BALANCE_FL_POSITION_MUSIC + resValue); 
			return bValue;

		case WRITE_VALUE_MODE:
			if(Movie_Music_Speaker_Flag==0)
				EepromWriteByte(BALANCE_FL_POSITION + resValue, bValue);
			else
				EepromWriteByte(BALANCE_FL_POSITION_MUSIC+ resValue, bValue); 
			break;

		case LOAD_DEFAULT_MODE:
			return 30;
		}
	return 0;
}

extern unsigned char DolbyType;
SETUP_T LateSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","LateSetup",bValue);
#endif
			SetLate(DolbyType,bValue);
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(LATE_POSITION);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(LATE_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:
			return DEFAULT_LATE_MODE;
		}
	return 0;
}

uchar pwd_mode = 0;
SETUP_T PasswordModeSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","PasswordSetup",bValue);
#endif
			if(bValue)
				pwd_mode = 1;
			else
				pwd_mode = 0;
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(PASSWORD_MODE_POSITION);
			if(bValue)
				pwd_mode = 1;
			else
				pwd_mode = 0;
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(PASSWORD_MODE_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:
			return DEFAULT_PASSWORD_MODE;
		}
	return 0;
}

SETUP_T PasswordSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","PasswordSetup",bValue);
#endif
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(PASSWORD_MODE_POSITION);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(PASSWORD_MODE_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:
			return DEFAULT_PASSWORD_MODE;
		}
	return 0;
}

SETUP_T BrightSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","BrightSetup",bValue);
#endif
			MsgSetDimer(bValue,MODE_FIXED);
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(DIMER_POSITION);
//			if(bValue > MAX_DIMER)
//				bValue = MAX_DIMER;
			sys_flag.system_dimer = bValue;
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(DIMER_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:
			return DEFAULT_BRIGHT_MODE;
		}
	return 0;
}

SETUP_T AutoInputSeekSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","AutoInputSeekSetup",bValue);
#endif
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(AUTO_DETECT_POSITION);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(AUTO_DETECT_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return DEFAULT_INPUT_SEEK_MODE;
		}
	return 0;
}

extern const unsigned int zone2_analog_in_table[];
SETUP_T Zone2InputSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	unsigned char value;
	
	switch(bMode)
		{
		case EXECUTE_MODE:			
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","Zone2InputSetup",bValue);
#endif
			if(IsZone2PowerOn())
				{
				if((bValue & 0xF) == INPUT_TUNER)
					{
					switch(bValue & 0xF0)
						{
						case 0x10: //AM
							if(!IsCurFocusTid(RADIO_ID))
								Zone2RadioMode(_AM_MODE);
							break;
						case 0x20: //FM
							if(!IsCurFocusTid(RADIO_ID))
								Zone2RadioMode(_FM_MODE);
							break;
						case 0x40: //HDFM
							if(!IsCurFocusTid(RADIO_ID))
								Zone2RadioMode(_HD_FM_MODE);
							break;
						case 0x80: //HDAM
							if(!IsCurFocusTid(RADIO_ID))
								Zone2RadioMode(_HD_AM_MODE);
							break;
						default:
							break;
						}
					sys_flag.zone2_channel = bValue & 0xF;  
					Zone2AnalogInOff(); /*关闭所有ZONE2 模拟输出*/
					SetMax335Switch(zone2_analog_in_table[9],1); /*打开HD Radio analog 开关*/
					UpdateMax335();	/*Update MAX335*/
					DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/				
					}
				else /*Other input*/
					{
					sys_flag.zone2_channel = bValue & 0xF;
					switch(sys_flag.zone2_channel )
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
							DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000000); /*Downmix 使能,从DA02_Channel 1/2 输出*/					
							Zone2SelectChannel(sys_flag.zone2_channel);	
							break;
						case INPUT_HDMI1:
							if((sys_flag.zone1_channel>=INPUT_HDMI1)&&(sys_flag.zone1_channel<=INPUT_HDMI4))
								{
								value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
								if(value==5) /*当为6表示Zone1的HDMI 流已经稳定*/
									{
									DigiSendByte(ADDR_ZONE2,0X00);/*通道切到HDMI口*/
									DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix 使能,从DA02_Channel 1/2 输出*/					
									}
								}
							break;
						case INPUT_HDMI2:
							if(sys_flag.zone1_channel==ARC_CHANNEL)
								{
								value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
								if(value==5) /*当为6表示Zone1的HDMI 流已经稳定*/
									{
									DigiSendByte(ADDR_ZONE2,0x00);/*通道切到HDMI口*/
									DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix 使能,从DA02_Channel 1/2 输出*/														
									}
								}
							break;
						default:
							break;
						}							
					}			
				}			
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(ZONE2_OUT_POSITION);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(ZONE2_OUT_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return DEFAULT_ZONE2_INPUT;
		}
	return 0;
}

uchar GetVolumeIndex(short volume)
{
	uchar i = 0;
	for(i=0;i<MAX_VOLUME;i++)
		{
		if(volume == volume_table[i])
			return i;
		}
	return 60;
}

SETUP_T Zone2VolumeSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:			
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","Zone2VolumeSetup",bValue);
#endif
			sys_flag.zone2_volume = volume_table[bValue];
			SetZone2Volume(sys_flag.zone2_volume);	
			break;

		case READ_VALUE_MODE:		
			bValue = GetVolumeIndex(sys_flag.zone2_volume);
			bValue = EepromReadByte(ZONE2_VOLUME_POSITION);
			sys_flag.zone2_volume = volume_table[bValue];
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(ZONE2_VOLUME_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return DEFAULT_ZONE2_VOLUME;
		}
	return 0;
}

SETUP_T IrControlSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","IrControlSetup",bValue);
#endif
			Zone1SelectIr(bValue);
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(IR_CONTROL_POSITION);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(IR_CONTROL_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return DEFAULT_IR_MODE;
		}
	return 0;
}

SETUP_T Zone2IrControlSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","IrControlSetup",bValue);
#endif
			Zone2SelectIr(bValue);
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(ZONE2_IR_CONTROL_POSITION);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(ZONE2_IR_CONTROL_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:
			return DEFAULT_ZONE2_IR_MODE;
		}
	return 0;
}

unsigned char zone2_ir_mode = 0;
SETUP_T Zone2IrModeSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","Zone2IrModeSetup",bValue);
#endif
			if(bValue)
				bValue = 1;
			else
				bValue = 0;
			zone2_ir_mode = bValue;
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(ZONE2_IR_MODE_POSITION);
			if(bValue)
				bValue = 1;
			else
				bValue = 0;
			zone2_ir_mode = bValue;
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(ZONE2_IR_MODE_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:		
			return 0;
		}
	return 0;
}

unsigned char rear_ir_mode = 0;
SETUP_T RearIrModeSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
#ifdef DEBUG_SETUP_FUNC
			//debug_printf("%s->%d\n","RearIrModeSetup",bValue);
#endif
			if(bValue)
				bValue = 1;
			else
				bValue = 0;
			rear_ir_mode = bValue;
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(REAR_IR_MODE_POSITION);
			if(bValue)
				bValue = 1;
			else
				bValue = 0;
			rear_ir_mode = bValue;
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(REAR_IR_MODE_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:
			return 0;
		}
	return 0;
}

uchar SystemTriggerMode = 0;
uchar SystemTriggerMode1 = 0;
uchar SystemTriggerMode2 = 0;
SETUP_T TriggerSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SystemTriggerMode = bValue;
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(TRIGGER_POSITION);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(TRIGGER_POSITION, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return DEFAULT_TRIGGER_MODE;
		}
	return 0;
}

SETUP_T TriggerSetup1(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SystemTriggerMode1 = bValue;
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(TRIGGER_POSITION1);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(TRIGGER_POSITION1, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return DEFAULT_TRIGGER_MODE;
		}
	return 0;
}

SETUP_T TriggerSetup2(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SystemTriggerMode2 = bValue;
			break;

		case READ_VALUE_MODE:
			bValue = EepromReadByte(TRIGGER_POSITION2);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(TRIGGER_POSITION2, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return DEFAULT_TRIGGER_MODE;
		}
	return 0;
}
SETUP_T RestoreDefaultSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			key_enable = 0;
			DacMute(1);
			VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"....................");
			SetupLoadDefault(1);
			VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"Restore Default OK  ");
			DelayMs(500);
			DacMute(0);
			key_enable = 1;
			return 1;

		case READ_VALUE_MODE:
			return bValue;

		case WRITE_VALUE_MODE:
			break;

		case LOAD_DEFAULT_MODE:	
			return 0;
		}
	return 0;
}

/* 
 * @brief set speaker config 功能执行函数
 *
 * @author chenjianming
 * @date 2007-12-5
 * @refer chenshiguang C11 code
 */
SETUP_T SpkConfigSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SetDspSpkMode(resValue,bValue); /*bValue 为设置的值、resValue为子菜单当前选项数*/			
			break;
		case READ_VALUE_MODE:
			if(Movie_Music_Speaker_Flag==0)  
				bValue = EepromReadByte(SPK_MODE_POSITION + resValue);
			else
				bValue = EepromReadByte(SPK_MODE_POSITION_MUSIC + resValue); 		
			if(bValue)
				return 1;
			return 0;
		case WRITE_VALUE_MODE:
			if(bValue)
				bValue = 1;
			if(Movie_Music_Speaker_Flag==0)  			
				EepromWriteByte(SPK_MODE_POSITION + resValue, bValue);
			else
				EepromWriteByte(SPK_MODE_POSITION_MUSIC + resValue, bValue);	
			break;
		case LOAD_DEFAULT_MODE:
			return 0x01;
		}
	return 0;
}

SETUP_T SpkDistanceSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SetSpkDistance(resValue,bValue);
			break;

		case READ_VALUE_MODE:			
			if(Movie_Music_Speaker_Flag==0)
				bValue = EepromReadByte(DISTANCE_FL_POSITION + resValue);
			else
				bValue = EepromReadByte(DISTANCE_FL_POSITION_MUSIC+ resValue); 
			return bValue;

		case WRITE_VALUE_MODE:			
			if(Movie_Music_Speaker_Flag==0)  
				EepromWriteByte(DISTANCE_FL_POSITION + resValue, bValue);
			else
				EepromWriteByte(DISTANCE_FL_POSITION_MUSIC+ resValue, bValue); 		
			break;

		case LOAD_DEFAULT_MODE:
			return 0;
		}
	return 0;
}

uchar AnalogInLevel[INPUT_TUNER + 1] = {0};
void SetAnalogInLevel(uchar bMode,uchar channel,uchar level)
{
	if(channel <= INPUT_TUNER)
		{
		AnalogInLevel[channel] = level;
		if(sys_flag.zone1_channel == channel)
			{
			if(bMode == EXECUTE_MODE)
				Set2ChVolume(AnalogInLevelTable[level]);  
			}
		}
}

SETUP_T AnalogLevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SetAnalogInLevel(bMode,resValue,bValue);
			break;
			
		case READ_VALUE_MODE:
			bValue = EepromReadByte(ANALOG1_LEVEL_POSITION + resValue);
			SetAnalogInLevel(bMode,resValue,bValue);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(ANALOG1_LEVEL_POSITION+resValue, bValue);
			break;

		case LOAD_DEFAULT_MODE:	
			return DEFAULT_INPUT1_LEVEL;
		}
	return 0;
}

uchar AnalogInMode[INPUT_TUNER + 1] = {0};
void SetAnalogInMode(uchar bMode,uchar channel,uchar mode)
{
	if(channel <= INPUT_TUNER)
		{
		AnalogInMode[channel] = mode;
		}
}

SETUP_T AnalogInSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SetAnalogInMode(bMode,resValue,bValue);
			if(sys_flag.zone1_channel == resValue)
				MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
			break;
		case READ_VALUE_MODE:
			bValue = EepromReadByte(ANALOG1_MODE_POSITION + resValue);
			SetAnalogInMode(bMode,resValue,bValue);
			return bValue;

		case WRITE_VALUE_MODE:
			EepromWriteByte(ANALOG1_MODE_POSITION + resValue, bValue);
			break;

		//change by cjm 009-03-31
		case LOAD_DEFAULT_MODE:
			switch(resValue)
				{
				case INPUT_1:
					return DEFAULT_ANALOG1_MODE;
				case INPUT_2:
				case INPUT_3:
				case INPUT_4:
				case INPUT_5:
				case INPUT_6:
				case INPUT_7:
				case INPUT_8:
					return DEFAULT_ANALOG2_MODE;
				case INPUT_CH71:
					return DEFAULT_ANALOG7CH1_MODE;
				case INPUT_TUNER:
					return DEFAULT_ANALOG_TUNER_MODE;
				}
		//change by cjm 009-03-31
		}
	return 0;
}

SETUP_T HDMIInAudioSetup(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			break;
		case READ_VALUE_MODE:
			bValue = EepromReadByte(HDMI1_IN_AUDIO + resValue);  //bValue 为设置的值、resValue为子菜单当前选项数
			break;
		case WRITE_VALUE_MODE:
			EepromWriteByte(HDMI1_IN_AUDIO + resValue, bValue);			
			break;
		case LOAD_DEFAULT_MODE:	  
			return 0;
		}	
}


void SetHdmiAudioOut(uchar mode)
{
	if(sys_flag.system_status == POWER_ON) 
		{
		if(mode==1)
			HdmiSendByte(ADDR_HDMI_AUDIO_OUT,1);  
		else
			HdmiSendByte(ADDR_HDMI_AUDIO_OUT,0);
		}
}

unsigned char CD_Pure_Audio=0;
SETUP_T CDPureAudio(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:  
			CD_Pure_Audio=bValue;
			break;
		case READ_VALUE_MODE:
			bValue = EepromReadByte(CD_PURE_AUDIO + resValue);  
			break;
		case WRITE_VALUE_MODE:
			EepromWriteByte(CD_PURE_AUDIO + resValue, bValue);	
			break;
		case LOAD_DEFAULT_MODE:
			return 0;
		}
}

unsigned char HDMI_Audio_Out=0;
SETUP_T HDMIAUIDOOUT(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:  
			HDMI_Audio_Out=bValue;
			SetHdmiAudioOut(bValue);
			break;
		case READ_VALUE_MODE:
			bValue = EepromReadByte(HDMI_AUDIO_OUT + resValue);  
			break;
		case WRITE_VALUE_MODE:
			EepromWriteByte(HDMI_AUDIO_OUT + resValue, bValue);	
			break;
		case LOAD_DEFAULT_MODE:
			return 0;
		}
}

void SetMasterVolumeLevel(unsigned char volume)
{
	short max_vol_Eeprom;
	short max_vol_value;
	short temp_volume;
	
	if(sys_flag.system_status == POWER_ON)
	{
		max_vol_Eeprom=EepromReadByte(Set_Max_Volume_Level); 
		max_vol_value=volume_table[max_vol_Eeprom];
		temp_volume=volume_table[volume];
		if(temp_volume<=max_vol_value)
		{
			SetMasterVolumeValue(volume);
			Set7Ch1MainVolume(sys_flag.zone1_volume);
		}
	}
}

SETUP_T MasterVolumeLevel(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue)
{
	short max_vol_Eeprom;
	short max_vol_value;
	short temp_volume;
	
	switch(bMode)
		{
		case EXECUTE_MODE:
			SetMasterVolumeLevel(bValue);
			break;
		case READ_VALUE_MODE:
			bValue = EepromReadByte(Master_Volume_Level);  
			return bValue;
		case WRITE_VALUE_MODE:
			max_vol_Eeprom=EepromReadByte(Set_Max_Volume_Level); 
			max_vol_value=volume_table[max_vol_Eeprom];
			temp_volume=volume_table[bValue];
			if(temp_volume<=max_vol_value)
			{
				EepromWriteByte(Master_Volume_Level,bValue);
			}
			break;
		case LOAD_DEFAULT_MODE:
			return 60; //default -30DB
		}
}

void SetZ2MasterVolumeLevel(unsigned char volume)
{
	if(IsZone2PowerOn())
	{
		SetZ2MasterVolumeValue(volume);
		SetZone2Volume(sys_flag.zone2_volume);
	}
}

SETUP_T Z2MasterVolumeLevel(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue)
{
	switch(bMode)
		{
		case EXECUTE_MODE:
			SetZ2MasterVolumeLevel(bValue); 
			break;
		case READ_VALUE_MODE:
			bValue = EepromReadByte(Z2_Master_Volume_Level);  
			return bValue;
		case WRITE_VALUE_MODE:
			EepromWriteByte(Z2_Master_Volume_Level,bValue);   
			break;
		case LOAD_DEFAULT_MODE:
			return 60; //default -30DB
		}
}

void MaxVolumeLevel(unsigned char volume)
{
	short temp_volume_Eeprom; 
	short temp_volume_value; 
	short max_vol_value;
	int i=0;
	
	if(sys_flag.system_status == POWER_ON)
	{
		max_vol_value=volume_table[volume];
		//debug_printf("\r\nmax_vol_value=%d",max_vol_value);
		for(i=0;i<15;i++) //Input1-8/7.1/Tuner/HDMI1/HDMI2 
		{
			temp_volume_Eeprom=EepromReadByte(ZONE1_VOLUME_POSITION + i); 
			temp_volume_value=volume_table[temp_volume_Eeprom];
			//debug_printf("\r\nmax_vol_value=%d",temp_volume_value);
			if(temp_volume_value>max_vol_value) //比最大输出音量大
			{
				//debug_printf("\r\nsys_flag.zone1_channel=%d",sys_flag.zone1_channel);
				EepromWriteByte(ZONE1_VOLUME_POSITION+i,volume);
				if(i==sys_flag.zone1_channel) //且是当前通道.立马起作用
				{	
					sys_flag.zone1_volume=volume_table[volume];
					Set7Ch1MainVolume(sys_flag.zone1_volume);	
				}
			}
		}

		for(i=1;i<5;i++) //AM/FM/HDAM/HDFM
		{
			temp_volume_Eeprom=EepromReadByte(AM_VOLUME_POSITION + i-1); 
			temp_volume_value=volume_table[temp_volume_Eeprom];
			//debug_printf("\r\nmax_vol_value=%d",temp_volume_value);
			if(temp_volume_value>max_vol_value) //比最大输出音量大
			{
				EepromWriteByte(AM_VOLUME_POSITION+i-1,volume);
				if(sys_flag.zone1_channel==INPUT_TUNER)
				{
					if(i==current_radio_mode)
					{
						sys_flag.zone1_volume=volume_table[volume];
						Set7Ch1MainVolume(sys_flag.zone1_volume);
					}
				}
			}	
		}

		temp_volume_Eeprom=EepromReadByte(Master_Volume_Level); 
		temp_volume_value=volume_table[temp_volume_Eeprom];
		if(temp_volume_value>max_vol_value) //比最大输出音量大
		{
			EepromWriteByte(Master_Volume_Level,volume);
		}
	}
}

SETUP_T SetMaxVolumeLevel(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue)
{
	switch(bMode)
	{
		case EXECUTE_MODE:
			MaxVolumeLevel(bValue);
			break;
		case READ_VALUE_MODE:
			bValue = EepromReadByte(Set_Max_Volume_Level);  
			return bValue;
		case WRITE_VALUE_MODE:
			EepromWriteByte(Set_Max_Volume_Level,bValue);   
			break;
		case LOAD_DEFAULT_MODE:
			return 90; //default +15DB
	}
}
//
void ReadName(unsigned char addr,unsigned char len,char *src)
{
	uchar i;
	for(i=0;i<len;i++)
		{
		src[i] = EepromReadByte(addr+i);
		}
	src[i] = 0;
}

void WriteName(unsigned char addr,unsigned char len,char *src)
{
	unsigned char i;
#ifdef DEBUG_SETUP_FUNC
	//debug_printf("\nWrite(%s):",src);
#endif

	for(i=0;i<len;i++)
		{
		if((src[i] >= 0x20) && (src[i] <= 0x7F))
			{	
#ifdef DEBUG_SETUP_FUNC			
			//debug_printf("\naddr=%d",addr+i);
			//debug_printf("    data=%02X ",src[i]);
#endif
			EepromWriteByte(addr+i, src[i]);
			}
		else
			break;
		}	
	for(;i<len;i++) /*若超出此范围的,写OX20*/
		{
		EepromWriteByte(addr+i, 0x20);
#ifdef DEBUG_SETUP_FUNC
		//debug_printf("%02X ",0x20);
#endif
		}
#ifdef DEBUG_SETUP_FUNC
		//debug_printf("\n");
#endif
}

uchar ChanegName(char *src, uchar position, uchar len, uchar mode)
{
#ifdef DEBUG_SETUP_FUNC
	//debug_printf("\nmode:%d,position:%d->%c\nname:%s\n",mode,position,src[position],src);
#endif
	switch(mode)
		{
		case _NAME_UP_KEY: /*改变显示字符*/
			if(src[position] < 0x7F)
				src[position] ++;
			break;
		case _NAME_DOWN_KEY:  /*改变显示字符*/
			if(src[position] > 0x20)
				src[position] --;
			break;
		case _NAME_LEFT_KEY:  /*改变显示字符位置(向左移)*/
			if(position)
				position --;
			break;
		case _NAME_RIGHT_KEY: /*改变显示字符位置(向右移)*/
			if(position < (len-1))
				position ++;
			break;
		case _NAME_ENTER_KEY: /*按Enter 键字符显示的位置都在第0个位置*/
			position = 0;
			break;
		}
#ifdef DEBUG_SETUP_FUNC
	debug_printf("%s\n",src);
#endif
	src[len] = 0;
	return position;
}

SETUP_T InputNameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	static unsigned char position = 0;
	switch(bMode)
		{
		case EXECUTE_MODE:
			position = ChanegName(SrcName[resValue], position, INPUT_NAME_LENGTH, bValue);
			return INPUT_NAME_LENGTH - position;

		case WRITE_VALUE_MODE:	
			WriteName(INPUT1_NAME_POSITION + (resValue * INPUT_NAME_LENGTH), INPUT_NAME_LENGTH, SrcName[resValue]);
			break;

		case READ_VALUE_MODE:
			ReadName(INPUT1_NAME_POSITION + (resValue * INPUT_NAME_LENGTH), INPUT_NAME_LENGTH, SrcName[resValue]);
			break;

		case LOAD_DEFAULT_MODE:
			memcpy(SrcName[resValue], DefaultName[resValue], INPUT_NAME_LENGTH);	
			break;
		}
	return 0;
}

const char CaryLogo[] = " CARY Cinema 12 HD ";      
SETUP_T LogoSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue)
{
	static unsigned char position = 0;
	switch(bMode)
		{
		case EXECUTE_MODE:
			position = ChanegName(C11Logo, position, PRODUCT_NAME_LENGTH, bValue);
			return PRODUCT_NAME_LENGTH - position;   

		case WRITE_VALUE_MODE:
			WriteName(PRODUCT_NAME_POSITION, PRODUCT_NAME_LENGTH, C11Logo);
			break;

		case READ_VALUE_MODE:
			ReadName(PRODUCT_NAME_POSITION, PRODUCT_NAME_LENGTH, C11Logo);
			break;

		case LOAD_DEFAULT_MODE:
			memcpy(C11Logo, CaryLogo, PRODUCT_NAME_LENGTH);
			break;
		}
	return 0;
} 

