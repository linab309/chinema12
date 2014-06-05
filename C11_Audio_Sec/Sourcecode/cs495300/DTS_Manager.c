/* @file	DTS_Manager.c
 * @brief DTS Manager File
 *
 * @author chenjianming
 * @date 2008-1-7
 * @refer AN246DB
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief DTS 使能控制
 *
 * @author chenjianming
 * @date 2007-1-7
 * @refer AN246DB
 */
void DSP_DTS_Control(unsigned char DTS_Enable,unsigned char DRC_Enable,unsigned char DTS_Extension_Decode)
{
	short i=0;
	unsigned char DTS_Enable_Value=0;
	unsigned char DRC_Enable_Value=0;
	unsigned char DTS_Extension_Value=0;

	if(DTS_Enable)
		DTS_Enable_Value=0x01;
	if(DRC_Enable)
		DRC_Enable_Value=0x01;
	if(DTS_Extension_Decode)
		DTS_Extension_Value=0x01;

	dsp_buf[i++]=DTS_Manager_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=DTS_Extension_Value;	dsp_buf[i++]=DRC_Enable_Value;	dsp_buf[i++]=DTS_Enable_Value;
	
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  读取DTS 固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0x91C0HHHH
 * 		 Read Response Message =0x11C0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
unsigned long DspReadDTS(unsigned char index)
{
	short i=0;

	dsp_buf[i++]=0x91;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x00;	dsp_buf[i++]=index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}

