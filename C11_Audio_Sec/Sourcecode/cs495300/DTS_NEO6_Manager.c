/* @file   DTS_NEO6_Manager.c
 * @brief  DTS NEO6 Manager File
 *
 * @author chenjianming
 * @date 2007-1-7
 * @refer AN246MPB
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief NEO6 使能控制
 *
 * @author chenjianming
 * @date 2007-1-7
 * @refer AN246DB
 */
void DTS_NEO6_Control(unsigned char NEO6_Enable)
{
	short i=0;
	unsigned char reg_value;

	if(NEO6_Enable)
		reg_value=0x01;

	dsp_buf[i++]=DTS_NEO6_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief NEO6 Mode Select
 *
 * @author chenjianming
 * @date 2007-1-7
 * @refer AN246DB
 */
void DTS_NEO6_Mode(unsigned char Wide_Mode,unsigned char NEO6_Mode)
{
	short i=0;
	unsigned char reg_value;

	if(Wide_Mode)
		reg_value|=0x02;
	if(NEO6_Mode)
		reg_value|=0x01;

	dsp_buf[i++]=DTS_NEO6_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x01;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief NEO6 Center Gain Setting
 *
 * @author chenjianming
 * @date 2007-1-7
 * @refer AN246DB
 */
void DTS_NEO6_CGain(unsigned long Gain_Value)
{
	short i=0; 

	dsp_buf[i++]=DTS_NEO6_Wop;	 dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x02;
	dsp_buf[i++]=(Gain_Value>>24)&0xFF;	 dsp_buf[i++]=(Gain_Value>>16)&0xFF;	dsp_buf[i++]=(Gain_Value>>8)&0xFF;	dsp_buf[i++]=Gain_Value&0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  读取NEO6 固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0xB5C0HHHH
 * 		 Read Response Message =0x35C0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
unsigned long DspReadNEO6(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0xB5;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}
