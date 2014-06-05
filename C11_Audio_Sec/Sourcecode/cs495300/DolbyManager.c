/* @file   DolbyManager.c
 * @brief 	Dolby Firmware Module Manager
 *
 * @author chenjianming
 * @date    2007-09-07
 * @refer   AN246DA P4
 *
 *		  Write Message=0x9000HHHH 0xhhhhhhhh
 *		  Read Request Message=0x90C0HHHH
 *		  Read Response Message=0x10C0HHHH 0xhhhhhhhh
 *		  0xHHHH=index,0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "DolbyManager.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief  Dolby Digital Control
 *
 * @author chenjianming
 * @date 2007-09-07
 * @refer AN246DA P4
 */
void DSP_AC3_Control(unsigned char LFE_Enable,unsigned char AC3_Enable)
{
	short i=0;
	unsigned char reg_value=0;

	if(LFE_Enable)
		reg_value|=0x10;
	if(AC3_Enable)
		reg_value|=0x01;

	dsp_buf[i++] =Dobly_Digital_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby Stereo Mode Control
 *
 * @author chenjianming
 * @date 2007-09-07
 * @refer AN246DA P4
 */
void DSP_AC3_STEREO_MODE_CONTROL(unsigned char Stereo_Mode)
{
	short i=0;
	unsigned char reg_value=0;

	reg_value |=Stereo_Mode & 0x03;

	dsp_buf[i++] =Dobly_Digital_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby Compression Control
 *
 * @author chenjianming
 * @date 2007-09-07
 * @refer AN246DA P4
 */
void DSP_AC3_COMPRESSION_CONTROL(void)
{
	short i=0;

	dsp_buf[i++] =Dobly_Digital_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x10; 

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby Compression cut scale factor(-x setting of Dolby C decoder)
 *
 * @author chenjianming
 * @date 2007-09-07
 * @refer AN246DA P4
 */
void DSP_AC3_CUT_X(unsigned long value)
{
	short i=0;

	dsp_buf[i++] =Dobly_Digital_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x03;
	dsp_buf[i++] =(value >> 24) & 0xFF;	dsp_buf[i++] =(value >> 16) & 0xFF;	dsp_buf[i++] =(value >> 8) & 0xFF;	dsp_buf[i++] =value & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby Compression boost scale factor(-y setting of Dolby C decoder)
 *
 * @author chenjianming
 * @date 2007-09-07
 * @refer AN246DA P4
 */
void DSP_AC3_BOOST_Y(unsigned long value)
{
	short i=0;

	dsp_buf[i++] =Dobly_Digital_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x04;	
	dsp_buf[i++] =(value >> 24) & 0xFF;	dsp_buf[i++] =(value >> 16) & 0xFF;	dsp_buf[i++] =(value >> 8) & 0xFF;	dsp_buf[i++] =value & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby dynamics compress 设置为最大
 *
 * @author chenjianming
 * @date 2009-02-12
 * @refer 
 */
void DSP_AC3_DRC_INIT(void)
{
	DSP_AC3_COMPRESSION_CONTROL();
	DSP_AC3_CUT_X(0x00);
	DSP_AC3_BOOST_Y(0x00);
}

/*
 * @brief  读取Dolby 固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0x90C0HHHH
 * 		 Read Response Message =0x10C0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
unsigned long DspReadDolby(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0x90;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}

//#if 0
unsigned long DspReadCrossbar(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0xDB;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}

unsigned long DspReadRegist(unsigned char module,unsigned char index)
{
	short i=0;

	dsp_buf[i++] =module;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0X00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}
//#endif

