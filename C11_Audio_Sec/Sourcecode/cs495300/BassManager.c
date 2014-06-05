/*@file       	BassManager.c
 * @brief     	Bass Manager Driver File
 *
 * @author 	chenjianming
 * @date    	2007-09-10
 * @refer   	AN246PPB
 *		  	chenshiguang C11 code (Bassmanager.c)
 *
 * 		  	Write=0xD700HHHH 0xhhhhhhhh
 *		  	Read Request=0xD7C0HHHH
 *		  	Read Response=0x57C0HHHH 0xhhhhhhhh
 *
 * 		  	0xHHHH=index 0xhhhhhhhh=data value
 */
#include "math.h"  
#include "dspdrv.h"
#include "BassManager.h"
#include "..\driver\Dsp_Spi.h"
#include "..\main\Debug.h"

#ifdef _DEBUG
//#define _DEBUG_BASS_MANAGER
#endif

_BASS_CORNER dsp_bass_corner;
extern unsigned char system_spk_mode[8];

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief: converting a dB value into a 1.31 format hex value
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: chenshiguang C11 code (Bassmanager.c)
 */
unsigned long ConvertDbValue(float db_value)
{
	unsigned long rtn_value=0;
	
	db_value = pow(10,db_value/20); /*calculate db value to gain value*/
	rtn_value = db_value * POW_2_31;	
	return rtn_value;
}

/*
 * @brief: Bass Enable Manager Control
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P10
 */
 void DSP_BASS_CONTROL(unsigned char Bass_Enable,unsigned char Sub1_Sum_Enable,unsigned char Sub2_Sum_Enable,unsigned char Peak_Limiter_Enable,unsigned char Peak_Limiter_Select)
{
	short i=0;
	unsigned char value=0;

	if(Bass_Enable)		
		value|=0x01;			
	if(Sub1_Sum_Enable)
		value|=0x08;
	if(Sub2_Sum_Enable)
		value|=0x10;
	if(Peak_Limiter_Enable)
		value|=0x20;
	if(Peak_Limiter_Select)
		value|=0x40;

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief: Bass 5XOVER Control
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P11
 */
void DSP_BASS_MGR_5XOVER_CONTROL(unsigned long value)
{
	short i=0;
	
	dsp_buf[i++] =Bass_Manager_Wop;		dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;	
	dsp_buf[i++] =(value>>24 & 0xFF);	dsp_buf[i++] =(value>>16 & 0xFF);	dsp_buf[i++] =(value>>8 & 0xFF);	dsp_buf[i++] =value & 0xFF;	

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief: Bass HPF Control
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P12
 */
void DSP_BASS_MGR_HPF_CONTROL(unsigned long value)
{
	short i=0;

	dsp_buf[i++] =Bass_Manager_Wop;		dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;
	dsp_buf[i++] =(value>>24 & 0xFF);	dsp_buf[i++] =(value>>16 & 0xFF);	dsp_buf[i++] =(value>>8 & 0xFF);	dsp_buf[i++] =value & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
* @brief: bass L/C/R/LS/RS/LB/RB/SUM1/SUM2/SUM3/SUM4/SUM5 Corner frequency Setting
* @author: chenjianming
* @data: 2007-09-11
* @refer: AN246PPB  P13-P14
*/
void DSP_SET_BASS_CORNER(Bass_Channel channel,unsigned char freq)
{
	short i=0;
	unsigned char index_value, freq_value;

	switch(channel)
		{
		case BASS_L_HPF:		index_value=0x03;		break;
		case BASS_C_HPF:		index_value=0x04;		break;
		case BASS_R_HPF:		index_value=0x05;		break;
		case BASS_LS_HPF:		index_value=0x06;		break;
		case BASS_RS_HPF:		index_value=0x07;		break;
		case BASS_LB_HPF:		index_value=0x08;		break;
		case BASS_RB_HPF:		index_value=0x09;		break;
		case BASS_SUM1_LPF:		index_value=0x0A;		break;
		case BASS_SUM2_LPF:		index_value=0x0B;		break;
		case BASS_SUM3_LPF:		index_value=0x0C;		break;
		case BASS_SUM4_LPF:		index_value=0x0D;		break;
		case BASS_SUM5_LPF:		index_value=0x0E;		break;
		}

	switch(freq)
		{
		case CORNER_FREQUENCY_40HZ:	freq_value = 0x00;		break;
		case CORNER_FREQUENCY_45HZ:	freq_value = 0x01;		break;
		case CORNER_FREQUENCY_50HZ:	freq_value = 0x02;		break;	
		case CORNER_FREQUENCY_55HZ:      freq_value = 0x03;	break;
		case CORNER_FREQUENCY_60HZ:	freq_value = 0x04;		break;
		case CORNER_FREQUENCY_65HZ:	freq_value = 0x05;		break;
		case CORNER_FREQUENCY_70HZ:	freq_value = 0x06;		break;
		case CORNER_FREQUENCY_75HZ:	freq_value = 0x07;		break;
		case CORNER_FREQUENCY_80HZ:	freq_value = 0x08;		break;
		case CORNER_FREQUENCY_90HZ:	freq_value = 0x09;		break;
		case CORNER_FREQUENCY_100HZ:	freq_value = 0x0A;		break;
		case CORNER_FREQUENCY_110HZ:	freq_value = 0x0B;		break;
		case CORNER_FREQUENCY_120HZ:	freq_value = 0x0C;		break;
		case CORNER_FREQUENCY_130HZ:	freq_value = 0x0D;		break;
		case CORNER_FREQUENCY_140HZ:	freq_value = 0x0E;		break;
		case CORNER_FREQUENCY_150HZ:	freq_value = 0x0F;		break;
		case CORNER_FREQUENCY_175HZ:	freq_value = 0x10;		break;
		case CORNER_FREQUENCY_200HZ:	freq_value = 0x11;		break;
		case CORNER_FREQUENCY_225HZ:	freq_value = 0x12;		break;
		case CORNER_FREQUENCY_250HZ:	freq_value = 0x13;		break;
		}

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =freq_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief: level adjustment for input (L/C/R/LS/RS/LB/RB)channel pass through
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P14 
 *             chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_INPUT_LEVEL(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
			case CH_L:		index_value = 0x0F;		break;
			case CH_C:		index_value = 0x10;		break;
			case CH_R:		index_value = 0x11;		break;		
			case CH_LS:		index_value = 0x12;		break;
			case CH_RS:		index_value = 0x13;		break;
			case CH_LB:		index_value = 0x14;		break;
			case CH_RB:		index_value = 0x15;		break;
		}

	level_value=ConvertDbValue(level);
	
	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);
}

/* 
 * @brief: Bass LFE output level setting (L/C/R/LS/RS/LB/RB)
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P15 
 *             chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_LFE_OUTPUT_LEVEL(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
		default:
		case CH_L:		index_value = 0x16;		break;
		case CH_C:		index_value = 0x17;		break;
		case CH_R:		index_value = 0x18;		break;
		case CH_LS:		index_value = 0x19;		break;
		case CH_RS:		index_value = 0x1A;		break;
		case CH_LB:		index_value = 0x1B;		break;
		case CH_RB:		index_value = 0x1C;		break;
		}
	
	level_value = ConvertDbValue(level);
	
	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);
}

/* 
 * @brief: BASS Front Sum Output Level Setting (L/C/R)
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P15
 *		 chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_FRONT_SUM_OUTPUT_LEVEL(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
		case CH_L:		index_value = 0x1D;		break;
		case CH_C:		index_value = 0x1E;		break;
		case CH_R:		index_value = 0x1F;		break;			
		}

	level_value = ConvertDbValue(level);
	
	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief: BASS Back Sum Output Level Setting (LS/RS/LB/RB)
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P15
 *		 chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_BACK_SUM_OUTPUT_LEVEL(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
		case CH_LS:		index_value = 0x20;		break;
		case CH_RS:		index_value = 0x21;		break;
		case CH_LB:		index_value = 0x22;		break;
		case CH_RB:		index_value = 0x23;		break;
		}
	
	level_value = ConvertDbValue(level);
	
	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);		
}

/*
 * @brief: BASS SUM1 Input Level Setting (L\R\LFE)
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P15
 *		  chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_SUM1_INPUT_LEVEL(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;
	
	switch(channel)
		{
		case CH_L:		index_value = 0x24;		break;
		case CH_R:		index_value = 0x25;		break;
		case CH_SW:		index_value = 0x26;		break;
		}

	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);		
}

/*
 * @brief: BASS SUM2 Input Level Setting (C/LFE)
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P16
 *		chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_SUM2_INPUT_LEVEL(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
			case CH_C:		index_value = 0x27;		break;
			case CH_SW:		index_value = 0x28;		break;
		}
	
	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief: BASS SUM3 Input Level Setting (LS/RS/LFE)
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P16
 *		  chenshiguang C11 code (BassManager.c)
 */
 void DSP_BASS_SUM3_INPUT_LEVLE(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
			case CH_LS:		index_value=0x29;			break;
			case CH_RS:		index_value=0x2A;			break;
			case CH_SW:		index_value=0x2B;			break;
		}

	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);		
}

/*
 * @brief: BASS SUM4 Input Level Setting (LB/RB/LFE)
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P16
 *		  chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_SUM4_INPUT_LEVLE(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
			case CH_LB: 		index_value=0x2C;			break;
			case CH_RB:		index_value=0x2D;			break;
			case CH_SW:      	index_value=0x2E;			break;
		}
	
	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);		
}

/*
 * @brief: BASS SUM5 Input Level Setting (L/C/R/LS/RS/LB/RB/LFE)
 *
 * @author: chenjianming
 * @date: 2007-09-11
 * @refer: AN246PPB P16
 *		  chenshiguang C11 code (BassManager.c)
 */
 void DSP_BASS_SUM5_INPUT_LEVLE(Dsp_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
			case CH_L:		index_value=0x2f;			break;
			case CH_C:		index_value=0x30;			break;
			case CH_R:		index_value=0x31;			break;
			case CH_LS:		index_value=0x32;			break;
			case CH_RS:		index_value=0x33;			break;
			case CH_LB:		index_value=0x34;			break;
			case CH_RB:		index_value=0x35;			break;
			case CH_SW:		index_value=0x36;			break;			
		}

	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);		
}

/*
 * @brief: BASS (SUM1/SUM2/SUM3/SUM4/SUM5) Input Level Front Setting
 *
 * @author: chenjianming
 * @date: 2007-09-12
 * @refer: AN246PPB P16-P17
 *		 chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_X_INPUT_LEVEL_FRONT(Sum_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
			case SUM1:		index_value=0x37;			break;
			case SUM2:		index_value=0x38;			break;
			case SUM3:		index_value=0x39;			break;
			case SUM4:		index_value=0x3A;			break;
			case SUM5:		index_value=0x3B;			break;
		}

	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);			
}

/* 
 * @brief: BASS (SUM1/SUM2/SUM3/SUM4/SUM5) Input Levle Back Setting
 *
 * @author: chenjianming
 * @date: 2007-09-12
 * @refer: AN246PPB P17
 *		  chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_X_INPUT_LEVEL_BACK(Sum_Channel channel,float level)
{
	short i=0;
	unsigned char index_value;
	unsigned long level_value;

	switch(channel)
		{
			case SUM1:		index_value=0x3C;			break;
			case SUM2:		index_value=0x3D;			break;
			case SUM3:		index_value=0x3E;			break;
			case SUM4:		index_value=0x3F;			break;
			case SUM5:		index_value=0x40;			break;
		}
	
	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);			
}

/* 
 * @brief: BASS Input Sub Level Setting 
 *
 * @author: chenjianming
 * @date: 2007-09-12
 * @refer: AN246PPB P17
 *		  chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_INPUT_SUB_LEVEL(float level)
{
	short i=0;
	unsigned long level_value;

	level_value = ConvertDbValue(level);	
	
	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x41;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);			
}

/* 
 * @brief: BASS Front Sum Output Sub Level 
 *
 * @author: chenjianming
 * @date: 2007-09-12
 * @refer: AN246PPB P17
 *		 chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_FRONT_SUM_OUTPUT_SUB_LEVEL(float level)
{
	short i=0;
	unsigned long level_value;

	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x42;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);		
}

/* 
 * @brief: BASS Back Sum Output Sub Level 
 *
 * @author: chenjianming
 * @date: 2007-09-12
 * @refer: AN246PPB P17
 *		  chenshiguang C11 code (BassManager.c)
 */
void DSP_BASS_BACK_SUM_OUTPUT_SUB_LEVEL(float level)
{
	short i=0;
	unsigned long level_value;

	level_value = ConvertDbValue(level);	

	dsp_buf[i++] =Bass_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x43;
	dsp_buf[i++] =(level_value>>24 & 0xFF);	dsp_buf[i++] =(level_value>>16 & 0xFF);	dsp_buf[i++] =(level_value>>8 & 0xFF);	dsp_buf[i++] =level_value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);		  
}

/*   
 * @brief: 设置输出通道模式
 *
 * @author: chenjianming
 * @date: 2007-09-20
 * @refer: chenshiguang C11 code (BassManager.c)
 */
void ConfigSpkMode(void)   
{
	unsigned char left_flag = 0,right_flag = 0;
	unsigned long fl_vol = 0,sl_vol = 0,sbl_vol = 0,fr_vol = 0,sr_vol = 0,sbr_vol = 0,cen_vol = 0,sw_vol = 0;

		if(system_spk_mode[CH_L])
			{
			//fl_vol=0x7FFFFFFF; 
			fl_vol=0x3FFFFFFF; //change by cjm 2008-10-6
			left_flag|=0x01;
			}
		if(system_spk_mode[CH_LS])
			{
			//sl_vol=0x7FFFFFFF;
			sl_vol=0x3FFFFFFF; //change by cjm 2008-10-6
			left_flag|=0x02;
			}
		if(system_spk_mode[CH_LB])
			{
			//sbl_vol = 0x7FFFFFFF;
			sbl_vol = 0x3FFFFFFF; //change by cjm 2008-10-6
			left_flag |= 0x04;			
			}
		if(system_spk_mode[CH_C])
			{
			//cen_vol = 0x7FFFFFFF;
			cen_vol = 0x3FFFFFFF; //change by cjm 2008-10-6
			}
		if(system_spk_mode[CH_R])
			{
			//fr_vol = 0x7FFFFFFF;
			fr_vol = 0x3FFFFFFF; //change by cjm 2008-10-6
			right_flag |= 0x01;			
			}
		if(system_spk_mode[CH_RS])
			{
			//sr_vol = 0x7FFFFFFF;
			sr_vol = 0x3FFFFFFF; //change by cjm 2008-10-6
			right_flag |= 0x02;
			}
		if(system_spk_mode[CH_RB])
			{
			//sbr_vol = 0x7FFFFFFF;
			sbr_vol = 0x3FFFFFFF; //change by cjm 2008-10-6
			right_flag |= 0x04;
			}
		if(system_spk_mode[CH_SW])
			{
			//sw_vol = 0x7FFFFFFF;
			sw_vol = 0x3FFFFFFF; //change by cjm 2008-10-6
			}

		DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_L_COEF+MIXER_L_OFFSET,fl_vol);
		DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_Ls_COEF+MIXER_LS_OFFSET,sl_vol);
		DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_Lb_COEF+MIXER_LB_OFFSET,sbl_vol);
		DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_R_COEF+MIXER_R_OFFSET,fr_vol);
		DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_Rs_COEF+MIXER_RS_OFFSET,sr_vol);
		DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_Rb_COEF+MIXER_RB_OFFSET,sbr_vol);
		DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_C_COEF+MIXER_C_OFFSET,cen_vol);
		DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_LFE0_COEF+MIXER_SW_OFFSET,sw_vol);

		switch(left_flag)
			{
			case 0x01: /*如果LS和LB都关闭的话,把这2个声道的声音都加到L 声道*/
				DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_L_COEF+MIXER_LS_OFFSET,0x7FFFFFFF);
				DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_L_COEF+MIXER_LB_OFFSET,0x7FFFFFFF);
				break;
			case 0x02: 
			case 0x03: /*如果LB 关闭的话，把LB 声道的声音加到LS上*/
				DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_Ls_COEF+MIXER_LB_OFFSET,0x7FFFFFFF);
				break;
			case 0x04: 
			case 0x05: /*如果LB 开的话，把LS声道的声音加到LB上*/
				DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_Lb_COEF+MIXER_LS_OFFSET,0x7FFFFFFF);
				break;
			}

		switch(right_flag)
			{
			case 0x01:  /*如果RS和RB都关闭的话,把这2个声道的声音都加到R声道*/
				DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_R_COEF+MIXER_RS_OFFSET,0x7FFFFFFF);
				DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_R_COEF+MIXER_RB_OFFSET,0x7FFFFFFF);
				break;
			case 0x02: 
			case 0x03: /*如果RB 关闭的话，把RB 声道的声音加到RS上*/
				DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_Rs_COEF+MIXER_RB_OFFSET,0x7FFFFFFF);
				break;
			case 0x04: 
			case 0x05:  /*如果RB 开的话，把RS声道的声音加到RB上*/
				DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_Rb_COEF+MIXER_RS_OFFSET,0x7FFFFFFF);
				break;
			}

		if((fl_vol == 0) && cen_vol) /*如果左声道关,中置开，则把左声道的声音加到中置声道*/
			{
			DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_C_COEF+MIXER_L_OFFSET,0x7FFFFFFF);
			}
		else if((cen_vol == 0) && fl_vol) /*如果中置关了,左声道开，则把中置的声音加到左声道*/
			{
			DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_L_COEF+MIXER_C_OFFSET,0x3FFFFFFF);
			}	

		if((fr_vol == 0) && cen_vol)/*如果右声道关,中置开，则把右声道的声音加到中置声道*/
			{
			DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_C_COEF+MIXER_R_OFFSET,0x7FFFFFFF);
			}
		else if((cen_vol == 0) && fr_vol)/*如果中置关了,右声道开，则把中置的声音加到右声道*/
			{
			DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_R_COEF+MIXER_C_OFFSET,0x3FFFFFFF);
			}
  
		if((sw_vol == 0) && fl_vol) /*如果左声道开,LFE关,则把低音加到左声道*/
			{
			DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_L_COEF+MIXER_SW_OFFSET,0x3FFFFFFF);
			}
		if((sw_vol == 0) && fr_vol) /*如果右声道开,LFE关,则把低音加到右声道*/
			{
			DSPFirmwareWrite(Crossbar_Manager_Wop,MIXER_R_COEF+MIXER_SW_OFFSET,0x3FFFFFFF);
			}

		//if(pre_ab_code != DTS_ES_AB) /*changer later*/
		DSPFirmwareWrite(Crossbar_Manager_Wop,0x81,0x1); /*main output coefficients from Host*/
}

/* 
 * @brief: C11 Bass Setting
 *
 * @author: chenjianming
 * @date: 2007-09-20
 * @refer: chenshiguang C11 code (BassManager.c)
 */
extern float balance[8];
void C11SetBass(void)
{
	unsigned long hpf_flag = 0x000000,lpf_flag = 0x00000; /*change by cjm 2007-10-11*/
	unsigned char i = 0;

	if(dsp_bass_corner.sum1_corner) /*如果拐点频率大于最低的40HZ,就要使能LPF*/
		lpf_flag|=0x1F0000;
	
	DSP_BASS_MGR_5XOVER_CONTROL(lpf_flag); /*0x01*/

	if(dsp_bass_corner.fl_corner) /*有设置拐点0DB,无设置拐点衰减最大DB值*/
		{
		DSP_BASS_SUM1_INPUT_LEVEL(CH_L,0); /*0x24*/ 
		hpf_flag|=0x40;
		}
	else
		{
		DSP_BASS_SUM1_INPUT_LEVEL(CH_L,NEGATIVE_INFINITE); /*0x24*/
		}
	if(dsp_bass_corner.cen_corner)
		{
		DSP_BASS_SUM2_INPUT_LEVEL(CH_C,0); /*0x27*/
		hpf_flag|=0x20;
		}
	else
		{
		DSP_BASS_SUM2_INPUT_LEVEL(CH_C,NEGATIVE_INFINITE); /*0x27*/
		}
  	if(dsp_bass_corner.fr_corner)
  		{
  		DSP_BASS_SUM1_INPUT_LEVEL(CH_R,0); /*0x25*/
		hpf_flag|=0x10;
  		}
	else
		{
		DSP_BASS_SUM1_INPUT_LEVEL(CH_R,NEGATIVE_INFINITE); /*0x25*/
		}
	if(dsp_bass_corner.sl_corner)
		{
		DSP_BASS_SUM3_INPUT_LEVLE(CH_LS,0); /*0x29*/
		hpf_flag |= 0x08;
		}
	else
		{
		DSP_BASS_SUM3_INPUT_LEVLE(CH_LS,NEGATIVE_INFINITE); /*0x29*/
		}	
	if(dsp_bass_corner.sr_corner)
		{
		DSP_BASS_SUM3_INPUT_LEVLE(CH_RS,0); /*0x2A*/
		hpf_flag|=0x04;
		}
	else
		{
		DSP_BASS_SUM3_INPUT_LEVLE(CH_RS,NEGATIVE_INFINITE); /*0x2A*/
		}	
	if(dsp_bass_corner.sbl_corner)
		{
		DSP_BASS_SUM4_INPUT_LEVLE(CH_LB,0); /*0X2C*/
		hpf_flag|=0x02;
		}
	else
		{
		DSP_BASS_SUM4_INPUT_LEVLE(CH_LB,NEGATIVE_INFINITE); /*0X2C*/
		}
	if(dsp_bass_corner.sbr_corner)
		{
		DSP_BASS_SUM4_INPUT_LEVLE(CH_RB,0); /*0X2D*/ 
		hpf_flag|=0x01;
		} 
	else
		{
		DSP_BASS_SUM4_INPUT_LEVLE(CH_RB,NEGATIVE_INFINITE); /*0X2D*/
		} 

	DSP_BASS_MGR_HPF_CONTROL(hpf_flag); /*0x02*/

	ConfigSpkMode(); /*各通道分量分配设置*/

	for(i=0;i<8;i++) /*各通道ON/OFF 开关*/
		{
		if(i!=CH_SW)
			{
			if(system_spk_mode[i])
				DSP_BASS_INPUT_LEVEL(i,0); /*0x0F*/
			else
				DSP_BASS_INPUT_LEVEL(i,NEGATIVE_INFINITE); /*0x0F*/
			}
		else
			{
			if(system_spk_mode[i])
				DSP_BASS_INPUT_SUB_LEVEL(0); /*0x41*/
			else
				DSP_BASS_INPUT_SUB_LEVEL(NEGATIVE_INFINITE); /*0x41*/
			}
		}

	DSP_BASS_LFE_OUTPUT_LEVEL(CH_L,NEGATIVE_INFINITE); /*0X16*/
	DSP_BASS_LFE_OUTPUT_LEVEL(CH_C,NEGATIVE_INFINITE); /*0X17*/
	DSP_BASS_LFE_OUTPUT_LEVEL(CH_R,NEGATIVE_INFINITE); /*0X18*/
	DSP_BASS_LFE_OUTPUT_LEVEL(CH_LS,NEGATIVE_INFINITE); /*0X19*/
	DSP_BASS_LFE_OUTPUT_LEVEL(CH_RS,NEGATIVE_INFINITE); /*0X1A*/
	DSP_BASS_LFE_OUTPUT_LEVEL(CH_LB,NEGATIVE_INFINITE); /*0X1B*/
	DSP_BASS_LFE_OUTPUT_LEVEL(CH_RB,NEGATIVE_INFINITE); /*0X1C*/

	DSP_BASS_FRONT_SUM_OUTPUT_LEVEL(CH_L,NEGATIVE_INFINITE);  /*0X1D*/
	DSP_BASS_FRONT_SUM_OUTPUT_LEVEL(CH_C,NEGATIVE_INFINITE); /*0X1E*/
	DSP_BASS_FRONT_SUM_OUTPUT_LEVEL(CH_R,NEGATIVE_INFINITE); /*0X1F*/
	DSP_BASS_BACK_SUM_OUTPUT_LEVEL(CH_LS,NEGATIVE_INFINITE);  /*0X20*/
	DSP_BASS_BACK_SUM_OUTPUT_LEVEL(CH_RS,NEGATIVE_INFINITE); /*0X21*/
	DSP_BASS_BACK_SUM_OUTPUT_LEVEL(CH_LB,NEGATIVE_INFINITE); /*0X22*/
	DSP_BASS_BACK_SUM_OUTPUT_LEVEL(CH_RB,NEGATIVE_INFINITE); /*0X23*/

	DSP_BASS_SUM1_INPUT_LEVEL(CH_SW,NEGATIVE_INFINITE); /*0X26*/
	DSP_BASS_SUM2_INPUT_LEVEL(CH_SW,NEGATIVE_INFINITE); /*0X28*/
	DSP_BASS_SUM3_INPUT_LEVLE(CH_SW,NEGATIVE_INFINITE); /*0X2B*/
	DSP_BASS_SUM4_INPUT_LEVLE(CH_SW,NEGATIVE_INFINITE); /*0X2E*/ 
	DSP_BASS_SUM5_INPUT_LEVLE(CH_SW,NEGATIVE_INFINITE); /*0X36*/ 

	DSP_BASS_X_INPUT_LEVEL_FRONT(SUM1,0); /*0X37*/
	DSP_BASS_X_INPUT_LEVEL_FRONT(SUM2,0); /*0X38*/
	DSP_BASS_X_INPUT_LEVEL_FRONT(SUM3,0); /*0X39*/
	DSP_BASS_X_INPUT_LEVEL_FRONT(SUM4,0); /*0X3A*/
	DSP_BASS_X_INPUT_LEVEL_FRONT(SUM5,0); /*0X3B*/

	DSP_BASS_X_INPUT_LEVEL_BACK(SUM1,0); /*0X3C*/
	DSP_BASS_X_INPUT_LEVEL_BACK(SUM2,0); /*0X3D*/
	DSP_BASS_X_INPUT_LEVEL_BACK(SUM3,0); /*0X3E*/
	DSP_BASS_X_INPUT_LEVEL_BACK(SUM4,0); /*0X3F*/
	DSP_BASS_X_INPUT_LEVEL_BACK(SUM5,0);	/*0X40*/

	DSP_BASS_FRONT_SUM_OUTPUT_SUB_LEVEL(0.5); /*0X42*/
	DSP_BASS_BACK_SUM_OUTPUT_SUB_LEVEL(NEGATIVE_INFINITE); /*0X43*/
	DSP_BASS_CONTROL(BASS_ENABLE,SUB1_SUM_ENABLE,SUB2_SUM_ENABLE,PEAK_LIMITER_DISABLE,0); /*0x00*/
}

/*   
 * @brief: Bass Manager Enable/Disable
 *
 * @author: chenjianming
 * @date: 2007-09-20
 * @refer: AN246PPB P20
 *		 chenshiguang C11 code (BassManager.c)
 */
void BassOn(unsigned char on)
{
	if(on)
		{
		DSP_BASS_CONTROL(BASS_ENABLE,SUB1_SUM_ENABLE,SUB2_SUM_DISABLE,PEAK_LIMITER_DISABLE,0);
		}
	else
		{
		DSP_BASS_CONTROL(BASS_DISABLE,SUB1_SUM_DISABLE,SUB2_SUM_DISABLE,PEAK_LIMITER_DISABLE,0);
		}
}

/*
 * @brief: Speaker Bass Manager Setting
 *
 * @author: chenjianming
 * @date: 2007-09-20
 * @refer: chenshiguang C11 code (BassManager.c)
 */
 void SetBassManagerSpeaker(void)
{
	if(!dsp_power_flag)   /*如果DSP 没引导起来，则不实现后面的功能*/
		return ;

	if(dsp_bass_corner.fl_corner) /*设置各声道拐点频率*/
		DSP_SET_BASS_CORNER(BASS_L_HPF,dsp_bass_corner.fl_corner);	    
	if(dsp_bass_corner.cen_corner)
		DSP_SET_BASS_CORNER(BASS_C_HPF,dsp_bass_corner.cen_corner);	
	if(dsp_bass_corner.fr_corner)
		DSP_SET_BASS_CORNER(BASS_R_HPF,dsp_bass_corner.fr_corner);	
	if(dsp_bass_corner.sl_corner)
		DSP_SET_BASS_CORNER(BASS_LS_HPF,dsp_bass_corner.sl_corner);	
	if(dsp_bass_corner.sr_corner)
		DSP_SET_BASS_CORNER(BASS_RS_HPF,dsp_bass_corner.sr_corner);	
	if(dsp_bass_corner.sbl_corner)
		DSP_SET_BASS_CORNER(BASS_LB_HPF,dsp_bass_corner.sbl_corner);	
	if(dsp_bass_corner.sbr_corner)
		DSP_SET_BASS_CORNER(BASS_RB_HPF,dsp_bass_corner.sbr_corner);	

	if(dsp_bass_corner.sum1_corner)
		{
		DSP_SET_BASS_CORNER(BASS_SUM1_LPF,dsp_bass_corner.sum1_corner);
		DSP_SET_BASS_CORNER(BASS_SUM2_LPF,dsp_bass_corner.sum2_corner);
		DSP_SET_BASS_CORNER(BASS_SUM3_LPF,dsp_bass_corner.sum3_corner);
		DSP_SET_BASS_CORNER(BASS_SUM4_LPF,dsp_bass_corner.sum4_corner);
		DSP_SET_BASS_CORNER(BASS_SUM5_LPF,dsp_bass_corner.sum5_corner);	
		}

	C11SetBass(); 
}

