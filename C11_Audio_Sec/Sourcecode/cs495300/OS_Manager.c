/*@file   	   OS_Manager.c
* @brief     OS Firmware Module Manager 
* @author  chenjianming
* @date     2007-09-05
* @refer    AN288 Datasheet P15
*
* 		  Write Command=0x8100HHHH 0xhhhhhhhh
* 		  Read Request Command=0x81C0HHHH
* 		  Read Response Message=0x01C0HHHH 0xhhhhhhhh
* 		  0xHHHH=Index	0xhhhhhhhh=Data Vaule
*/
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "OS_Manager.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

#if 0
/*
 * @brief  控制DSP 通道切换(Digital\Analog 2ch \Analog Multi ch ) 
 *
 * @author chenjianming
 * @date 2007-09-05
 * @refer AN288 Datasheet P15
 */
void OS_IO_CONFIG_Control(unsigned char channel)
{
	if(channel==Analog_Input_Multi_ch) /*模拟多通道*/
		DSPFirmwareWrite(OS_A_B_Wop,0x0001,0X00000001); //Multi channel input
	else 							/*模拟2 通道\数字输入*/
		DSPFirmwareWrite(OS_A_B_Wop,0x0001,0X00000002); //default (2 channel input)
}
#endif

/* 
 * @brief  用于设置输出模式
 *
 * @author  chenjianming
 * @date 	   2007-09-05
 * @refer    AN288 Datasheet P16
 * @note     For enabling and configuring DualZone output,please refer to 
 *		    to the Crossbar Manager documented in AN246MPC
 */
void OS_OUTPUT_MODE_CONTROL(unsigned char Karaoke,unsigned char dual_mode,unsigned char output_mode)
{
	short i=0;
	unsigned char reg_value1=0,reg_value2=0;

	if(Karaoke)
		reg_value1=0x01;
	else
		reg_value1=0x00;

	reg_value2 |= (dual_mode & 0x3) << 4;
	reg_value2 |= output_mode & 0xF;
		
	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value1;	dsp_buf[i++] =reg_value2;

	DspSpiWriteNByte(dsp_buf,i);
}

#if 0
/* 
 * @brief 设置输入流采样率 
 *
 * @author chenjianming
 * @date 2007-09-05
 * @refer AN288 Datasheet P16
 */
void OS_SAMPLE_RATE_CONTROL(unsigned char rate)
{
	short i=0;
	unsigned char reg_value=0;
	
	reg_value|=rate & 0x0F;
	
	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x03;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}
#endif

#if 0
/* 
 * @brief OS Auto Detect Config
 *
 * @author chenjianming
 * @date 2007-09-06
 * @refer AN288 Datasheet P17
 */
void OS_AUTODETECT_Config(unsigned char ACCN,unsigned char bypass,unsigned char autodetection)
{
	short i=0;
	unsigned char reg_value=0;
	
	if(ACCN)
		reg_value|=0x04;
	if(bypass)
		reg_value|=0x02;
	if(autodetection)
		reg_value|=0x01;

	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x05;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}
#endif

#if 0
/*
 * @brief OS PCM Auto Switch Enable
 *
 * @author chenjianming
 * @date 2007-09-06
 * @refer  AN288 Datasheet P17
 */
void OS_PCM_AUTOSWITCH_ENABLE(unsigned char enable)
{
	short i=0;
	unsigned char reg_value=0;

	if(enable)
		reg_value=0x01;
	else
		reg_value=0x00;

	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x06;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}
#endif

/* 
 * @brief output mode control for DSPB
 *
 * @author chenjianming
 * @date 2007-09-06
 * @refer AN288 Datasheet P18
 */
void OS_OUTPUT_MODE_CONTROL_FOR_DSPB(unsigned char Karaoke,unsigned char dual_mode,unsigned char output_mode)
{
	short i=0;
	unsigned char reg_value1=0,reg_value2=0;

	if(Karaoke)
		reg_value1=0x01;
	else
		reg_value1=0x00;

	reg_value2 |= (dual_mode & 0x3) << 4;
	reg_value2 |= output_mode & 0xF;

	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x0A;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value1;	dsp_buf[i++] =reg_value2;

	DspSpiWriteNByte(dsp_buf,i);
}

void OS_OUTPUT_MODE_CONTROL_Debug()
{
	short i=0;
		
	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x07;

	DspSpiWriteNByte(dsp_buf,i);
}

void OS_OUTPUT_MODE_CONTROL_FOR_DSPB_Debug()
{
	short i=0;
	
	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x0A;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x07;

	DspSpiWriteNByte(dsp_buf,i);
}



#if 0
/* 
 * @brief DSPB OUTPUT MODE ENABLE
 *
 * @author chenjianming
 * @date 2007-09-06
 * @refer AN288 Datasheet P18
 */
void OS_DSPB_OUTPUT_MODE_ENABLE(unsigned char enable)
{
	short i=0;
	unsigned char reg_value=0;

	if(enable)
		reg_value=0x01;
	else
		reg_value=0x00;

	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x0B;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}
#endif

#if 0
/*
 * @brief PLL Standard Configuration
 *
 * @author chenjianming
 * @date 2007-09-06
 * @refer AN288 Datasheet P20
 */
void OS_PLL_STANDARD_CONFIG(unsigned char clock_freq,unsigned char Speed_Grade_Select,unsigned char PLL_Config)
{
	short i=0;
	unsigned char reg_value=0;

	reg_value |=(clock_freq & 0x0F) << 4;
	reg_value |=(Speed_Grade_Select & 0x03)<<2;

	if(PLL_Config)
		reg_value|=0x01;

	dsp_buf[i++] =OS_A_B_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x54;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}
#endif

#if 0
/*
 * @brief DSP OS Firmware configuration
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer AN288 Datasheet P15
 */
void DSP_0S_Firmware_Cfg(unsigned char channel_mode)
{
	ContextSwitchDSPA(); /*下面设置必须发送到DSPA,故向DSP 发送这个命令*/	

	OS_IO_CONFIG_Control(channel_mode);
	switch(channel_mode)
	{
		case Digital_INPUT: /*数字输入*/
			OS_AUTODETECT_Config(Enable,Disable,Enable);
			OS_PCM_AUTOSWITCH_ENABLE(Enable);
			break;			
		case Analog_INPUT_2ch: /*模拟2 通道*/
			OS_AUTODETECT_Config(Enable,Disable,Enable);
			OS_PCM_AUTOSWITCH_ENABLE(Enable);
			break;			
		case Analog_Input_Multi_ch: /*模拟多通道*/
			OS_AUTODETECT_Config(Enable,Disable,Disable);
			OS_PCM_AUTOSWITCH_ENABLE(Disable);
			break;
	}

  	//OS_SAMPLE_RATE_CONTROL(_48k); /*default (48K)*/ 	
	OS_OUTPUT_MODE_CONTROL(Disable,Stereo,L_C_R_Ls_Rs_Sbl_Sbr); /* (3/4 L/C/R/LS/RS/SBL/SBR)*/	
	OS_OUTPUT_MODE_CONTROL_FOR_DSPB(Disable,Stereo,L_C_R_Ls_Rs_Sbl_Sbr); /* (3/4 L/C/R/LS/RS/SBL/SBR)*/	
	OS_DSPB_OUTPUT_MODE_ENABLE(Enable); /*Enable disparate output mode for DSP B */	
	OS_PLL_STANDARD_CONFIG(_12_288MHZ,_100MHZ,Standard); /*12.288M \100MHZ \Standard*/
}
#endif

/*
 * @brief DSP OS Firmware Read
 *
 * @author chenjianming
 * @date 2008-06-24
 * @refer AN288 Datasheet P12
 */
unsigned long DspReadOS(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0x81;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);   
	return ReadDsp();
}


