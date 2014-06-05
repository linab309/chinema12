/* @file 		Dspdrv.c
 * @brief 		DSP Driver File
 *
 * @author 	chenjianming
 * @date 	  	2007-06-27
 * @version 	2.0
 */
 
#include "..\main\macro.h"
#include "..\main\debug.h"
#include "..\main\task.h"
#include "..\main\c11_msg_handler.h"
#include "..\driver\ioport.h"
#include "..\driver\Dsp_Spi.h"
#include "..\driver\Cs8416.h"
#include "..\driver\Volume.h"
#include "..\driver\channel.h"
#include "..\driver\Hdmi.h"
#include "..\driver\Dac.h"
#include "dspdrv.h"
#include "DSP_API.h"
#include "DspInputOutputCfg.h"
#include "OS_Manager.h"
#include "AudioManager.h"
#include "PCM_Manager.h"
#include "DolbyManager.h"
#include "SignalGenerator.h"
#include "BassManager.h"
#include "DolbyPrologicII.h"
#include "DolbyPrologicIIx.h"
#include "Eq_Manager.h"
#include "DelayManager.h"
#include "Irc1_Manager.h"
#include "Irc2_Manager.h"
#include "DTS_ES_Manager.h"
#include "DTS_Manager.h"
#include "DTS_NEO6_Manager.h"
#include "DolbyHeadphone.h"
#include "..\driver\eeprom.h"
//#include "dsp_protocol.h"

#include <string.h>
#include <math.h>

#ifdef _DEBUG
#define _DEBUG_DSPDRV
#endif

#define OUT_MODE 	0x09  /*0x09= 3/4 L/C/R/LS/RS/SBL/SBR*/

unsigned char PreDspMode=8; 
unsigned char compression_mode = 0;
unsigned char enable_eq = 1;
unsigned char enable_bass = 1;
unsigned char enable_delay = 1;

unsigned char system_spk_mode[8] = {0};
unsigned char dsp_buf[DSP_MAX_MSG_SIZE];
unsigned char CurrentSampleFreq = FREQ_48KHZ;
unsigned char CurrentIputPort=Spdif_AutoSwitch;
unsigned char stream_type = STREAM_PCM;
unsigned char dsp_force_change_code = 1;

unsigned char DSDType;
unsigned char PcmType;
unsigned char DolbyType = _DOLBY_5CH;
unsigned char DtsType = _DTS;

extern const float balance_table[];
extern unsigned char Movie_Music_Speaker_Flag; 

unsigned char softmute_flag=0;
unsigned long last_stream=0;
extern unsigned char Pre_TureHD_Type;
extern unsigned char Pre_DTS_HD_Type;
extern unsigned char CD_Pure_Audio;


float ReConvert_8_24_Value(long in_value)
{
	float rtn_value;
	rtn_value = in_value / POW_2_24;
	return rtn_value;
}

float ConvertBalanceValue(float float_value)
{
	short temp_value;
	temp_value = float_value * 10;
	temp_value /= 5;
	float_value = temp_value * 5;
	float_value /= 10;
	return float_value;
}

/*
 * @brief transition the array type to word type
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang Cll code
 */
void ArrayToDWORD (DWORD *pdwMsg, unsigned char *message_array) 
{
	*pdwMsg = 0;
	*pdwMsg |= message_array[0];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[1];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[2];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[3];
	*pdwMsg <<=8;
	
	*pdwMsg |= message_array[4];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[5];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[6];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[7];	
	return;
}

/*
 * @brief transition the array type to half word type
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang Cll code
 */
void ArrayToHWORD (DWORD *pdwMsg, unsigned char *message_array) 
{
	*pdwMsg = 0;
	*pdwMsg |= message_array[0];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[1];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[2];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[3];  
	return;
}

/* 
 * @brief Panorama Width Setting
 *
 * @author chenjianming
 * @date 2008-2-23
 * @refer 
 */
void Set_Prologic2Width(unsigned char width)
{
	if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))
		{
		DSP_PLII_DECODE_MODE(Music_Mode); /*设置为music mode*/
		DSP_PLII_CENTER_WIDTH_CONFIGURATION(width);
		}
}

/* 
 * @brief Panorama Dimension Setting
 *
 * @author chenjianming
 * @date 2008-2-23
 * @refer 
 */
void Set_Prologic2Dimension(unsigned char Dimension)
{		
	if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))	
		{
		DSP_PLII_DECODE_MODE(Music_Mode); /*设置为music mode*/
		DSP_PLII_DIMENSION_CONFIGURATION(Dimension);
		}
}

/* 
 * @brief Panorama Enable Control
 *
 * @author chenjianming
 * @date 2008-2-23
 * @refer 
 */
void Set_Prologic2Panorama_On_Or_Off(unsigned char enable)
{
	if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))
		{
			DSP_PLII_Control(Enable); /*Prologic2 Enable*/
			DSP_PLII_DECODE_MODE(Music_Mode); /*设置为music mode*/
			if(enable)
				DSP_PLII_CONTROL_REGISTER(Enable); /*Panorama Enable*/
			else
				DSP_PLII_CONTROL_REGISTER(Disable); /*Panorama Disable*/		
		}
}

/* 
 * @brief Set Speaker Config 功能函数 
 *
 * @author chenjianming
 * @date 2007-12-4
 * @refer chenshiguang C11 code
 */
#ifdef _USE_DSP_PROTOCOL
void DspProcSpkMode(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcSpkMode(unsigned char channel,unsigned char mode)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	assert(msg_size < 2);

	spk_mode = buffer[3];

	switch(buffer[2])
#else
	system_spk_mode[channel] = mode;
#endif

	SetBassManagerSpeaker();
}

/*
 * @brief Set Distance 功能函数
 *
 * @author chenjianming
 * @date 2007-12-5
 * @refer chenshiguang C11 code
 */
#ifdef _USE_DSP_PROTOCOL
void DspProcDelay(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcDelay(unsigned char channel,unsigned short delay_time)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	unsigned short delay_time = 0;
	assert(msg_size < 3);

	delay_time = buffer[3];
	delay_time <<= 8;
	delay_time |= buffer[4];

	switch(buffer[2])
#else
	switch(channel)
#endif
		{
		case CH_L:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_fl_dtime = delay_time;
			else
				dsp_delay_music_time.spk_fl_dtime=delay_time;
			break;
		case CH_C:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_c_dtime = delay_time;
			else
				dsp_delay_music_time.spk_c_dtime = delay_time;
			break;
		case CH_R:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_fr_dtime = delay_time;
			else
				dsp_delay_music_time.spk_fr_dtime = delay_time;
			break;
		case CH_LS:
			if(Movie_Music_Speaker_Flag==0) 
				dsp_delay_time.spk_ls_dtime = delay_time;
			else
				dsp_delay_music_time.spk_ls_dtime = delay_time;
			break;
		case CH_RS:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_rs_dtime = delay_time;
			else
				dsp_delay_music_time.spk_rs_dtime = delay_time;
			break;
		case CH_LB:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_sbl_dtime = delay_time;
			else
				dsp_delay_music_time.spk_sbl_dtime = delay_time;
			break;
		case CH_RB:
			if(Movie_Music_Speaker_Flag==0) 
				dsp_delay_time.spk_sbr_dtime = delay_time;
			else
				dsp_delay_music_time.spk_sbr_dtime = delay_time;
			break;
		case CH_SW:
			if(Movie_Music_Speaker_Flag==0)
				dsp_delay_time.spk_lfe_dtime = delay_time;
			else
				dsp_delay_music_time.spk_lfe_dtime = delay_time;
			break;
		}

#ifdef _DEBUG_DSPDRV
		//debug_printf("\n@@@@@@dsp_delay_time.spk_fl_dtime=%d",dsp_delay_time.spk_fl_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_c_dtime=%d",dsp_delay_time.spk_c_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_fr_dtime=%d",dsp_delay_time.spk_fr_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_ls_dtime=%d",dsp_delay_time.spk_ls_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_rs_dtime=%d",dsp_delay_time.spk_rs_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_sbl_dtime=%d",dsp_delay_time.spk_sbl_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_sbr_dtime=%d",dsp_delay_time.spk_sbr_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_lfe_dtime=%d",dsp_delay_time.spk_lfe_dtime);

		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_fl_dtime=%d",dsp_delay_music_time.spk_fl_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_c_dtime=%d",dsp_delay_music_time.spk_c_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_fr_dtime=%d",dsp_delay_music_time.spk_fr_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_ls_dtime=%d",dsp_delay_music_time.spk_ls_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_rs_dtime=%d",dsp_delay_music_time.spk_rs_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_sbl_dtime=%d",dsp_delay_music_time.spk_sbl_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_sbr_dtime=%d",dsp_delay_music_time.spk_sbr_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_lfe_dtime=%d",dsp_delay_music_time.spk_lfe_dtime);	
#endif

	if(dsp_power_flag) 
		{
#ifdef _USE_DSP_PROTOCOL
		switch(buffer[2])
#else
		switch(channel)
#endif
			{
			case CH_L:
				DSP_Delay_Channel_Setting(CH_L,dsp_delay_time.spk_fl_dtime);
				break;
			case CH_C:
				DSP_Delay_Channel_Setting(CH_C,dsp_delay_time.spk_c_dtime);
				break;
			case CH_R:
				DSP_Delay_Channel_Setting(CH_R,dsp_delay_time.spk_fr_dtime);
				break;
			case CH_LS:
				DSP_Delay_Channel_Setting(CH_LS,dsp_delay_time.spk_ls_dtime);
				break;
			case CH_RS:
				DSP_Delay_Channel_Setting(CH_RS,dsp_delay_time.spk_rs_dtime);
				break;
			case CH_LB:
				DSP_Delay_Channel_Setting(CH_LB,dsp_delay_time.spk_sbl_dtime);
				break;
			case CH_RB:
				DSP_Delay_Channel_Setting(CH_RB,dsp_delay_time.spk_sbr_dtime);
				break;
			case CH_SW:
				DSP_Delay_Channel_Setting(CH_SW,dsp_delay_time.spk_lfe_dtime);
				break;
			}
		}
}

#ifdef _USE_DSP_PROTOCOL
void DspProcCorner(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcCorner(unsigned char channel,unsigned char corner_value)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char corner_value = 0;

	assert(msg_size < 2);

	corner_value = buffer[3];

	switch(buffer[2]) 
#else
	switch(channel)
#endif
		{
		case CH_L:
			dsp_bass_corner.fl_corner = corner_value;
			break;
		case CH_C:
			dsp_bass_corner.cen_corner = corner_value;
			break;
		case CH_R:
			dsp_bass_corner.fr_corner = corner_value;
			break;
		case CH_LS:
			dsp_bass_corner.sl_corner = corner_value;
			break;
		case CH_RS:
			dsp_bass_corner.sr_corner = corner_value;
			break;
		case CH_LB:
			dsp_bass_corner.sbl_corner = corner_value;
			break;
		case CH_RB:
			dsp_bass_corner.sbr_corner = corner_value;
			break;
		case CH_SW:
			dsp_bass_corner.sum1_corner = corner_value;
			dsp_bass_corner.sum2_corner = corner_value;
			dsp_bass_corner.sum3_corner = corner_value;
			dsp_bass_corner.sum4_corner = corner_value;
			dsp_bass_corner.sum5_corner = corner_value;
			break;
		}
	if(dsp_power_flag)
		{
#ifdef _USE_DSP_PROTOCOL
		switch(buffer[2])
#else
		SetBassManagerSpeaker();
#endif
		}
}

#ifdef _USE_DSP_PROTOCOL
void DspProcSignal(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcSignal(unsigned char signal_type, unsigned char channel, unsigned char freq)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char channel, freq;

	assert(msg_size < 1);
#endif
	if(!dsp_power_flag)	/*dsp not power, return*/
		return;
	
#ifdef _USE_DSP_PROTOCOL
	channel = buffer[3];
	freq = buffer[4];

	switch(buffer[2])
#else
	switch(signal_type)
#endif
		{
		case Signal_White_Noise:		
			DSP_SGEN_CHANNEL_ENABLE(channel);			
			break;	
		case Signal_Stop:
			DSP_SGEN_ENABLE(0);    
			//Change_Input_Audio_Source(CurrentIputPort); /*通道切回到当前输入口,由于测试粉红噪声时做了输入口切换,重新切回去*/	
			//DelayMs(30);
			//Change_DSP_Decoder_Code(1,ULD_ID_AC3); /*切换下解码固件*/        			
			break;
		}
}

#define SPK_TEST_TIME 3
#ifdef _USE_DSP_PROTOCOL
void DspProcIrc(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcIrc(unsigned char irc_mode,unsigned char test_channel)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	unsigned long irc_value = 0;
	uchar test_channel;

	test_channel = buffer[3];

	assert(msg_size < 1);
#endif
	if(!dsp_power_flag)
		return;
#ifdef _USE_DSP_PROTOCOL
	switch(buffer[2])
#else
	switch(irc_mode)
#endif
		{
		case IRC_TEST_INIT: /*Load IRC2 code and init */     
			DacMute(1);  /*mute 掉导码时的噪声*/
			Change_DSP_VIRTUALIZER_Code(0,ULD_ID_IRC); /*切换为IRC 固件*/
			DelayMs(100); /*加载完固件Kick Start 后,DSP还处于不稳定状态,延时下*/
			DSP_IRC1_Test_Duration(SPK_TEST_TIME); /*IRC Test Time=3*/
			DSP_Rx_Channel(CH_L); /*选择MIC输入通道*/		
			InitMicParameter(); /*设置麦克风增益参数*/  
			DacMute(0);  /*mute 掉导码时的噪声*/
			break;
		case IRC_TEST_AND_SETUP_DELAY: /*Test Speaker Status*/
			DacMute(1);  /*mute 掉导码时的噪声*/
			Dsp_IRC2_Speaker_Select(test_channel,0); /*Speaker 通道选择*/			
			Dsp_IRC2_Control(0x01); /*Dealy/Polarity Disable passthrough*/		  		
			DacMute(0);  /*mute 掉导码时的噪声*/
			break;
		case IRC_TEST_AND_SETUP_TRIMS: /*Test Speaker Size and level*/
			DacMute(1);  /*mute 掉导码时的噪声*/
			Dsp_IRC2_Speaker_Select(test_channel,0); /*Speaker 通道选择*/
			Dsp_IRC2_Control(0x02); /*Trim mode--Balance 测试/Disable passthrough*/
			DacMute(0);  /*mute 掉导码时的噪声*/
			break;
		case IRC_TEST_AND_SETUP_EQ: /*Test EQ*/		
			DacMute(1);  /*mute 掉导码时的噪声*/			
			Dsp_IRC2_Speaker_Select(test_channel,1); /*Speaker 通道选择 EQ enable*/  
			Dsp_IRC2_Control(0x08); /*Speaker EQ Calibration /Disable passthrough*/	    
			DacMute(0);  /*mute 掉导码时的噪声*/  
			break;    
		}
}

void SetDh2On(unsigned char on)
{    
//#ifdef _DEBUG_DSPDRV			
	//debug_printf("\nsys_flag.stream_type=%d",sys_flag.stream_type);
	//debug_printf("\nsys_flag.stream_subtype=%d",sys_flag.stream_subtype);
//#endif	

	/*高清不做Headphone 功能*/
	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_PLUS)) /*Dolby Plus 不支持Headphone*/
		{
		return;
		}

	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_2ch)) /*Dolby Ture HD2.0不支持Headphone*/
		{
		return;
		}

	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_5ch)) /*Dolby Ture HD5.1不支持Headphone*/
		{
		return;
		}

	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_6ch)) /*Dolby Ture HD6.1不支持Headphone*/
		{
		return;
		}

	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_7ch)) /*Dolby Ture HD7.1不支持Headphone*/
		{
		return;
		}
		
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_MA_48K)) /*DTS_HD_48K(2.0/5.1/7.1)不支持Headphone*/
		{
		return;
		}
		
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_MA_96k_192K)) /*DTS_HD_96K(2.0/5.1/7.1)不支持Headphone*/
		{
		return;
		}
	
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_HRA_48K)) /*DTS_HD_HRA_48k不支持Headphone*/
		{
		return;
		}
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_HRA_96K)) /*DTS_HD_HRA_48k不支持Headphone*/
		{
		return;
		}
	 
	if(on)
		{
		Change_DSP_VIRTUALIZER_Code(DH2_MODE_BYPASS,ULD_ID_DH2); /*Load DolbyHeadphone code*/
		DelayMs(30); /*加载稳定后再做设置*/
		}
	else 
		{
		Change_DSP_VIRTUALIZER_Code(0,ULD_ID_INVALID); /*Remove DolbyHeadphone code*/
		}
}

void DspChannelOut5_1(void)
{
	OS_OUTPUT_MODE_CONTROL(Disable,Stereo,L_C_R_LS_RS); /* (3/2 L_C_R_LS_RS)*/	
	OS_OUTPUT_MODE_CONTROL_FOR_DSPB(Disable,Stereo,L_C_R_LS_RS); /* (3/2 L_C_R_LS_RS)*/
}

void DspChannelOut7_1(void)
{
	OS_OUTPUT_MODE_CONTROL(Disable,Stereo,L_C_R_Ls_Rs_Sbl_Sbr); /* (3/4 L/C/R/LS/RS/SBL/SBR)*/	
	OS_OUTPUT_MODE_CONTROL_FOR_DSPB(Disable,Stereo,L_C_R_Ls_Rs_Sbl_Sbr); /* (3/4 L/C/R/LS/RS/SBL/SBR)*/
}

void DspChannelOutSetting(unsigned char mode)
{
	switch(mode)
		{
		case PCM_MODE_PL2:
			DspChannelOut5_1();
			break;
		case PCM_MODE_PL2X:
			DspChannelOut7_1();
			break;
		default:
			break;
		}
}

/*DSP引导成功后加载菜单中与DSP 相关的设置*/
extern unsigned char DSP_HDMI_Setting_Flag;
void DspLoadDefaultSetting(unsigned char mode)
{
	if(mode==0)
		{  
		//SoftMute(1);      
#ifdef _DEBUG_DSPDRV	
		debug_printf("\n___Before DspLoadDefaultSetting  :%d",PreDspMode);
#endif		
		if(PreDspMode==3)/*Analog 2 channel +NEO6/PL2X时没办法加载APP,故做此条件判断*/
			{
			if((sys_flag.decode_mode.pcm_decode_mode==PCM_MODE_NORMAL))
				{			
				/*add by cjm 2009-4-7*/
				DelayMs(300);  
				DSP_BOOT(configuration_lock,boot_idle);
				DSP_CFG_PPM(ULD_ID_APP);  /*加载APP固件*/
				DelayMs(30);
				DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/	
				DSP_BOOT(configuration_unlock,change_to_new_config);
				DelayMs(300); 
				DSP_Delay_Firmware_Init(); /*加载Delay设置值*/
				SetBassManagerSpeaker(); /*加载低音管理设置值*/
				DSP_EQ_Firmware_Init();		 
				/*add by cjm 2009-4-7*/
				}
			else /*Analog 2 channel +NEO6/PL2X时没办法加载APP,故做此条件判断*/
				{
				DelayMs(200); 
				DSP_Delay_Firmware_Init(); /*加载Delay设置值*/
				SetBassManagerSpeaker(); /*加载低音管理设置值*/				
				}
			}
		else /*光纤和同轴*/
			{
			DelayMs(200); 
			DSP_Delay_Firmware_Init(); /*加载Delay设置值*/
			SetBassManagerSpeaker(); /*加载低音管理设置值*/			
			DSP_EQ_Firmware_Init();  
			}
#ifdef _DEBUG_DSPDRV	
		debug_printf("\n___After DspLoadDefaultSetting");
#endif			
		DSP_AV_Delay(EepromReadByte(AV_SYNC_POSITION)); // jian add in 10-08-05

		SoftMute(0); 
//		if(sys_flag.zone2_channel == INPUT_HDMI)
			Zone2SelectChannel(sys_flag.zone2_channel);
		}
	else
		{	   
		if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
			{
			Change_DSP_VIRTUALIZER_Code(0,ULD_ID_CROSSBAR_B); /*HDMI 只能加载CROSSBAR_B 而不能加载CROSSBAR*/
			DSP_HDMI_Setting_Flag=1;
			//DelayMs(300); /*加载完固件后延时下*/      
			//MsgSetDSPHDMISetting(ZONE1_ID,0); /*加载菜单设置*/
			}
		}  
}
  
extern char dsp_power_flag;
extern unsigned char Pre_AC3_Type; 
extern unsigned char Pre_DTS_Type; 
extern unsigned char Pre_TureHD_Type;
extern unsigned char Pre_DTS_HD_Type;
void DspProcPower(unsigned char mode)
{
	if(mode!=PreDspMode)
		{
#ifdef _DEBUG_DSPDRV	
		debug_printf("\nDiffer Dsp power mode=%d",mode);
#endif				
		PreDspMode=mode;
		}
	else
		{
#ifdef _DEBUG_DSPDRV	
		debug_printf("\nThe same Dsp power mode=%d",mode);
#endif			
		return;
		}

	if(mode==0) /*复位状态*/
		{
		DSPHwReset(1);
		dsp_power_flag = 0;

//		if(SUCCESS == DspMasterBoot())
//			dsp_power_flag = 1;	/*boot success */
//		else
//			dsp_power_flag = 0; /*boot fail*/			
		
		return ;
		}
	
	if(SUCCESS == DspMasterBoot())
		dsp_power_flag = 1;	/*boot success */
	else
		dsp_power_flag = 0; /*boot fail*/	
	
	switch(mode)
		{
		case 1: //SPDIF
			Pre_AC3_Type=0; /*每次重新引导后清掉当前Dolby流信息*/
			Pre_DTS_Type=0; /*每次重新引导后清掉当前dts流信息*/
			Change_Input_Audio_Source(Spdif_AutoSwitch); 	
			break;
		case 2: //Multi Analog		  
		case 3: //Multi Analog	
			Change_Input_Audio_Source(Multi_Channel_Analog);	
			break;
		case 4: //HDMI			
			Pre_TureHD_Type=0;
			Pre_DTS_HD_Type=0;
			Change_Input_Audio_Source(HDMI_AutoSwitch);	  
			break;
		default:
			break;
		}  

#if 0	 // jian remote it in 12-03-08
        if(last_stream ==  LINEAR_5_1_PCM_STREAM)
		ReadDsp();
#endif		
}

/*
 * @brief 设置所有的MIXER 为Stereo
 *
 * 每个单独声道都是由其它8个声道组成(L/C/R/LS/RS/SLB/SRB/LFE)
 * 立体声的理解就是(左声道就给左声道，右声道就给右声道)
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer AN246MPC
 * 		chenshiguang Cll code
 */
void SetMixerAllStereo()
{
#if 0
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x00,0x7FFFFFFF); /*FL*/ /*左声道就只取左声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x02,0);
	
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x08,0x3FFFFFFF); /*CEN*/ /*中置各取1/2*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0A,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x10,0);		     /*FR*/ /*右声道也只取右声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x12,0x7FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x18,0x7FFFFFFF); /*SL*/ /*左环绕就只取左声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x1A,0);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x20,0);		     /*SR*/ /*右环绕就只取右声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x22,0x7FFFFFFF);
	
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x28,0x7FFFFFFF); /*SBL*/ /*左后环绕就只取左声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x2A,0);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x30,0);		      /*SBR*/ /*右后环绕就只取右声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x32,0x7FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x38,0x3FFFFFFF); /*在LFE声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x3A,0x3FFFFFFF);
#endif

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x00,0x1FFFFFFF); /*FL*/ /*左声道就只取左声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x02,0);
	
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x08,0x1FFFFFFF); /*CEN*/ /*中置各取1/2*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x10,0);		     /*FR*/ /*右声道也只取右声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x12,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x18,0x1FFFFFFF); /*SL*/ /*左环绕就只取左声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x1A,0);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x20,0);		     /*SR*/ /*右环绕就只取右声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x22,0x1FFFFFFF);
	
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x28,0x1FFFFFFF); /*SBL*/ /*左后环绕就只取左声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x2A,0);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x30,0);		      /*SBR*/ /*右后环绕就只取右声道*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x32,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x38,0x1FFFFFFF); /*在LFE声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x3A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x81,0x1111); 	     /*混频系数从HOST*/
}

/*  
 * @brief 设置所有的MIXER 为Mono
 *
 * 每个声道都是由其它8个声道组成(L/C/R/LS/RS/SLB/SRB/LFE)
 * 单声道的理解就是1/2(L+R)给L，同样1/2(L+R)给R。
 * 0X7FFFFFFF 的1/2 就是0X3FFFFFFF
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer AN246MPC
 * 		chenshiguang Cll code
 */
void SetMixerAllMono()
{
#if 0 
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x00,0x3FFFFFFF); /*在L声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x02,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x08,0x3FFFFFFF); /*在C声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0A,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x10,0x3FFFFFFF); /*在R声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x12,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x18,0x3FFFFFFF); /*在LS声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x1A,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x20,0x3FFFFFFF); /*在RS声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x22,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x28,0x3FFFFFFF); /*在SLB声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x2A,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x30,0x3FFFFFFF); /*在SRB声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x32,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x38,0x3FFFFFFF); /*在LFE声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x3A,0x3FFFFFFF);
#endif	

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x00,0x1FFFFFFF); /*在L声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x02,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x08,0x1FFFFFFF); /*在C声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x10,0x1FFFFFFF); /*在R声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x12,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x18,0x1FFFFFFF); /*在LS声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x1A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x20,0x1FFFFFFF); /*在RS声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x22,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x28,0x1FFFFFFF); /*在SLB声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x2A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x30,0x1FFFFFFF); /*在SRB声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x32,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x38,0x1FFFFFFF); /*在LFE声道中取1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x3A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x81,0x1111); 	     /*混频系数从HOST*/
}

/*
 * @brief 设置Crossbar effect
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang Cll code
 */
void DspSetCrossbarEffect(unsigned char mode)
{

#ifdef _DEBUG_DSPDRV	
	debug_printf("\DspSetCrossbarEffect : %d\n",mode);
#endif

	switch(mode)
		{
		case 1: /*CES 7.1 All Stereo*/
			SetMixerAllStereo(); /*立体声*/
			break;
		case 2:  /*CES 7.1 All Mono*/
			SetMixerAllMono(); /*单声道*/
			break;
		case 0:		
		default: /*CES 7.1*/
			Change_DSP_MATRIX_Code(CROSSBAR_MODE_COEF_L_R_7CH,ULD_ID_CROSSBAR); /*mode: 2chs to 7.1 chs,切换到CROSSBAR*/
			DelayMs(300);/*在切完码后要延时下等DSP 稳定下来*/  
			Change_DSP_VIRTUALIZER_Code(0,ULD_ID_INVALID); /*mode: 2chs to 7.1 chs,切换到CROSSBAR code*/	
			MsgSetDSPSetting(ZONE1_ID,0); /*切换完固件后APP/SPP 恢复为默认值,故重新加载DSPSETTING*/
			break;
		}
}		

/* 
 * @brief DSP 解PCM 流  
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang C11 code
 */
void DspDecodePcm(unsigned char decode_mode)
{	
#ifdef _DEBUG_DSPDRV	
	debug_printf("\nDspDecodePcm\n");
#endif

	if(GetNormalFreq() == FREQ_192KHZ)	/*192K不做其他效果*/
		decode_mode = PCM_MODE_NORMAL;

	switch(PcmType)
		{
		case _PCM_2_1:
			switch(decode_mode)
				{
				case PCM_MODE_PL2:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_PL2\n");
#endif
					Change_DSP_MATRIX_Code(PL2X_MODE_MUSIC,ULD_ID_PL2X);
					DelayMs(300);
					DspChannelOut5_1(); /*打开5.1 个声道*/ 
					break;
				case PCM_MODE_PL2X:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_PL2X\n");
#endif
					Change_DSP_MATRIX_Code(PL2X_MODE_DD_EX,ULD_ID_PL2X);
					DelayMs(300); /*在切完码后要延时下等DSP 稳定下来*/  
					DspChannelOut7_1(); /*打开7.1个通道*/
					break;			
				case PCM_MODE_NEO6:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_NEO6\n");
#endif
					//Change_DSP_MATRIX_Code(NEO6_MODE_MUSIC,ULD_ID_NEO6);
					Change_DSP_MATRIX_Code(PL2X_MODE_MUSIC,ULD_ID_PL2X); /*由于NEO6有问题,故用PL2X代替*/
					DelayMs(300); /*在切完码后要延时下等DSP 稳定下来*/  
					DspChannelOut7_1(); /*打开7.1个通道*/
		 			break;
				case PCM_MODE_CES:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_CES\n");
#endif
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_R_7CH,ULD_ID_CROSSBAR); /*mode: 2chs to 7.1 chs,切换到CROSSBAR code*/
					DelayMs(30);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);	/*把VIRTUALIZER Code切换为空码,不然会产生通道混乱BUG*/
					break;
				case PCM_MODE_NORMAL:
				default:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_NORMAL\n");
#endif
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR);  /*0-not mode,只切换到CROSSBAR code*/
					DelayMs(30);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					break;  
				}	 
			break;
		case _PCM_5_1:
			switch(decode_mode)
				{
				case PCM_MODE_PL2X:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_PL2X\n");
#endif
					Change_DSP_MATRIX_Code(PL2X_MODE_DD_EX,ULD_ID_PL2X);
					DelayMs(300); /*在切完码后要延时下等DSP 稳定下来*/  
					DspChannelOut7_1(); /*打开7.1个通道*/
					break;			
				case PCM_MODE_NEO6:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_NEO6\n");
#endif
					//Change_DSP_MATRIX_Code(NEO6_MODE_MUSIC,ULD_ID_NEO6);
					Change_DSP_MATRIX_Code(PL2X_MODE_MUSIC,ULD_ID_PL2X); /*由于NEO6有问题,故用PL2X代替*/
					DelayMs(300); /*在切完码后要延时下等DSP 稳定下来*/  
					DspChannelOut7_1(); /*打开7.1个通道*/
		 			break;
				case PCM_MODE_CES:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_CES\n");
#endif
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*mode: 5chs to 7.1 chs,切换到CROSSBAR code*/
					DelayMs(30);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID); /*把VIRTUALIZER Code切换为空码,不然会产生通道混乱BUG*/
					DSP_BOOT(configuration_unlock,change_to_new_config);
					break;
				case PCM_MODE_NORMAL:
				default:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_NORMAL\n");
#endif
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
					DelayMs(30);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);	
					break;  
				}
			break;
		case _PCM_7_1: /*不需要做处理*/
			break;
		default:
			break;
		}
}

#if 0
/* 
 * @brief   分析PCM 流信息变化后进行相应的处理
 *
 * @author chenjianming
 * @date 2008-07-04
 * @refer 
 */
char Pcm_Cfg_Change_Deal(void)
{	
#ifdef _DEBUG_DSPDRV
	debug_printf("\nPcm_Cfg_Change_Deal");
#endif
	if(dsp_power_flag==1)
		{    
		if(stream_type==STREAM_PCM) /*流已经是PCM 了,就不需要重新再解*/
			return;

		DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);	  
		}	
}
#endif

/* 
 * @brief DSP 解Dolby 流 
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer  chenshiguang C11 code
 */
void DspDecodeAc3(unsigned char Decode_Mode)
{
#ifdef _DEBUG_DSPDRV	
	debug_printf("\nDspDecodeAc3\n");
#endif

	switch(DolbyType)
		{
		case _DOLBY_2CH:
			DSP_BOOT(configuration_lock,boot_idle);
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2:
#ifdef _DEBUG_DSPDRV	
					debug_printf("Dolby_2Ch_PL2\n");
#endif				
					DSP_CFG_MATRIX(PL2X_MODE_MUSIC,ULD_ID_PL2X);
					break;
				case AC3_MODE_PL2X:
#ifdef _DEBUG_DSPDRV	
					debug_printf("Dolby_2Ch_PL2X\n");    
#endif							
					DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X);
					break;
				case AC3_MODE_CES_ON:
#ifdef _DEBUG_DSPDRV	
					debug_printf("Dolby_2Ch_CES_ON\n");
#endif			
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_R_7CH,ULD_ID_CROSSBAR); /*mode: 2chs to 7.1 chs,切换到CROSSBAR*/
					DelayMs(30);/*在切完码后要延时下等DSP 稳定下来*/  
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;
				case AC3_MODE_NORMAL:				
				default:	
#ifdef _DEBUG_DSPDRV	
					debug_printf("Dolby_2Ch_NORMAL\n");
#endif			
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
					DelayMs(30);/*在切完码后要延时下等DSP 稳定下来*/  
					if(sys_flag.headphone_flag)
						DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
					else
						DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;			
				}
			DelayMs(30);
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/	
			DSP_BOOT(configuration_unlock,change_to_new_config);
			break;   
		case _DOLBY_5CH:
			DSP_BOOT(configuration_lock,boot_idle);
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
#ifdef _DEBUG_DSPDRV
					debug_printf("Dolby_5Ch_PL2X\n");
#endif
					DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X);
					break;
				case AC3_MODE_CES_ON:  
#ifdef _DEBUG_DSPDRV
					debug_printf("Dolby_5Ch_CES_ON\n");
#endif
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*mode: 5chs to 7.1 chs,切换到CROSSBAR code*/
					DelayMs(30);/*在切完码后要延时下等DSP 稳定下来*/  
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;
				case AC3_MODE_NORMAL:
				default:
#ifdef _DEBUG_DSPDRV
					debug_printf("Dolby_5Ch_NORMAL\n");
#endif				
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
					DelayMs(30);/*在切完码后要延时下等DSP 稳定下来*/  
					if(sys_flag.headphone_flag)
						DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
					else
						DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;
				}
			DelayMs(30);
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/	
			DSP_BOOT(configuration_unlock,change_to_new_config);			
			break;
		case _DOLBY_EX:
			DSP_BOOT(configuration_lock,boot_idle);
			switch(Decode_Mode)
				{
				default:
#ifdef _DEBUG_DSPDRV
  				debug_printf("Dolby_EX_Normal\n");    
#endif			
				DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*加入crossbar 使discrete 变成7.1*/
	//			DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X); /*切换到PL2X EX mode*/	 jian change in 11-06-22
				if(sys_flag.headphone_flag)
					DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
				else
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
				break;
				}
			DelayMs(30);
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/	
			DSP_BOOT(configuration_unlock,change_to_new_config);			
			break;  
		//case _DOLBY_PLUS: /*当前暂不做处理,市面上暂只有PLUS7.1*/
		//	break;
		case _DOLBY_TRUE_HD_2ch: /*TureHD2.0和5.1只做PLIIx处理*/
		case _DOLBY_TRUE_HD_5ch:		
		case _DOLBY_TRUE_HD_6ch:		
			DSP_BOOT(configuration_lock,boot_idle);
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
#ifdef _DEBUG_DSPDRV
  					debug_printf("Dolby TureHD + PL IIx\n");    
#endif						
					DSP_CFG_VIRTUALIZER(PL2X_B_MODE_MOVIE,ULD_ID_PL2X_B);
					break;
				case AC3_MODE_NORMAL:
				default:
#ifdef _DEBUG_DSPDRV
					debug_printf("Dolby TureHD Normal\n");    
#endif
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;
				}
			DelayMs(30);
			DSP_BOOT(configuration_unlock,change_to_new_config);
			break;
		//case _DOLBY_TRUE_HD_7ch: /*不需要额外处理*/
		//	break;
	}
	
	switch(DolbyType)
		{
		case _DOLBY_2CH:
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2:
					break;
				case AC3_MODE_PL2X:
					DelayMs(300); /*在切完码后要延时下等DSP 稳定下来*/  
					DspChannelOut7_1(); /*打开7.1个通道*/
					break;
				case AC3_MODE_CES_ON:
					break;
				case AC3_MODE_NORMAL:				
				default:						
					break;			
				}
			DelayMs(300); 
			DSP_AC3_DRC_INIT();
			break;   
		case _DOLBY_5CH:
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
					DelayMs(300); /*在切完码后要延时下等DSP 稳定下来*/  
					DspChannelOut7_1(); /*打开7.1个通道*/					
					break;
				case AC3_MODE_CES_ON:  
					break;
				case AC3_MODE_NORMAL:
				default:
					break;
				}
			DelayMs(300); 
			DSP_AC3_DRC_INIT();
			break;
		case _DOLBY_EX:
			switch(Decode_Mode)
				{
				default:
					DelayMs(300); /*切换完Code等待DSP 稳定*/
					DspChannelOut7_1(); /*打开7.1个通道*/  
					break;
				}
			DelayMs(300);  
			DSP_AC3_DRC_INIT();
			break;  
		//case _DOLBY_PLUS: /*DRC 默认设置在DSPloop里*/
		//	break;
		case _DOLBY_TRUE_HD_2ch:
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
					DelayMs(300); /*切换完Code等待DSP 稳定*/
					DspChannelOut7_1(); /*打开7.1个通道*/    
					break;
				case AC3_MODE_NORMAL:
				default:
					break;
				}
			DelayMs(300); 
			DSP_TRUEHD_DRC_INIT();
			break;
		case _DOLBY_TRUE_HD_5ch:		
		case _DOLBY_TRUE_HD_6ch:		
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
					DelayMs(300); /*切换完Code等待DSP 稳定*/
					DspChannelOut7_1(); /*打开7.1个通道*/  
					break;
				case AC3_MODE_NORMAL:
				default:
					break;
				}			
			DelayMs(300); 
			DSP_TRUEHD_DRC_INIT();
			break;
		case _DOLBY_TRUE_HD_7ch:
			DelayMs(300); 
			DSP_TRUEHD_DRC_INIT();  
			break;
		default:
			break;
		}	
}  

/* 
 * @brief   分析AC3 流信息变化后进行相应的Code 切换
 *
 * @author chenjianming
 * @date 2008-07-04
 * @refer
 */
unsigned char Pre_AC3_Type=0; 
char AC3_Cfg_Change_Deal(unsigned char AC3_Type)
{	
#ifdef _DEBUG_DSPDRV
	debug_printf("\nAC3_Cfg_Change_Deal");
	debug_printf("\nAC3_Type=%d",AC3_Type);
	debug_printf("\nPre_AC3_Type=%d",Pre_AC3_Type);  
	debug_printf("\dsp_power_flag=%d",dsp_power_flag);  
#endif
	if(dsp_power_flag==1)
		{    
		if(Pre_AC3_Type==AC3_Type)
			return 0;

		DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);	
		MsgSetDSPSetting(ZONE1_ID,0); /*加载DSPSETTING*/
		Pre_AC3_Type=AC3_Type;
		}	
}

/* 
 * @brief   分析AC3 信息后进行解码
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
extern unsigned char Dolby2ch_Flag; 
void AnalyseAc3Cfg(_AC3_STREAM ac3_stream)  
{	
	unsigned char dolby_type=_DOLBY_5CH;
	
#ifdef _DEBUG_DSPDRV
	//dsp_printf("\nacmod:%X\n",ac3_stream.acmode);	
	//dsp_printf("dsurmod:%X\n",ac3_stream.dsurmod);
	//dsp_printf("bsid:%X\n",ac3_stream.bsid);
	//dsp_printf("lfeon:%X\n",ac3_stream.lfeon);
	//dsp_printf("TIMECOD2:%X\n\n",ac3_stream.timecod2);
#endif

	//if(ac3_stream.bsid == 6) 
	if((ac3_stream.timecod2 == 0x2) && (ac3_stream.bsid == 6)) 
		{
#ifdef _DEBUG_DSPDRV
		debug_printf("\nAnalyse AC3:Dolby_EX");
#endif
		dolby_type = _DOLBY_EX;
		}
	else if(ac3_stream.acmode == 2)
		{
#ifdef _DEBUG_DSPDRV
		debug_printf("\nAnalyse AC3:Dolby_2CH");
#endif	
		dolby_type = _DOLBY_2CH;
		}	

	DolbyType = dolby_type;
	sys_flag.decode_mode.dolby_type = DolbyType;
	MsgSetStream(stream_type,DolbyType);	
	AC3_Cfg_Change_Deal(dolby_type);
}

/* 
 * @brief   分析Dolby Ture HD 流信息变化后进行相应的Code切换
 *
 * @author chenjianming
 * @date 2008-12-16
 * @refer 
 */
unsigned char Pre_TureHD_Type=0;
char TureHD_Cfg_Change_Deal(unsigned char TureHD_Type)
{
#ifdef _DEBUG_DSPDRV
	debug_printf("\nTureHD_Cfg_Change_Deal");
	debug_printf("\nTureHD_Type=%d",TureHD_Type);
	debug_printf("\nPre_TureHD_Type=%d",Pre_TureHD_Type);
#endif

	if(dsp_power_flag==1)
		{
		if(Pre_TureHD_Type==TureHD_Type)
			return 0 ;
		DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
		Pre_TureHD_Type=TureHD_Type; 
		}
}

/* 
 * @brief   分析Dolby Ture HD 信息后进行解码
 *
 * @author chenjianming
 * @date 2008-12-16
 * @refer 
 */
void AnalyseTureHDCfg(unsigned char *uCmdBuf)
{
	unsigned char dolby_type;
	unsigned char TureHD_Channels;

	//DelayMs(100); /*延时下,使得可以正确读到下面这个寄存器的值*/
	//TureHD_Channels=DspReadTRUEHD(0x0a);

	uCmdBuf+=12;  
	TureHD_Channels=uCmdBuf[3]&0x0F;
#ifdef _DEBUG_DSPDRV	
	debug_printf("\nuCmdBuf[3]=%x",uCmdBuf[3]); 
	debug_printf("\nTureHD_Channels=%d",TureHD_Channels);
#endif

	switch(TureHD_Channels)
		{
		case 2: /*Ture HD 2.0*/
			dolby_type=_DOLBY_TRUE_HD_2ch;
			break;
		case 7: /*Ture HD 5.1*/
			dolby_type=_DOLBY_TRUE_HD_5ch;
			break;
		case 8: /*Ture HD 6.1*/
			dolby_type=_DOLBY_TRUE_HD_6ch;
			break;
		case 9: /*Ture HD 7.1*/
			dolby_type=_DOLBY_TRUE_HD_7ch;
			break;
		}
	
	DolbyType = dolby_type;
	sys_flag.decode_mode.dolby_type = DolbyType;
	MsgSetStream(stream_type,DolbyType);	
	TureHD_Cfg_Change_Deal(dolby_type);
}

/* 
 * @brief  DTS 流信息变化后进行相应的Code 切换
 *
 * @author chenjianming
 * @date 2008-07-04
 * @refer 
 */
void DspDecodeDTS(unsigned char decode_mode)
{
	switch(sys_flag.decode_mode.dts_type) /*只有DTS 和DTS 96/24才做CES7.1 ON*/
		{
		case _DTS:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nDTS\n");
#endif					
			DSP_BOOT(configuration_lock,boot_idle);
			if(decode_mode==DTS_MODE_CES_ON)
				{
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_MODE_CES_ON\n");
#endif					
				Change_DSP_MATRIX_Code(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR);/*mode: 5chs to 7.1 chs,切换到CROSSBAR code*/
				DelayMs(300);/*在切完码后要延时下等DSP 稳定下来*/  
				Change_DSP_VIRTUALIZER_Code(0,ULD_ID_INVALID); /*把VIRTUALIZER Code切换为空码,不然会产生BUG*/					
				}
			else
				{
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_MODE_NORMAL\n");  
#endif		
				DSP_CFG_DECODER(DTS_ES_MODE_DTS,ULD_ID_DTS_ES);
				DelayMs(10); /*固件间切换时加个延时*/
				DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR);
				DelayMs(10); /*固件间切换时加个延时*/
				if(sys_flag.headphone_flag) 
					DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
				else
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);		
				}		
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/
			DSP_BOOT(configuration_unlock,change_to_new_config);	 
			break;
		case _DTS_96_24:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nDTS 96/24\n");
#endif			
			DSP_BOOT(configuration_lock,boot_idle);
			if(decode_mode==DTS_MODE_CES_ON)
				{
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_MODE_CES_ON\n");
#endif				
				Change_DSP_MATRIX_Code(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR);/*mode: 5chs to 7.1 chs,切换到CROSSBAR code*/
				DelayMs(300);/*在切完码后要延时下等DSP 稳定下来*/  
				Change_DSP_VIRTUALIZER_Code(0,ULD_ID_INVALID); /*把VIRTUALIZER Code切换为空码,不然会产生BUG*/					
				}
			else
				{
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_MODE_NORMAL\n");  
#endif					
				DSP_CFG_DECODER(DTS_ES_MODE_DTS9624,ULD_ID_DTS_ES);
				DelayMs(10); /*固件间切换时加个延时*/
				DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR);
				DelayMs(10); /*固件间切换时加个延时*/		
				if(sys_flag.headphone_flag)
					DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
				else
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
				}
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/
			DSP_BOOT(configuration_unlock,change_to_new_config);	 
			break;  
		case _DTS_ES_MATRIX:  
#ifdef _DEBUG_DSPDRV
			debug_printf("\nDTS-ES Matrix\n");
#endif			
			DSP_BOOT(configuration_lock,boot_idle);
			DSP_CFG_DECODER(DTS_ES_MODE_MATRIX,ULD_ID_DTS_ES);
			DelayMs(50); /*固件间切换时加个延时*/
		//	DSP_CFG_MATRIX(0,ULD_ID_NEO6); /*Matrix 由6.1变7.1有问题,故先用PL2X*/    
			DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*加入crossbar 使discrete 变成7.1*/
			DelayMs(50); /*固件间切换时加个延时*/  
			if(sys_flag.headphone_flag)
				DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
			else
				DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);  
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/
			DSP_BOOT(configuration_unlock,change_to_new_config);	 
			break;
		case _DTS_ES_DISCRETE:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nDTS-ES Discrete\n");  
#endif			
			DSP_BOOT(configuration_lock,boot_idle);
			DSP_CFG_DECODER(DTS_ES_MODE_DISCRETE,ULD_ID_DTS_ES); 
			DelayMs(10); /*固件间切换时加个延时*/ 
			DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*加入crossbar 使discrete 变成7.1*/
			DelayMs(10); /*固件间切换时加个延时*/
			if(sys_flag.headphone_flag)
				DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);  
			else
				DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/
			DSP_BOOT(configuration_unlock,change_to_new_config);	 
			break;      
		case _DTS_HD_MA_48K_2ch:
		case _DTS_HD_MA_48K_5ch:
			switch(decode_mode)
				{
				case DTS_MODE_PLIIX:
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_VIRTUALIZER(PL2X_B_MODE_MOVIE,ULD_ID_PL2X_B);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					DelayMs(300); /*切换完Code等待DSP 稳定*/
					DspChannelOut7_1(); /*打开7.1个通道*/
					break;
				case DTS_MODE_NORMAL:
				default:
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					DelayMs(300); /*切换完Code等待DSP 稳定*/
					DspChannelOut7_1(); /*打开7.1个通道*/
					break;
				}
			break;
		case _DTS_HD_MA_48K_6ch: /*DTS HD 48k 7.1不需要处理*/
		case _DTS_HD_MA_48K_7ch: /*DTS HD 48k 7.1不需要处理*/
			break;
		case _DTS_HD_MA_96K_2ch:
		case _DTS_HD_MA_96K_5ch:
			switch(decode_mode)
				{
				case DTS_MODE_PLIIX:
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_VIRTUALIZER(PL2X_B_MODE_MOVIE,ULD_ID_PL2X_B);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					DelayMs(300); /*切换完Code等待DSP 稳定*/
					DspChannelOut7_1(); /*打开7.1个通道*/
					break;
				case DTS_MODE_NORMAL:
				default:
					DSP_BOOT(configuration_lock,boot_idle); 
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					DelayMs(300); /*切换完Code等待DSP 稳定*/
					DspChannelOut7_1(); /*打开7.1个通道*/
					break;
				}
			break;  
		case _DTS_HD_MA_96K_7ch: /*DTS HD 96k 7.1不需要处理*/
			break;
		case _DTS_HD_MA_192K_2ch:
		case _DTS_HD_MA_192K_5ch:
			break;
		case _DTS_HD_MA_192K_7ch:
			break;
		}
}

/* 
 * @brief   分析DTS 流信息变化后进行相应的处理
 *
 * @author chenjianming
 * @date 2008-07-04
 * @refer  
 */
unsigned char Pre_DTS_Type=0; 
void DTS_Cfg_Change_Deal(unsigned char decode_mode)
{	
	if(dsp_power_flag==1)
		{
#ifdef _DEBUG_DSPDRV		
		debug_printf("\nPre_DTS_Type=%d",Pre_DTS_Type);
		debug_printf("\nsys_flag.decode_mode.dts_type=%d",sys_flag.decode_mode.dts_type);
#endif		
		if(Pre_DTS_Type==sys_flag.decode_mode.dts_type)
			return;
		
		switch(sys_flag.decode_mode.dts_type)
			{	
			case _DTS:
				DspDecodeDTS(decode_mode);
				break;
			case _DTS_96_24:
				DspDecodeDTS(decode_mode);
				break;
			case _DTS_ES_MATRIX: 
				DspDecodeDTS(decode_mode);
				DelayMs(300);
				DspChannelOut7_1(); /*打开7.1个通道*/				
				break;		
			case _DTS_ES_DISCRETE:  
				DspDecodeDTS(decode_mode);
				DelayMs(300);
				DspChannelOut7_1(); /*打开7.1个通道*/				
				break;
			}
			
		Pre_DTS_Type=sys_flag.decode_mode.dts_type;
		DelayMs(30);
		MsgSetDSPSetting(ZONE1_ID,0); /*加载DSPSETTING*/  
		}	
}  

/* 
 * @brief 分析DTS 信息后进行解码 
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
void AnalyseDtsCfg(_DTS_STREAM dts_stream)
{
	unsigned char dts_type=_DTS;
	unsigned char ext_audio_id,ext_audio;

	ext_audio_id = dts_stream.ext_audio_id;
	ext_audio=dts_stream.ext_audio & 0x1;

	switch(ext_audio_id)
		{
		case 0: /*XCh*/
			if(ext_audio)
				{
				dts_type = _DTS_ES_DISCRETE;
				break;
				}
			switch(dts_stream.pcmr)
				{
				case 0x01:
				case 0x03:
				case 0x05:
					dts_type = _DTS_ES_MATRIX;
					break;
				}
			break;
		case 2: /*96K*/
			switch(dts_stream.pcmr)
				{
				case 0x01:
				case 0x03:
				case 0x05:
					dts_type = _DTS_ES_MATRIX;
					break;
				case 0x06:
					dts_type = _DTS_96_24;
					break;
				}
			break;
		case 3: /*96K & XCh*/
			dts_type = _DTS_ES_DISCRETE;
			break;
		}

#ifdef _DEBUG_DSPDRV
	//debug_printf("AMODE:%X\n",dts_stream.amode);	
	//debug_printf("LFE:%X\n",dts_stream.lfeon);
	//debug_printf("PCMR:%X\n",dts_stream.pcmr);
	//debug_printf("EXT_AUDIO_ID:%X\n",dts_stream.ext_audio_id);
	//debug_printf("EXT_AUDIO:%X",dts_stream.ext_audio);
	//debug_printf("\ndts_type=%d",dts_type);
#endif

	sys_flag.decode_mode.dts_type = dts_type;
	MsgSetStream(stream_type,sys_flag.decode_mode.dts_type);
	DTS_Cfg_Change_Deal(sys_flag.decode_mode.dts_decode_mode);
}

/* 
 * @brief   分析DTS HD MA 流信息变化后进行相应的Code切换
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer 
 */
unsigned char Pre_DTS_HD_Type=0;
char DtsHD_Cfg_Change_Deal(unsigned char Dts_type)
{
#ifdef _DEBUG_DSPDRV
	debug_printf("\nDtsHD_Cfg_Change_Deal");
	debug_printf("\nDts_type=%d",Dts_type);
	debug_printf("\nPre_DTS_HD_Type=%d",Pre_DTS_HD_Type);
#endif

	if(dsp_power_flag==1)
		{
		if(Pre_DTS_HD_Type==Dts_type)
			return;
		
		DspDecodeDTS(sys_flag.decode_mode.dts_decode_mode);
		Pre_DTS_HD_Type=Dts_type;
		}
}

/*
 * @brief  读取DTS-HD固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0x9FC0HHHH
 * 		 Read Response Message =0x1FC0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer chenshiguang C11 code
 */
 unsigned char DspReadDTSHDChannel()
{
	short i=0;

	dsp_buf[i++]=0xA0;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x05;	dsp_buf[i++]=0x86;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}
/* 
 * @brief   分析DTS_HD_MA 信息后进行解码
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer 
 */
 void AnalyseDtsHDCfg(unsigned char *uCmdBuf)
{
	unsigned char dts_type;
	unsigned char DTS_HD_Channels;
	unsigned char DTS_HD_MA_Freq;
	
	uCmdBuf+=8;  
	DTS_HD_Channels=uCmdBuf[3]&0x0F;
#ifdef _DEBUG_DSPDRV	
	debug_printf("\nuCmdBuf[3]=%x",uCmdBuf[3]);
	debug_printf("\nDTS_HD_Channels=%d",DTS_HD_Channels);
#endif
	uCmdBuf+=4;  
	DTS_HD_MA_Freq=((uCmdBuf[1]>>4)&0x0F); 
#ifdef _DEBUG_DSPDRV		
	debug_printf("\nuCmdBuf[1]=%x",uCmdBuf[1]);
	debug_printf("\nDTS_HD_MA_Freq=%x",DTS_HD_MA_Freq);
#endif

	switch(DtsType)
		{
		case _DTS_HD_MA_48K:
			switch(DTS_HD_Channels)
				{
				case 2: 
					dts_type=_DTS_HD_MA_48K_2ch;
					break;
				case 6:
					dts_type=_DTS_HD_MA_48K_5ch;
					break;
				case 7 :
					dts_type=_DTS_HD_MA_48K_6ch; // jian add in 11-10-10
					break;
				case 8:
					dts_type=_DTS_HD_MA_48K_7ch;
					break;
				default:
					break;
				}
			break;
		case _DTS_HD_MA_96k_192K:
			 if(DTS_HD_MA_Freq==0x0E)//192k
			 {
				switch(DTS_HD_Channels)
				{
					case 2:
						dts_type=_DTS_HD_MA_192K_2ch;
						break;
					case 6:
						dts_type=_DTS_HD_MA_192K_5ch;
						break;
					case 8:
						dts_type=_DTS_HD_MA_192K_7ch;
						break;
					default :
						break;
				}			 
			 }
			 else //96K
			 {
				switch(DTS_HD_Channels)
				{
					case 2:
						dts_type=_DTS_HD_MA_96K_2ch;
						break;
					case 6:
						dts_type=_DTS_HD_MA_96K_5ch;
						break;
					case 8:
						dts_type=_DTS_HD_MA_96K_7ch;
						break;
					default :
						break;
				}			 
			 }
			break;
		default:
			break;
		}
	
	sys_flag.decode_mode.dts_type=dts_type;
	MsgSetStream(stream_type,DtsType);
	DtsHD_Cfg_Change_Deal(sys_flag.decode_mode.dts_type);
}


/* 
 * @brief   分析DTS_HD_HRA 信息后进行解码
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer 
 */
void AnalyseDtsHRACfg(unsigned char *uCmdBuf)
{
	unsigned char dts_type;
	unsigned char DTS_HD_HRA_Channels;

	uCmdBuf+=4;  
	DTS_HD_HRA_Channels = ((uCmdBuf[0]>>4)&0x0F); 
#ifdef _DEBUG_DSPDRV	
	debug_printf("\n__uCmdBuf[0]=%x",uCmdBuf[0]); 
	debug_printf("\nDTS_HD_HRA_Channels=%d",DTS_HD_HRA_Channels); 
#endif		

	switch(DtsType)
	{
		case _DTS_HD_HRA_48K:
			switch(DTS_HD_HRA_Channels)
			{
				case 2:
					dts_type=_DTS_HD_HRA_48K_2ch;
					break;
				case 6:
					dts_type=_DTS_HD_HRA_48K_6ch;
					break;
				case 7:
					dts_type=_DTS_HD_HRA_48K_7ch;
					break;
				case 8:
					dts_type=_DTS_HD_HRA_48K_8ch;
					break;
				default:
					break;
			}
			break;
		case _DTS_HD_HRA_96K:
			switch(DTS_HD_HRA_Channels)
			{
				case 2:
					dts_type=_DTS_HD_HRA_96K_2ch;
					break;
				case 6:
					dts_type=_DTS_HD_HRA_96K_6ch;
					break;
				case 7:
					dts_type=_DTS_HD_HRA_96K_7ch;
					break;
				case 8:
					dts_type=_DTS_HD_HRA_96K_8ch;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	sys_flag.decode_mode.dts_type=dts_type;
	MsgSetStream(stream_type,DtsType);
}

/* 
 * @brief   DTS-HD HRA 寄存器读取
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer 
 */
unsigned long DSP_DTS_HD_HRA_Read(unsigned char index)
 {
  	short i=0;

	dsp_buf[i++] =0x9F;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
 }

/* 
 * @brief DSP 自动检测消息处理
 *
 * @author chenjianming
 * @date 2007-10-18
 * @refer chenshiguang C11 code
 		 AN288 P23
 */


void lpcm_20_config(void)
{
	SetDspPower(1); /*DSP 切换到Spdif 口*/ 
	DigiSendByte(ADDR_ZONE1,0x00); /*数字板切换到HDMI 板上的Spdif 口*/
	DSP_Input_Stream_Select(AD_Input); /*输入到DSP源--A/D CS8416*/
}

void lpcm_20_config1(void)
{
	unsigned char temp_freq=0;
	stream_type=STREAM_PCM;
	PcmType=_PCM_2_1;

	temp_freq= Cs8416GetFreq();
	if(temp_freq!=0x0c) //0x0c-FREQ_192KHZ 不加载配置值
	{  	


	//	if(CD_Pure_Audio==0)
		{
			DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
			MsgSetDSPSetting(ZONE1_ID,0); /*加载DSPSETTING*/	
		}
	}
	else
	{
		SoftMute(0); 
	} 
	MsgSetStream(stream_type,0); 
	SoftMute(0);

}

char ProcAutodetecResponse(unsigned char *buf)
	
{
	unsigned long current_stream;
	unsigned char HDMI_Stream_Channel;
	unsigned char temp_freq=0;
	extern unsigned char  pcm20_delay_timer;

#ifdef _DEBUG_DSPDRV
	debug_printf("\nProcAutodetecResponse");
#endif

	current_stream=((buf[2]&0xFF)<<8) | (buf[3]&0xFF); /*得到当前流信息*/	

	/*解决高清+Pl2x功能next键不能保持Pl2x功能的问题,同样的next 操作后,高清流无法被DSP解,而普通流可以被DSP解*/
	if(!(buf[0] & DECODEABLE_STRM_FLAG))	/*This bit 0/1 = This stream is not/is decodable by the application (no need for new download if 1).*/
		{
		if((current_stream != SILENT_INPUT_DATA) && (current_stream != SILENT_DATA))/*If silent, don't need to reload*/
			{
			Pre_TureHD_Type=0;
			Pre_DTS_HD_Type=0;
			}
		}

	if(last_stream!=current_stream)
		{
		last_stream=current_stream;
		pcm20_delay_timer = 0;
		}
	else /*上次流信息跟这次的一样就不处理*/
		{	
#ifdef _DEBUG_DSPDRV
		debug_printf("\nThe same stream Information");      
#endif			
		MsgSetStream(stream_type,0); 
		return 0 ;
		}
	
#ifdef _DEBUG_DSPDRV
	debug_printf("\ncurrent_stream:%04X",current_stream);
#endif

	//if(IsCurFocusTid(AUTO_SOUND_ID) == FALSE) /*如果在IRC测试过程中，则不允许重新导码*/
	//	return;				

	if(buf[3]&NON_IEC_STFM_FLAG) /*bit5 = 1. Non IEC61937 compressed Stream*/
		{  
		switch(last_stream)
			{
			case SILENT_INPUT_DATA: /*Silent input data*/		
#ifdef _DEBUG_DSPDRV
				debug_printf("\nSilence");  
#endif
				stream_type=STREAM_NONE;  
				MsgSetStream(stream_type,0); 
				break;
			case SILENT_DATA: /*Silent input data*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nNo signal");  
#endif
				if(sys_flag.zone1_sub_channel!=3) //Analog Input 检测到的No signal 不处理
				{
					stream_type=STREAM_NONE;
					MsgSetStream(stream_type,0);
				}
				if(sys_flag.zone1_channel != INPUT_CH71) 
				{
					if(sys_flag.zone1_sub_channel!=3) //Analog Input 检测到的No signal 不处理
					{
						SoftMute(1);
					}
				}
				break;
			case DTS_FMT16_STREAM: /*DTS FORMAT-16 elementary stram*/
			case DTS_FMT14_STREAM: /*DTS FORMAT-14 elementary stram*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS FMT16 DTS FMT14\n");
#endif
				stream_type=STREAM_DTS;
				DtsType=_DTS;
				MsgSetStream(stream_type,DtsType); 
				MsgSetDSPSetting(ZONE1_ID,0); /*加载DSPSETTING*/
				break; 
			  case LINEAR_2_1_PCM_STREAM: /*0023*/	
				if(PreDspMode!=3) /*DSP切换到Analog 口反馈的LPCM2.0信息不处理*/
			  	{
#ifdef _DEBUG_DSPDRV  
					debug_printf("\nLINEAR_2_0_PCM_STREAM");
#endif							  			

				if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))				
					{ 					
					 pcm20_delay_timer = 200; // jain add in 10-08-06
                                         break;										 
					}
                                      //
					stream_type=STREAM_PCM;
					PcmType=_PCM_2_1;
					SetDspPower(1); /*DSP 切换到Spdif 口*/ 

					temp_freq= Cs8416GetFreq();
					//debug_printf("\ntemp_freq=%d",temp_freq); 
					if(temp_freq!=0x0c) //0x0c-FREQ_192KHZ 不加载配置值
					{  

//					if(CD_Pure_Audio==0)
						{
							DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
							MsgSetDSPSetting(ZONE1_ID,0); /*加载DSPSETTING*/	
						}
					}
					else
					{
						SoftMute(0); 
					} 
					MsgSetStream(stream_type,0); 
					SoftMute(0);
				}
				break;  
			case LINEAR_5_1_PCM_STREAM: /*0623*/
#ifdef _DEBUG_DSPDRV			  	
			  	debug_printf("\nLINEAR_5_1_PCM_STREAM");
#endif						
				HDMI_Stream_Channel=HdmiReadByte(5); /*读取当前LPCM通道数*/
#ifdef _DEBUG_DSPDRV			  	
				debug_printf("\nHDMI_Stream_Channel=%d",HDMI_Stream_Channel);
#endif	
				if(HDMI_Stream_Channel==1) //change by cjm 2009-7-10
				{
					stream_type=STREAM_PCM;
					PcmType=_PCM_5_1;
					SetDspPower(3); /*DSP 切换到Analog 口*/	
					DSP_Input_Stream_Select(HDMI_Input); /*输入到DSP源--HDMI(I2S)--放到DSP引导起来后,否则DSP处于忙状态*/
					temp_freq= HdmiReadByte(3); 
					//debug_printf("\ntemp_freq=%x",temp_freq);
					if(temp_freq!=0x0e) //不是192K时
					{
						MsgSetDSPSetting(ZONE1_ID,0); /*加载DSPSETTING*/	
						MsgSetStream(stream_type,0);  
					}
					DelayMs(50); /*在切完码后要延时下等DSP 稳定下来*/  
					DspChannelOut7_1(); /*打开7.1个通道*/
				}
				break;
			case LINEAR_7_1_PCM_STREAM: /*0823*/
#ifdef _DEBUG_DSPDRV			 
			  	debug_printf("\nLINEAR_7_1_PCM_STREAM\n");  
#endif						
				HDMI_Stream_Channel=HdmiReadByte(5); /*读取当前LPCM通道数*/
#ifdef _DEBUG_DSPDRV			 
				debug_printf("\nHDMI_Stream_Channel=%d",HDMI_Stream_Channel);
#endif					
				if(HDMI_Stream_Channel==1) //change by cjm 2009-7-10
				{
					stream_type=STREAM_PCM; 
					PcmType=_PCM_7_1;  	
					SetDspPower(3); /*DSP 切换到Analog 口*/	  
					DSP_Input_Stream_Select(HDMI_Input); /*输入到DSP源--HDMI(I2S)--放到DSP引导起来后,否则DSP处于忙状态*/	
					MsgSetStream(stream_type,0); 
					temp_freq= HdmiReadByte(3); 
					if(temp_freq!=0x0e)
					{
						MsgSetDSPSetting(ZONE1_ID,0); /*加载DSPSETTING*/	
					}
					else
					{
						SoftMute(0);
					}
				} 
				break;
			  case HDCD_PCM_Detect_STREAM: /*不要求处理HDCD*/
			  case HDCD_PCM_Lost_STREAM:
#ifdef _DEBUG_DSPDRV
				debug_printf("\ndecode HDCD");
#endif
			  	break;
			  default:
			  	break;
			}
		}
	else
		{
		switch(last_stream)
			{
			case NEVER_REPORTED:
#ifdef _DEBUG_DSPDRV
				debug_printf("\nNEVER_REPORTED");
#endif				
				break;
			case AC_3_DATA: /*AC-3 stream*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nAC3 stream");
#endif
				stream_type=STREAM_AC3; 
				DolbyType=_DOLBY_5CH;
				SetDspPower(1); /*DSP 切换到spdif 口*/ 	  
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				
				{ 
					DigiSendByte(ADDR_ZONE1,0X00); /*数字板切换到HDMI 板上的Spdif 口*/
					DSP_Input_Stream_Select(AD_Input); /*输入到DSP源--A/D CS8416*/
				}
				MsgSetStream(stream_type,DolbyType);
				softmute_flag=1;
				break;
			case MPEG1_LAYER1_DATA:		/*MPEG -1 layer 1 data*/
			case MPEG1_LAYER2_3_DATA:	/*MPEG -1 layer 2 or 3 data or MPEG-2 without extension*/
			case MPEG2_DATA:			/*MPEG -2 data with extension*/
			case MPEG2_AAC_ADTS_DATA:  /*MPEG -2 AAC ADTS data*/
			case MPEG2_LAYER1_DATA:		/*MPEG -1 layer 1 data Low sampling frequency*/
			case MPEG2_LAYER2_3_DATA:	/*MPEG -1 layer 2 or 3 data Low sampling frequency*/
				break;
			case DTS_1_DATA:			/*DTS-1 data (512-sample bursts))*/
			case DTS_2_DATA:			/*DTS-2 data (1024-sample bursts))*/
			case DTS_3_DATA:			/*DTS-3 data (2014-sample bursts))*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_1 DTS_2 DTS_3");
#endif
				stream_type=STREAM_DTS;
				DtsType=_DTS;
				SetDspPower(1); /*DSP 切换到spdif 口*/ 	
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{ 
					DigiSendByte(ADDR_ZONE1,0X00); /*数字板切换到HDMI 板上的Spdif 口*/
					DSP_Input_Stream_Select(AD_Input); /*输入到DSP源--A/D CS8416*/
				}		
				MsgSetStream(stream_type,sys_flag.decode_mode.dts_type); 
				softmute_flag=1; 
				break;
			case MPEG2_AAC_ADTS:  
				break;
			case DOLBY_DIGITAL_PLUS:
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDOLBY DIGITAL PLUS");
#endif				
				stream_type=STREAM_AC3;
				DolbyType=_DOLBY_PLUS;
				sys_flag.decode_mode.dolby_type=DolbyType;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))				{
				
					SetDspPower(4); /*DSP 切换到HDMI 口*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}				
				MsgSetStream(stream_type,DolbyType); 
				MsgSetDSPSetting(ZONE1_ID,1); /*加载DSPSETTING*/
				//DelayMs(300); 
				//DSP_AC3_DRC_INIT(); 
				break;
			case True_HD:
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDolby True HD");
#endif			
				stream_type=STREAM_AC3; 
				DolbyType=_DOLBY_TRUE_HD; //change by cjm 2009-2-9
				sys_flag.decode_mode.dolby_type=DolbyType;	
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP 切换到HDMI 口*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}	
				MsgSetStream(stream_type,DolbyType); 
				MsgSetDSPSetting(ZONE1_ID,1); /*加载DSPSETTING*/
				break;			
			case DTS_HD_MA_Stream_48k: /*0x0411 --DTS-HD 48K*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS HD 48K");
#endif				    
				stream_type=STREAM_DTS;
				DtsType=_DTS_HD_MA_48K;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP 切换到HDMI 口*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}	
				MsgSetStream(stream_type,DtsType); 
				MsgSetDSPSetting(ZONE1_ID,1); /*加载DSPSETTING*/ 
				break;							
			case DTS_HD_MA_Stream_96k_192K: /*0x04411 --DTS-HD 96k*/									
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS HD 96K/192K");
#endif
				stream_type=STREAM_DTS;    
				DtsType=_DTS_HD_MA_96k_192K;    
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP 切换到HDMI 口*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}	
				MsgSetStream(stream_type,DtsType); 
				MsgSetDSPSetting(ZONE1_ID,1); /*加载DSPSETTING*/  
				break;			
			case DTS_HD_HRA_Stream_48K: /*0x0211 --DTS-HD HRA 48K*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS HD HRA 48K"); 
#endif				
				stream_type=STREAM_DTS;
				DtsType=_DTS_HD_HRA_48K;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP 切换到HDMI 口*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}	
				MsgSetDSPSetting(ZONE1_ID,1); /*加载DSPSETTING*/
				break;
			case DTS_HD_HRA_Stream_96k:/*0x4211--DTS-HD HRA 96K*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS HD HRA 96K"); 
#endif							
				stream_type=STREAM_DTS;
				DtsType=_DTS_HD_HRA_96K;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP 切换到HDMI 口*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}
				MsgSetDSPSetting(ZONE1_ID,1); /*加载DSPSETTING*/
				break;
			default: 
				break;			
			}
		}

#if 0
	if(sys_flag.headphone_flag == 1) /*如果耳麦接上,且是TrueHD需要设置DH2 Heap*/
		{
		if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD))
			{
#ifdef _DEBUG_DSPDRV			
			debug_printf("\nDH2 Mode -- TRUE HD");
#endif
			DH2_HEAP_MODE(1); /*Mode 1 only for Dolby TRUE HD*/
			}
		else  
			{
#ifdef _DEBUG_DSPDRV						
			debug_printf("\nDH2 Mode -- Other stream");
#endif			
			DH2_HEAP_MODE(1); /*Mode 0 for other Stream*/  
			}
		}	
#endif	  
}

/* 
 * @brief DSP 返回消息处理 
 *
 * @author chenjianming
 * @date 2007-10-18
 * @refer chenshiguang C11 code
 */
void DspProcUnsolicitedMsg(unsigned char *uCmdBuf)
{
	switch(uCmdBuf[3])
		{
		case AUTODETECT_RESPONSE:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nAutoDetect Response");
#endif
			ProcAutodetecResponse(uCmdBuf+4);
			break;
		case PLL_OUT_OF_CLK:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nPLL OUT OF lock");
#endif
			break;
		default:
			break;
		}
}

/*
 * @brief Check DSP Return Message
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang Cll code
 */
char CheckDspMessage(unsigned long message_value)
{
	signed long dwMsg;
	unsigned char ret;
	unsigned char k; 	

	ret=DspSpiReadNByte(dsp_buf,8); /*读DSP 返回消息*/
	ArrayToDWORD(&dwMsg, dsp_buf); /*把数组型的消息转换为Word 型*/
	
#ifdef _DEBUG_DSPDRV
	debug_printf("\nCheck Message Return\n");
	debug_printf("%02X\n",ret);
	for(k=0;k<8;k++)
	    debug_printf("%02X ",dsp_buf[k]);	 
	debug_printf("\n");
#endif  

	if(dwMsg != message_value)
		return FAILURE;

	return SUCCESS;
}

/*
 * @brief 向DSP ULD 固件写值
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer 
 */
void DSPFirmwareWrite(unsigned char wop,unsigned short index,signed long value)
{
	short i=0;
	
	dsp_buf[i++] = wop;					dsp_buf[i++] = 0x00;	
	dsp_buf[i++] = (index >> 8) & 0xFF;	dsp_buf[i++] = index & 0xFF;
	
	dsp_buf[i++] = (value >> 24) & 0xFF;	dsp_buf[i++] = (value >> 16) & 0xFF;	
	dsp_buf[i++] = (value >> 8) & 0xFF;	dsp_buf[i++] = value & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief 读取DSP Response Message
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
unsigned long ReadDsp(void)
{
	unsigned long rtn_value=0;
	unsigned char time_out_cnter=200;
	
#ifdef _DEBUG_DSPDRV
	unsigned char k;
#endif
	
	do{
		if(!SenseDspIrq())
			break;
		DelayMs(1);
		}while(--time_out_cnter); /*判断DSP_IRQ 是否为低*/

	if(time_out_cnter)
		{
		DspSpiReadNByte(dsp_buf,8);  /*读取返回消息并放到dsp_buf*/		
		
#ifdef _DEBUG_DSPDRV
		debug_printf("\n");
		for(k=0;k<8;k++)
	    		debug_printf("%02X ",dsp_buf[k]);	 
		debug_printf("\n");
#endif

		ArrayToHWORD(&rtn_value,dsp_buf+4); /*把返回的data value 转换成字*/
		}
	else
		{
		rtn_value = 0xFFFFFFFF;
#ifdef _DEBUG_DSPDRV
		debug_printf("ReadDspc Time out\n");
#endif		
		}

	DelayMs(1);
	return rtn_value;
}

/* 
 * @brief 返回当前采样率频率
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang C11 code
 */
unsigned char GetNormalFreq(void)
{
	return CurrentSampleFreq;
}

/*
 * @brief 遥控器上Late 按键功能实现 
 *
 * @author chenjianming
 * @date 2007-12-13
 * @refer chenshiguang C11 code
 */
void SetLate(unsigned char stream,unsigned char mode)
{
	unsigned long set_value;
	
	switch(mode)
		{
		case 0:	/*off*/
			set_value = 0x00;
			break;
		case 1:	/*half*/
			set_value = 0x3FFFFFFF;
			break;
		case 2:	/*full*/
			set_value = 0x7FFFFFFF;
			break;
		}

	if(dsp_power_flag) 
		{
		switch(stream)
			{
			case _DOLBY_2CH:
			case _DOLBY_5CH:
			case _DOLBY_EX:
			case _DOLBY_PLUS:
				DSP_AC3_COMPRESSION_CONTROL();/*Compression mode=Line out mode*/
				DSP_AC3_CUT_X(set_value);
				DSP_AC3_BOOST_Y(set_value);				
				break;
			case _DOLBY_TRUE_HD:
			case _DOLBY_TRUE_HD_2ch:
			case _DOLBY_TRUE_HD_5ch:
			case _DOLBY_TRUE_HD_6ch:
			case _DOLBY_TRUE_HD_7ch:
				DSP_TRUEHD_DRC_Control(); /*Force dynamic range control on*/
				DSP_TRUEHD_Cut_Scalefactor(set_value);
				DSP_TRUEHD_Boost_Scalefactor(set_value);
				break;
			default:
				break;
			}
		}
}

/*
 * @brief Master Boot Program
 *
 * @author chenjianming
 * @date 2008-06-17
 * @refer 49700x_System_Designers_Guide p23
 */
uchar  dsp_repower_flag = 0; // jian add in 10-10-13
char DspMasterBoot(void)
{
#ifdef _DEBUG_DSPDRV
	debug_printf("\nDsp Master Boot");
#endif

	DSPHwReset(0);	/*Hardware reset*/
	do{
		if(!SenseDspIrq())
			break;
#ifdef _DEBUG_DSPDRV		
		else
			debug_printf("\nDSP IRQ is Still hight)");
#endif		
		}while(1); /*等待IRQ置低*/

	if(CheckDspMessage(FLASH_VERIFIED) == FAILURE) /*FLASH_IMAGE_VERIFIED=0xEF000020 00000001*/
		{
#ifdef _DEBUG_DSPDRV
		debug_printf("\ncheck FLASH IMAGE VERIFIED error\n");
#endif
		return FAILURE;
		}	

	DelayMs(10);
	dsp_repower_flag = 1;
	return SUCCESS;
}

