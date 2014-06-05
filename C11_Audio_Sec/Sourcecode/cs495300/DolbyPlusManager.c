/* @file   DolbyPlusManager.c
 * @brief 	Dolby Plus Firmware Module Manager
 *
 * @author chenjianming
 * @date    2008-05-09
 * @refer   AN304DA P3
 *
 *		  Write Message=0x9000HHHH 0xhhhhhhhh
 *		  Read Request Message=0x90C0HHHH
 *		  Read Response Message=0x10C0HHHH 0xhhhhhhhh
 *		  0xHHHH=index,0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "DolbyManager.h"
#include "DolbyPlusManager.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief  Dolby Plus Control
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DA P3
 */
void DSP_EC3_Control(unsigned char EC3_enable,unsigned char LFE_enable,unsigned char LFE2_enable,unsigned char Transcoded_enable,unsigned char compressed_enable)	
{
	short i=0;
	unsigned char reg_value1=0,reg_value2=0;

	if(EC3_enable)
		reg_value1|=0x01;
	if(LFE_enable)
		reg_value1|=0x10;
	if(LFE2_enable)
		reg_value1|=0x20;
	if(Transcoded_enable)
		reg_value2|=0x01;
	if(compressed_enable)
		reg_value2|=0x10;

	dsp_buf[i++] =Dolby_Digital_Plus_Wop; 	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value2;	dsp_buf[i++] =reg_value1;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby Plus Stereo Mode Control
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DA P3
 */
void DSP_EC3_Stereo_Mode_Control(unsigned char mode)
{
	short i=0;
	unsigned char reg_value=0;

	reg_value|=mode & 0x03;
	
	dsp_buf[i++] =Dolby_Digital_Plus_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby Plus Compression Control
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DA P3
 */
 void DSP_EC3_Compression_Control(unsigned char mode)
{
	short i=0;
	unsigned char reg_value=0;

	reg_value |=mode & 0x03;

	dsp_buf[i++] =Dolby_Digital_Plus_Wop; 	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby Plus Compression cut scale factor(-x setting of Dolby C decoder)
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DA P3
 */
void DSP_EC3_CUT_X(unsigned long value)
{
	short i=0;

	dsp_buf[i++] =Dolby_Digital_Plus_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x03;
	dsp_buf[i++] =(value >> 24) & 0xFF;	dsp_buf[i++] =(value >> 16) & 0xFF;	dsp_buf[i++] =(value >> 8) & 0xFF;	dsp_buf[i++] =value & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby Plus Compression boost scale factor(-y setting of Dolby C decoder)
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DA P3
 */
void DSP_EC3_BOOST_Y(unsigned long value)
{
	short i=0;

	dsp_buf[i++] =Dolby_Digital_Plus_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x04;	
	dsp_buf[i++] =(value >> 24) & 0xFF;	dsp_buf[i++] =(value >> 16) & 0xFF;	dsp_buf[i++] =(value >> 8) & 0xFF;	dsp_buf[i++] =value & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  读取Dolby Plus固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0x90C0HHHH
 * 		 Read Response Message =0x10C0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer chenshiguang C11 code
 */
 unsigned long DspReadEC3(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0x90;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}

