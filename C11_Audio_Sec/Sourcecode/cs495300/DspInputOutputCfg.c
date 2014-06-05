/**
 * @file DspInputOutputCfg.c
 * @brief 此文件用于DSP 的输入和输出配置函数
 *
 * @author chenjianming
 * @date 2007-06-27
 * @version 1.0
 * @refer
 *	input: CS4953xx_hw_user_guide P66
 *	output:CS4953xx_hw_user_guide P77
 */
#include "DspInputOutputCfg.h"
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "..\main\debug.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/************************Input Configuration************************************/
#if 0
/*
 * @brief DSP configuration Input A (I2S)
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p67
 */
void DSP_Cfg_Input_I2s(void)
{
	short i=0;
	
	dsp_buf[i++] =0x81; dsp_buf[i++] =0x80; dsp_buf[i++] =0x00; dsp_buf[i++] =0x20; 
	dsp_buf[i++] =0xFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; 		
	dsp_buf[i++] =0X01; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X21;		
	dsp_buf[i++] =0xFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X21; 	
	dsp_buf[i++] =0X01; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00; 
		
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X22;   		
	dsp_buf[i++] =0xFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X22; 		
	dsp_buf[i++] =0X01; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X23; 		
	dsp_buf[i++] =0xFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X23;  		
       dsp_buf[i++] =0X01; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00;
	       
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X24; 		
	dsp_buf[i++] =0xFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X24; 		
       dsp_buf[i++] =0X01; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00; 

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration Input A (Left Justify)
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p67
 */
void DSP_Cfg_Input_Left_Justify(void)
{
	short i=0;

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; 
	dsp_buf[i++] =0XFE; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00;  
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X21;  
	dsp_buf[i++] =0XFE; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X21;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X22;  
	dsp_buf[i++] =0XFE; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X22;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X23; 
	dsp_buf[i++] =0XFE; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X23;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X24;  
	dsp_buf[i++] =0XFE; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 	

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X24;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X1F; dsp_buf[i++] =0X00;
	
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration Input A
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p67 
 */
void DSP_Cfg_Input_Data_Format(int type)
{
	switch(type)
		{		
		case 1:
			DSP_Cfg_Input_Left_Justify();
			break;
		case 0:
		default:	
			DSP_Cfg_Input_I2s();
			break;
		}
}

/*
 * @brief DSP configuration input Data Clocked in on SCLK Rising Edge
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p68
 */
void DSP_Cfg_input_SCLK_Rising_Edge(void)
{
	short i=0;

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X21;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF;  
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X22;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X23;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF;  
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X24;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF;  
	
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration input Data Clocked in on SCLK Falling Edge
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p68
 */
void DSP_Cfg_input_SCLK_Falling_Edge(void)
{
	short i=0;

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X21;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X22;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X23;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X24;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  
	
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration Input B
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p68
 */
void DSP_Cfg_Input_SCLK_Polarity(int Polarity)
{
	switch(Polarity)
		{
		case 1:
			DSP_Cfg_input_SCLK_Falling_Edge();
			break;
		case 0:
		default:
			DSP_Cfg_input_SCLK_Rising_Edge();
			break;	
		}
}

/*
 * @brief DSP Configuration LRCLK (Low indicates Channel 0--i.e.Left)
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p68
 */
void DSP_Cfg_Input_LRCLK_Left_Polarity(void)
{
	short i=0;

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X21;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X22;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X23;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X24;  
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF;  
	
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP Configuration LRCLK (Low indicates Channel 0--i.e.Right)
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p68
 */
void DSP_Cfg_Input_LRCLK_Right_Polarity(void)
{
	short i=0;

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  //

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X21;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  //

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X22;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  //

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X23;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  //

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X24;  
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  //

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration Input C
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide p68
 */
void DSP_Cfg_Input_LRCLK_Polarity(int Polarity)
{
	switch(Polarity)
		{
		case 1:
			DSP_Cfg_Input_LRCLK_Right_Polarity();
			break;
		case 0:
		default:
			DSP_Cfg_Input_LRCLK_Left_Polarity();
			break;	
		}
}

/*
 * @brief DSP Configuration DAI2_LRCLK/SCLK_Slave Compressed Data in on DAI_D4
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide P69
 */
void DSP_Cfg_Input_On_DAI_D4(void)
{
	short i=0;

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X25;  
	dsp_buf[i++] =0X10; dsp_buf[i++] =0X08; dsp_buf[i++] =0Xd1; dsp_buf[i++] =0X10;  

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP Configuration DAI1_LRCLK/SCLK_Slave Compressed Data in on DAI_D0
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide P69
 */
void DSP_Cfg_Input_On_DAI_D0(void)
{
	short i=0;

	dsp_buf[i++] =0x81; dsp_buf[i++] =0x00; dsp_buf[i++] =0x00; dsp_buf[i++] =0x25;  
	dsp_buf[i++] =0x00; dsp_buf[i++] =0x00; dsp_buf[i++] =0xd1; dsp_buf[i++] =0x90;  

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration Input D
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer CS4953xx_hw_user_guide P69
 */
void DSP_Cfg_Input_DAI_Mode(int mode)
{
	switch(mode)
		{
		case 1:
			DSP_Cfg_Input_On_DAI_D0();
			break;		
		case 0:
		default:
			DSP_Cfg_Input_On_DAI_D4();
			break;
		}
}
#endif

/******************************OutputConfiguration*******************************/
#if 0 
/*
 * @brief DSP configuraton output clock mode0
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Clock_Mode0(void)
{
	short i=0; 
	
	dsp_buf[i++] =0x81; dsp_buf[i++] =0x40; dsp_buf[i++] =0x00; dsp_buf[i++] =0x2C; 
	dsp_buf[i++] =0x00; dsp_buf[i++] =0x00; dsp_buf[i++] =0x20; dsp_buf[i++] =0x00; 

	dsp_buf[i++] =0x81; dsp_buf[i++] =0x00; dsp_buf[i++] =0x00; dsp_buf[i++] =0x2D; 
	dsp_buf[i++] =0x00; dsp_buf[i++] =0x00; dsp_buf[i++] =0x20; dsp_buf[i++] =0x00; 

	DspSpiWriteNByte(dsp_buf,i); 
}

/*
 * @brief DSP configuraton output clock mode1
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Clock_Mode1(void)
{
	short i=0;
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2D; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; 

	DspSpiWriteNByte(dsp_buf,i); 	
}

/*
 * @brief DSP configuration output A
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Clock_Mode(int mode)
{
	switch(mode)
		{
		case 1: 
			DSP_Cfg_Output_Clock_Mode1();
			break;		
		case 0:
		default:
			DSP_Cfg_Output_Clock_Mode0();
			break;
		}
}

/*
 * @brief DSP configuration output clock relationship0
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Clock_Relationship0(void)
{
	short i=0; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2B; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; dsp_buf[i++] =0X00; 

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output clock relationship1
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Clock_Relationship1(void)
{
	short i=0;
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2B; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XDF; dsp_buf[i++] =0XFF; 
 
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output B
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Clock_Relationship(int Relation)
{	
	switch(Relation)
		{
		case 1:
			DSP_Cfg_Output_Clock_Relationship1();
			break;		
		case 0:
		default:
			DSP_Cfg_Output_Clock_Relationship0();
			break;
		}
}

/*
 * @brief DSP configuration output Frequency Ratio
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_SCLK_Freq0(void)
{
	short i=0; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X3D; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X77; dsp_buf[i++] =0X11; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X3E; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X77; dsp_buf[i++] =0X11; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X8F; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; 

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output Frequency Ratio
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_SCLK_Freq1(void)
{
	short i=0; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X3D; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X77; dsp_buf[i++] =0X01; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X3E; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X77; dsp_buf[i++] =0X01; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0X8F; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X20; 
 
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output C
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Clock_Frequence(int Fre)
{
	switch(Fre)
		{
		case 1:
			DSP_Cfg_Output_SCLK_Freq1();
			break;		
		case 0:
		default:
			DSP_Cfg_Output_SCLK_Freq0();
			break;
		}
}

/*
 * @brief DSP configuration output data format _I2S
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_I2S(void)
{
	short i=0; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X30; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X01; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X31; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X01; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X32; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X01; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X33; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X01; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X34; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X01;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X35; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X01;  
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X36; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X01; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X37; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X01; 

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output data format _Left_Justify
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Left_Justify(void)
{
	short i=0; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X30; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X31; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X32; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X33; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X34; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00;  

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X35; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 
	
	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X36; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X37; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; 
	 
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output D
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_Data_Format(int Format)
{
	switch(Format)
		{
		case 1:
			DSP_Cfg_Output_Left_Justify();
			break;		
		case 0:
		default:
			DSP_Cfg_Output_I2S();
			break;
		}
}

/*
 * @brief DSP configuration output LRCLK polarity 0
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_LRCLK_Polarity0(void)
{
	short i=0; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X07; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2D; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X07; dsp_buf[i++] =0X00; 

	DspSpiWriteNByte(dsp_buf,i); 
 }

/*
 * @brief DSP configuration output LRCLK polarity 1
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_LRCLK_Polarity1(void)
{
	short i=0; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFB; dsp_buf[i++] =0XFF; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X03; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2D; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFB; dsp_buf[i++] =0XFF; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2D; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X03; dsp_buf[i++] =0X00; 
 
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output E
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_LRCLK_Polarity(int Polarity)
{
	switch(Polarity)
		{
		case 1:
			DSP_Cfg_Output_LRCLK_Polarity1();
			break;		
		case 0:
		default:
			DSP_Cfg_Output_LRCLK_Polarity0();
			break;
		}
}

/*
 * @brief DSP configuration output sclk polarity 0
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_SCLK_Polarity0(void)
{
	short i=0; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XEF; dsp_buf[i++] =0XFF; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X80; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2D; 
	dsp_buf[i++] =0XFF; dsp_buf[i++] =0XFF; dsp_buf[i++] =0XEF; dsp_buf[i++] =0XFF; 

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output sclk polarity 1
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_SCLK_Polarity1(void)
{
	short i=0;

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2C; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X10; dsp_buf[i++] =0X00; 

	dsp_buf[i++] =0X81; dsp_buf[i++] =0X40; dsp_buf[i++] =0X00; dsp_buf[i++] =0X2D; 
	dsp_buf[i++] =0X00; dsp_buf[i++] =0X00; dsp_buf[i++] =0X10; dsp_buf[i++] =0X00; 

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP configuration output F
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p77
 */
void DSP_Cfg_Output_SCLK_Polarity(int Polarity)
{
	switch(Polarity)
		{
		case 1:
			DSP_Cfg_Output_SCLK_Polarity1();
			break;		
		case 0:
		default:
			DSP_Cfg_Output_SCLK_Polarity0();
			break;
		}
}
#endif

#if 0
/*
 * @brief audio port configuration (input an output)
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet p66 (input)
 *            cs4953xx_datasheet p77 (output)
 *            The HostControlledMasterBoot Flow Need
 */
void DSPInputOutputCfg(unsigned char channel_mode)
{
	/*Input Configuration*/ 
//	DSP_Cfg_Input_Data_Format(0);            /*Input_A  default 	I2S-24bit*/ 
//	DSP_Cfg_Input_SCLK_Polarity(0);          /*Input_B  default	Raising*/
//	DSP_Cfg_Input_LRCLK_Polarity(0);         /*Input_C default	LRCK low Indicate Left Channel*/
	if(channel_mode==Analog_INPUT_2ch)
		DSP_Cfg_Input_DAI_Mode(1);    /*Input_D 	DAI_D0*/
	else
		DSP_Cfg_Input_DAI_Mode(0);    /*Input_D 	DAI_D4*/

	/*Output Configuration*/	 	
	ContextSwitchDSPB();			    /*Context to DSPB (refer cs4953xx_hw_user_guide p77)*/	
	DSP_Cfg_Output_Clock_Mode(1);  	    /*DAO_A	Master mode to generate audio clock*/	  	
//	DSP_Cfg_Output_Clock_Relationship(0);  /*DAO_B   default	DAO2 dependent on DAO1 clocks	*/
//	DSP_Cfg_Output_Clock_Frequence(0);	    /*DAO_C   default	<96K */		
//	DSP_Cfg_Output_Data_Format(0);  	    /*DAO_D   default	I2S-32bit */
//	DSP_Cfg_Output_LRCLK_Polarity(0); 	    /*DAO_E    default	LRCK low Indicate Left Channel */
//	DSP_Cfg_Output_SCLK_Polarity(0);  	    /*DAO_F    default	Raising */	
}
#endif
