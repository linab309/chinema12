/*@file      PCM_Manager.c
 * @brief    PCM Firmware Manager
 *
 * @author chenjianming
 * @date 	   2007-09-06
 * @refer    AN288 Datasheet P29
 *
 *		   Write=0x9B00HHHH 0xhhhhhhhh
 *		   Read Request=0x9BC0HHHH 
 *		   Read Response=0x1BC0HHHH 0xhhhhhhhh
 *		   0xHHHH=index,0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "OS_Manager.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief PCM ENABLE
 *
 * @author chenjianming
 * @date 2007-09-06
 * @refer AN288 Datasheet P29
 */
void DSP_PCM_ENABLE_Control(unsigned char Decimator,unsigned char De_emphasis,unsigned char PCM_Enable)
{
	short i=0;
	unsigned char reg_value1=0,reg_value2=0;

	if(Decimator)
		reg_value1=0x01;
	else
		reg_value1=0x00;

	if(De_emphasis)
		reg_value2|=0x10;
	if(PCM_Enable)
		reg_value2|=0x01;
	
	dsp_buf[i++] =PCM_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value1;	dsp_buf[i++] =reg_value2;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  读取PCM 固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0x9BC0HHHH
 * 		 Read Response Message =0x1BC0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
unsigned long DspReadPCM(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0x9B;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}

