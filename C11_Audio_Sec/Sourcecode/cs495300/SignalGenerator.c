/*@file   SignalGenerator.c
 * @brief Signal Generator Module Manager
 *
 * @author chenjianming
 * @date 2007-09-07
 * @refer AN246DH
 *
 * Write Message=0x9A00HHHH 0xhhhhhhhh
 * Read Request Message=0x9AC0HHHH;
 * Read Response Message=0x1AC0HHHH 0xhhhhhhhh
 * 0xHHHH=index 0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "BassManager.h"
#include "SignalGenerator.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief Signal Generator Enable
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246DH P4
 */
void DSP_SGEN_ENABLE(unsigned char mode)
{
	if(mode)
		DSPFirmwareWrite(Signal_Generator_Wop,0x0000,0x00000001); 
	else
		DSPFirmwareWrite(Signal_Generator_Wop,0x0000,0x00000000);
}

/* 
 * @brief Signal Generator Channel Enable
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246DH p4
 */
void DSP_SGEN_CHANNEL_ENABLE(unsigned long value)
{
	short i=0;

	switch(value)
		{
		case CH_L :		value=0x00000001;		break;
		case CH_C:		value=0x00000010;		break;
		case CH_R:		value=0x00000100;		break;
		case CH_LS:		value=0x00001000;		break;
		case CH_RS:		value=0x00010000;		break;
		case CH_LB:		value=0x00100000;		break;
		case CH_RB:		value=0x01000000;		break;		
		case CH_SW:		value=0x10000000;		break;
		}
	
	dsp_buf[i++] =Signal_Generator_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =(value>>24 & 0xFF);	dsp_buf[i++] =(value>>16 & 0xFF);	dsp_buf[i++] =(value>>8 & 0xFF);	dsp_buf[i++] =value & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief 噪声信号类型选择
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246DH P4
 */
void DSP_CHANNEL_SIGNAL_SELECT(Dsp_Channel channel,unsigned char signal_type)
{
	short i=0;
	unsigned char index_value,type_value;
	
	switch(channel)
		{
			case CH_L:		index_value=0x02;			break;
			case CH_C:		index_value=0x03;			break;
			case CH_R:		index_value=0x04;			break;
			case CH_LS:		index_value=0x05;			break;
			case CH_RS: 		index_value=0x06;			break;
			case CH_LB:		index_value=0x07;			break;
			case CH_RB:		index_value=0x08;			break;
			case CH_SW:		index_value=0x09;			break;
		}

	switch(signal_type)
		{
			case Signal_White_Noise:			type_value=0x00;			break;
			case Signal_Dolby_Pink_Noise:		type_value=0x01;			break;
			case Signal_THX_Pink_Noise:		type_value=0x02;			break;
			case Signal_Bass_Noise:			type_value=0x03;			break;
			case Signal_Sine_Wave:			type_value=0x04;			break;
			case Signal_Square_Wave:			type_value=0x05;			break;
			case Signal_DC_Wave:				type_value=0x06;			break;
			case Signal_Impulse_Wave:			type_value=0x07;			break;
		}

	dsp_buf[i++] =Signal_Generator_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;	
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =type_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  读取SGEN 固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0x9AC0HHHH
 * 		 Read Response Message =0x1AC0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
 unsigned long DspReadSgen(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0x9A;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}


