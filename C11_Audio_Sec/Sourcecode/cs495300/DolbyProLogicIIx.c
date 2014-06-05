/* @file  DolbyPrologicIIx.c
 * @brief Dolby Prologic IIx Firmware Manager
 *
 * @author chenjianming
 * @date 2007-10-15
 * @refer AN246MPA
 *
 * Write Message=0xBF00HHHH 0xhhhhhhhh
 * Read Request Message=0xBFC0HHHH;
 * Read Response Message=0x3FC0HHHH 0xhhhhhhhh
 * 0xHHHH=index 0xhhhhhhhh=data value
 */

#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "DolbyPrologicII.h"
#include "DolbyPrologicIIx.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/* 
 * @brief Dolby PrologicIIx 使能控制
 *
 * @author chenjianming
 * @date 2007-10-15
 * @refer AN246MPA P4
 */
void DSP_PLIIx_Control(unsigned char enable)
{
	short i=0;
	unsigned char value=0;

	if(enable)
		value=0x01;
	else
		value=0x00;

	dsp_buf[i++] =Dolby_Pro_LogicIIx_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief Dolby PrologicIIx Decode Mode Select (针对输入源为立体声)
 *
 * @author chenjianming
 * @date 2007-10-15
 * @refer AN246MPA P4
 */
void DSP_PLIIx_DECODE_MODE_STEREO(unsigned char mode)
{
	short i=0;
	unsigned char Decode_Mode;

	switch(mode)
		{
			case Pro_Logic_Emulation_Mode:	 	Decode_Mode=0x00; 		break;
			case Virtual_Mode:				Decode_Mode=0x01;		break;
			case Music_Mode:					Decode_Mode=0x02;		break;
			case Movie_Mode:					Decode_Mode=0x03;		break;
			case Matrix_Mode:					Decode_Mode=0x04;		break;
			case Custom_Mode:				Decode_Mode=0x05;		break;
		}

	dsp_buf[i++] =Dolby_Pro_LogicIIx_Wop;	dsp_buf[i++] =0X00;	dsp_buf[i++] =0X00;	dsp_buf[i++] =0X01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =Decode_Mode;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief Dolby PrologicIIx Control Register Setting (only the Panorama setting)
 *
 * @author chenjianming
 * @date 2007-10-15
 * @refer AN246MPA P4
 * @note Only Custom and Music Mode
 */
void DSP_PLIIx_CNOTROL_REGISTER(unsigned char Panorama_Enable)
{
	short i=0;
	unsigned char value;

	if(Panorama_Enable)
		value=0x04;
	else
		value=0x00;

	dsp_buf[i++] =Dolby_Pro_LogicIIx_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);	
}

/* 
 * @brief Pro Logic IIx Center Width Configuration
 *
 * @author chenjianming
 * @date 2007-10-15
 * @refer AN246MPA P4
 * @note  只有在Custom and Music Mode 起作用
 */
void DSP_PLIIx_CENTER_WIDTH_CONFIGURATION(unsigned char Width)
{
	short i=0;
	unsigned char value;

	switch(Width)
		{
			case 0:		value=0x00;		break;
			case 1:		value=0x01;		break;
			case 2:		value=0x02;		break;
			case 3:		value=0x03;		break;
			case 4:		value=0x04;		break;
			case 5:		value=0x05;		break;
			case 6:		value=0x06;		break;
			case 7:		value=0x07;		break;			
		}

	dsp_buf[i++] =Dolby_Pro_LogicIIx_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x03;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);	
}


/*
 * @brief PrologicIIx Dimension Configuration
 *
 * @author chenjianming
 * @date 2007-10-15
 * @refer AN246MPA P5
 * @note  只有在Custom and Music Mode 起作用
 */
void DSP_PLIIx_DIMENSION_CONFIGURATION(unsigned char Dimension)
{
	short i=0;
	unsigned char value;

	switch(Dimension)
		{
			case 0:		value=0x00;		break;
			case 1:		value=0x01;		break;
			case 2:		value=0x02;		break;
			case 3:		value=0x03;		break;
			case 4:		value=0x04;		break;
			case 5:		value=0x05;		break;
			case 6:		value=0x06;		break;
		}

	dsp_buf[i++] =Dolby_Pro_LogicIIx_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x04;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief Dolby PrologicIIx Decode Mode Select (针对输入源为多通道)
 *
 * @author chenjianming
 * @date 2007-10-15
 * @refer AN246MPA P5
 */
void DSP_PLIIx_DECODE_MODE_MULTI(unsigned char mode)
{
	short i=0;
	unsigned char Decode_Mode;

	switch(mode)
		{
			case Reserved:			Decode_Mode=0x00;			break;
			case EX_Mode:			Decode_Mode=0x01;			break;
			case MUSIC_Mode:		Decode_Mode=0x02;			break;
			case MOVIE_Mode:		Decode_Mode=0x03;			break;
		}

	dsp_buf[i++] =Dolby_Pro_LogicIIx_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x05;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =Decode_Mode;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief  读取Dolby PLIIX固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0xBFC0HHHH
 * 		 Read Response Message =0x3FC0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
unsigned long DspReadDolbyPLIIX(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0xBF;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}


