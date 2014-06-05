/* @file 		DSP_API.c
 * @brief 		DSP API File
 *
 * @author 	chenjianming
 * @date 	  	2008-06-13
 * @version 	1.0
 */
#include "DSP_API.h"
#include "Dspdrv.h"
#include "..\main\debug.h"
#include "..\main\task.h"
#include "..\driver\Dsp_Spi.h"

#ifdef _DEBUG
#define _DEBUG_API
#endif

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

 /*
 * @brief  DSP Boot Activity
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P93
 */
void DSP_BOOT(unsigned char config_lock,unsigned char Boot_Action)
{
	short i=0;
	unsigned char lock_value=0;
	unsigned char boot_value=0;

	if(config_lock)
		lock_value=0x01;
	else
		lock_value=0x00;

	switch(Boot_Action)
		{
		case boot_idle:
			boot_value=0x00;
			break;
		case change_to_new_config:
			boot_value=0x01;
			break;
		case save_for_power_down:
			boot_value=0x02;
			break;
		case shut_down:
			boot_value=0x08;
			break;
		case save_para:
			boot_value=0x04;
			break;
		}

	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =lock_value;	dsp_buf[i++] =boot_value;

	DspSpiWriteNByte(dsp_buf,i);	
}

  /*
 * @brief  DSP Config output FS
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P94
 */
 void DSP_CFG_OUTPUT_FS(unsigned char Freq)
  {
   	short i=0;
	unsigned char index_value;

	switch(Freq)
		{
		case FS_48: 		index_value=0x00; 	break;		
		case FS_44_1: 	index_value=0x01;		break;	
		case FS_32:		index_value=0x02;		break;
		case FS_96:		index_value=0x04;		break;	
		case FS_88_2:	index_value=0x05;		break;
		case FS_64:		index_value=0x06;		break;
		case FS_24:		index_value=0x08;		break;
		case FS_22_05:	index_value=0x09;		break;
		case FS_16:		index_value=0x0A;		break;
		case FS_192:		index_value=0x0C;		break;
		case FS_176_4:	index_value=0x0D;		break;
		case FS_128:		index_value=0x0E;		break;			
		}

	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x05;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;

	DspSpiWriteNByte(dsp_buf,i);	
  }

 /*
 * @brief  DSP Input Audio Source Select
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P94
 */
void DSP_CFG_AUDIO_SRC(unsigned char Input_Source)
 {
 	short i=0;
	unsigned char Source=0;

	switch(Input_Source)
		{
		case Spdif_AutoSwitch:
			Source=0x01;
			break;
		case HDMI_AutoSwitch:
			Source=0x02;
			break;
		case Multi_Channel_Analog:
			Source=0x03;
			break;
		case DSD:
			Source=0x04;
			break;
		case Spdif_No_AutoSwitch:
			Source=0x05; 
			break;
		case HDMI_No_Autoswitch:
			Source=0x06;
			break;
		case Flash_Update:
			Source=0x07;  
			break;
		}
	
	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x06;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =Source;
	
	DspSpiWriteNByte(dsp_buf,i);	
 }

/*
 * @brief  DSP Decoder Config
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P94
 */
 void DSP_CFG_DECODER(unsigned char mode,unsigned char decoder_type)
 {
 	 short i=0;
	 
	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x07;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =mode;	dsp_buf[i++] =0x00;	dsp_buf[i++] =decoder_type;

	DspSpiWriteNByte(dsp_buf,i);	
 }

 /*
 * @brief  DSP Matrix Config
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P95
 */
 void DSP_CFG_MATRIX(unsigned char mode,unsigned char matrix_type)
 {
 	 short i=0;
	 
	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x08;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =mode;	dsp_buf[i++] =0x00;	dsp_buf[i++] =matrix_type;

	DspSpiWriteNByte(dsp_buf,i);	
 }

/*
 * @brief  DSP Virtualizer Config
 *
 * @author chenjianming  
 * @date 2008-06-13
 * @refer System Designers Guide P95
 */
 void DSP_CFG_VIRTUALIZER(unsigned char mode,unsigned char virtualizer_type)
 {
 	 short i=0;

	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x09;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =mode;	dsp_buf[i++] =0x00;	dsp_buf[i++] =virtualizer_type;

	DspSpiWriteNByte(dsp_buf,i);	
 }

/*
 * @brief  DSP PPM Change
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P95
 */
 void DSP_CFG_PPM(unsigned char PPM_type)
 {
 	 short i=0;

	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x0A;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =PPM_type;

	DspSpiWriteNByte(dsp_buf,i);	
 }

/*
 * @brief  DSP PPM Mode Change
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P95
 */
 void DSP_CFG_PPM_Mode(unsigned char Module2_mode,unsigned char Module1_mode)
 {
 	 short i=0;
	 
	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x0B;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =Module2_mode;	dsp_buf[i++] =0x00;	dsp_buf[i++] =Module1_mode;

	DspSpiWriteNByte(dsp_buf,i);	
 }

 void DSP_CFG_PPM_Mode_Pl2x(void)
 {
 	 short i=0;
	 
	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x0B;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x0a;

	DspSpiWriteNByte(dsp_buf,i);	
 }

 /*
 * @brief  DSP Config MCLK Factor
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P94
 */
void DSP_CFG_MCLK_FACTOR(unsigned char Freq)
 {
 	short i=0;
	unsigned long Freq_Value=0;

	switch(Freq)
		{
			case Fs_128:
				Freq_Value=0x0080;
				break;
			case Fs_256:
				Freq_Value=0x0100;
				break;
			case Fs_512:
				Freq_Value=0x0200;
				break;
		}
	
	dsp_buf[i++] =Dsp_API_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x13;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =(Freq_Value>>8 & 0xFF);	dsp_buf[i++] =Freq_Value & 0xFF;	

	DspSpiWriteNByte(dsp_buf,i);	
 }

/*
 * @brief  DSP Decoder code change
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer 
 */
 void Change_DSP_Decoder_Code(unsigned char mode,unsigned char Decode_type)
{
 	DSP_BOOT(configuration_lock,boot_idle);
	switch(Decode_type)
		{
		case ULD_ID_SGEN:
#ifdef _DEBUG_API
			debug_printf("\nChange DSP Decoder To SGEN Module");
#endif				
			DSP_CFG_AUDIO_SRC(Spdif_No_AutoSwitch); /*切换输入口*/
			DSP_CFG_DECODER(mode,ULD_ID_SGEN);  
			DSP_CFG_MCLK_FACTOR(Fs_256); /*设置时钟频率*/
			break;
		case ULD_ID_DDPLUS:
			DSP_CFG_DECODER(DDPLUS_MODE_DEFAULT,ULD_ID_DDPLUS);  
			break;
		case ULD_ID_TRUEHD:
			DSP_CFG_DECODER(TRUEHD_MODE_DEFAULT,ULD_ID_TRUEHD);  
			break;
		case ULD_ID_DTS_MA_48:
			DSP_CFG_DECODER(DTS_MA_48_MODE_DEFAULT,ULD_ID_DTS_MA_48);  
			break;
		case ULD_ID_DTS_MA_96:
			DSP_CFG_DECODER(DTS_MA_96_MODE_DEFAULT,ULD_ID_DTS_MA_96);  
			break;
		case ULD_ID_DTS_HRA:
			DSP_CFG_DECODER(DTS_HRA_MODE_DEFAULT,ULD_ID_DTS_HRA);  
			break;
		case ULD_ID_DTS_LBR:
			DSP_CFG_DECODER(DTS_LBR_MODE_DEFAULT,ULD_ID_DTS_LBR);  
			break;
		default:
			break;
		}  
	DSP_BOOT(configuration_unlock,change_to_new_config);	
}

 /*
 * @brief  DSP MATRIX code change
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer 
 */
void Change_DSP_MATRIX_Code(unsigned char mode,unsigned char matrix_type)
{
 	DSP_BOOT(configuration_lock,boot_idle);
	switch(matrix_type)
		{
		case ULD_ID_PL2X:
#ifdef _DEBUG_API
			debug_printf("\nchange matrix code to PL2X");
#endif
			switch(mode)
				{
				case PL2X_MODE_MOVIE:
#ifdef _DEBUG_API
					debug_printf("\nPL2X_MODE_MOVIE");  
#endif					
					DSP_CFG_MATRIX(PL2X_MODE_MOVIE,ULD_ID_PL2X);
				//	if(sys_flag.zone1_sub_channel==3) /*Analog Input */
				//		DSP_CFG_PPM_Mode(2,0); /*Analog Iput +PL2X 时,把内部通道设置为2通道*/
					break;
				case PL2X_MODE_MUSIC:
#ifdef _DEBUG_API
					debug_printf("\nPL2X_MODE_MUSIC");
#endif					
					DSP_CFG_MATRIX(PL2X_MODE_MUSIC,ULD_ID_PL2X); 
				//	if(sys_flag.zone1_sub_channel==3) /*Analog Input */
				//		DSP_CFG_PPM_Mode(2,0);/*Analog Iput +PL2X 时,把内部通道设置为2通道*/
					break;		
				case PL2X_MODE_DD_EX:  
#ifdef _DEBUG_API
					debug_printf("\nPL2X_MODE_DD_EX");
#endif					
					DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X);
				//	if(sys_flag.zone1_sub_channel==3) /*Analog Input */
				//		DSP_CFG_PPM_Mode(2,0); /*Analog Iput +PL2X 时,把内部通道设置为2通道*/
					break;
				}
			break;
		case ULD_ID_NEO6:
#ifdef _DEBUG_API
			debug_printf("\nchange matrix code to NEO6");
#endif			
			switch(mode)
				{
				case 0: /*no use any mode*/
					DSP_CFG_MATRIX(0,ULD_ID_NEO6);
					break; 
				case NEO6_MODE_CINEMA:
#ifdef _DEBUG_API
					debug_printf("\nNEO6_MODE_CINEMA");
#endif						
					DSP_CFG_MATRIX(NEO6_MODE_CINEMA,ULD_ID_NEO6);
					break;
				case NEO6_MODE_MUSIC:
#ifdef _DEBUG_API
					debug_printf("\nNEO6_MODE_MUSIC");
#endif							
					DSP_CFG_MATRIX(NEO6_MODE_MUSIC,ULD_ID_NEO6);
					break;
				case NEO6_MODE_MATRIX:
#ifdef _DEBUG_API
					debug_printf("\nNEO6_MODE_MATRIX");
#endif												
					DSP_CFG_MATRIX(NEO6_MODE_MATRIX,ULD_ID_NEO6);
					break;
				}
			break;
		case ULD_ID_CROSSBAR:
#ifdef _DEBUG_API
			debug_printf("\nchange matrix code to CROSSBAR:");
#endif			
			switch(mode)
				{
				case 0: /*只是把Matrix 切换为CROSSBAR,不做其它设置*/
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
					break;
				case CROSSBAR_MODE_COEF_L_R_7CH:
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_R_7CH,ULD_ID_CROSSBAR);
					break;
				case CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH:
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); 					
					break;
				}
			break; 
		}
	DSP_BOOT(configuration_unlock,change_to_new_config);	
}

/*
 * @brief  DSP VIRTUALIZER code change
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer 
 */
void Change_DSP_VIRTUALIZER_Code(unsigned char mode,unsigned char virtualizer_type)
{
 	DSP_BOOT(configuration_lock,boot_idle);
	switch(virtualizer_type)
		{
		case ULD_ID_CROSSBAR_B:
#ifdef _DEBUG_API	
			debug_printf("\n___HDMI Load CROSSBAR_B");
#endif					
			DSP_CFG_VIRTUALIZER(0,ULD_ID_CROSSBAR_B);  
			break;
		case ULD_ID_DH2:
			DSP_CFG_VIRTUALIZER(mode,ULD_ID_DH2);
			break;			
		case ULD_ID_IRC: /*Syetem Designer P116*/
			DSP_CFG_AUDIO_SRC(Multi_Channel_Analog); /*切换到多通道模拟口*/
			DSP_CFG_MCLK_FACTOR(Fs_512); /*FS 512K*/    		
			DSP_CFG_DECODER(0,0); /*Set to PCM decoder in OS  0-not use any mode */			
			DSP_CFG_VIRTUALIZER(mode,ULD_ID_IRC); /*加载IRC Code*/
			DSP_CFG_PPM(ULD_ID_APP); /*加载APP*/      
			break;
		case ULD_ID_DVS2:
			//DSP_CFG_VIRTUALIZER();
			break;
		case ULD_ID_PL2X_B:
			//DSP_CFG_VIRTUALIZER();
			break;
		case ULD_ID_INVALID:
			DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
			break;
		}
	DSP_BOOT(configuration_unlock,change_to_new_config);	
}

/*
 * @brief  DSP PPM code change
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer System Designers Guide P94
 */
 void Change_DSP_PPM_Code(unsigned char PPM_type)
{
 	DSP_BOOT(configuration_lock,boot_idle);
	switch(PPM_type)
		{
		case ULD_ID_SPP:
			DSP_CFG_PPM(ULD_ID_SPP);
			break;
		case ULD_ID_APP:
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/
			break;
		}	
	DSP_BOOT(configuration_unlock,change_to_new_config);	
}

/*
 * @brief  DSP API 应用函数
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer 
 */
extern unsigned char PcmType;
extern void Zone1Mute(uchar mute);
void Change_Input_Audio_Source(unsigned char Source)
{
	DSP_BOOT(configuration_lock,boot_idle);
	switch(Source)
		{
			case 0:
#ifdef _DEBUG_API
				debug_printf("\nSpdif_AutoSwitch");
#endif
				DSP_CFG_AUDIO_SRC(Spdif_AutoSwitch);
				DSP_CFG_MCLK_FACTOR(Fs_256);		
				if(sys_flag.headphone_flag == 1)  
					{
					DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
					Zone1Mute(1); 
					}
				DSP_CFG_PPM(ULD_ID_APP); /*加载APP固件*/
				DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/	
				break;
			case 1: /*高清时APP固件功能有些不支持默认用SPP固件*/  
#ifdef _DEBUG_API				
				debug_printf("\nHDMI_AutoSwitch");
#endif
				DSP_CFG_AUDIO_SRC(HDMI_AutoSwitch);  
				DSP_CFG_MCLK_FACTOR(Fs_512); /*HDMI 高清流用FS 512*/		
				break;
			case 2:  
#ifdef _DEBUG_API				
				debug_printf("\nMulti_Channel_Analog");  
#endif				
				DSP_CFG_AUDIO_SRC(Multi_Channel_Analog);
				DSP_CFG_DECODER(0,0); /*Set to PCM decoder in OS  0-not use any mode */		
				DSP_CFG_MCLK_FACTOR(Fs_256); /*普通流用FS 256*/
				switch(PcmType)
					{
					case _PCM_2_1:
						switch(sys_flag.decode_mode.pcm_decode_mode)
							{
							case PCM_MODE_PL2:
								DSP_CFG_MATRIX(PL2X_MODE_MUSIC,ULD_ID_PL2X);
								DSP_CFG_PPM_Mode(2,0); /*Analog Iput +PL2X 时,把内部通道设置为2通道*/
								break;
							case PCM_MODE_PL2X:
								DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X);
								DSP_CFG_PPM_Mode(2,0); /*Analog Iput +PL2X 时,把内部通道设置为2通道*/
								break;
							case PCM_MODE_NEO6:
								DSP_CFG_MATRIX(PL2X_MODE_MUSIC,ULD_ID_PL2X); /*由于NEO6有问题,故用PL2X代替*/
								DSP_CFG_PPM_Mode(2,0); /*Analog Iput +PL2X 时,把内部通道设置为2通道*/
								break;
							case PCM_MODE_CES:
								DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); 
								break;
							case PCM_MODE_NORMAL:
							default:
								DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
								DelayMs(30);/*在切完码后要延时下等DSP 稳定下来*/  
								DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
								DelayMs(30);
								DSP_CFG_PPM(ULD_ID_APP);  /*加载APP固件*/
								DelayMs(10);
								DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/	
								break;
							}
						break;
					case _PCM_5_1:
						switch(sys_flag.decode_mode.pcm_decode_mode)
							{
							case PCM_MODE_PL2X:
								DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X);
								break;
							case PCM_MODE_NEO6:
								DSP_CFG_MATRIX(PL2X_MODE_MUSIC,ULD_ID_PL2X); /*由于NEO6有问题,故用PL2X代替*/
								break;
							case PCM_MODE_CES:
								DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); 
								break;
							case PCM_MODE_NORMAL:
							default:
								DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
								DelayMs(30);/*在切完码后要延时下等DSP 稳定下来*/  
								DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
								break;
							}
						DSP_CFG_PPM(ULD_ID_APP);  /*加载APP固件*/
						DSP_CFG_PPM_Mode(1,0); /*加载配置文件设置EQ mode为True Parametric*/	
						break;
					default:
						break;
					}
				break;
			case 3: //No use 
				DSP_CFG_AUDIO_SRC(DSD);
				break;
			case 4: //No use
				DSP_CFG_AUDIO_SRC(Spdif_No_AutoSwitch);
				break;
			case 5: //No use
				DSP_CFG_AUDIO_SRC(HDMI_No_Autoswitch);
				break;  
		}
	DSP_BOOT(configuration_unlock,change_to_new_config);
}

/*
 * @brief  DSP flash update commond
 *
 * @author chenjianming
 * @date 2008-10-10
 * @refer 
 */
 void DSP_Update_Flash(void)
{
	DSP_BOOT(configuration_lock,boot_idle);
	DSP_CFG_AUDIO_SRC(Flash_Update);
	DSP_CFG_MCLK_FACTOR(Fs_256);
	DSP_BOOT(configuration_unlock,change_to_new_config);
}

/*
 * @brief  DSP API Read command
 *
 * @author chenjianming
 * @date 2008-06-13
 * @refer 
 */
 unsigned long DSP_API_Read(unsigned char index)
 {
  	short i=0;

	dsp_buf[i++] =0xEF;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
 }

