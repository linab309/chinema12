/* @file   DolbyPrologicII.C (take 2 channels in and 5 channels out)
 * @brief Dolby Pro Logic II Module Manager
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246MPM P3
 *
 * Write Message=0xB200HHHH 0xhhhhhhhh
 * Read Request Message=0xB2C0HHHH
 * Read Response Message=0x32C0HHHH 0xhhhhhhhh
 *
 * 0xHHHH=index, 0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "DolbyPrologicII.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief  Dolby ProLogic II Enable
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246MPM P3
 */
void DSP_PLII_Control(unsigned char enable)
{
	short i=0;
	unsigned char value=0;
	
	if(enable)
		value=0x01;		
	else
		value=0x00;

	dsp_buf[i++] =Dolby_Pro_LogicII_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief PLII DECODE MODE Setting
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246MPM P4
 */
void DSP_PLII_DECODE_MODE(unsigned char mode)
{
	short i=0;
	unsigned char Decode_Mode;

	switch(mode)
		{
			case Pro_Logic_Emulation_Mode:		Decode_Mode=0x00;		break;
			case Virtual_Mode:				Decode_Mode=0x01;		break;
			case Music_Mode:					Decode_Mode=0x02;		break;
			case Movie_Mode:					Decode_Mode=0x03;		break;
			case Matrix_Mode:					Decode_Mode=0x04;		break;
			case Custom_Mode:				Decode_Mode=0x05;		break;
		}

	dsp_buf[i++] =Dolby_Pro_LogicII_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =Decode_Mode;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief PLII Control Register (Only for Panorama Enable)
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246MPM P3
 * @note Only Custom and Music Mode
 */
void DSP_PLII_CONTROL_REGISTER(unsigned char Panorama_Enable)
{
	short i=0;
	unsigned char value;
	
	if(Panorama_Enable)
		value=0x04;
	else 
		value=0x00;

	dsp_buf[i++] =Dolby_Pro_LogicII_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief Pro Logic II Center Width Configuration 
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246MPM P3
 * @note  只有在Custom and Music Mode 起作用
 */
void DSP_PLII_CENTER_WIDTH_CONFIGURATION(unsigned char Width)
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

	dsp_buf[i++] =Dolby_Pro_LogicII_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x03;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);	
}

/* 
 * @brief Pro Logic II DIMENSION CONFIGURATION
 *
 * @author chenjianming
 * @date 2007-09-25
 * @refer AN246MPM P4
 * @note  只有在Custom and Music Mode 起作用
 */
void DSP_PLII_DIMENSION_CONFIGURATION(unsigned char Dimension)
{
	short i=0;
	short value;

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

	dsp_buf[i++] =Dolby_Pro_LogicII_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x04;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);
}

/* 
 * @brief Pro Logic II firmware setting
 *
 * @author chenjianming
 * @date 2007-10-11
 * @refer 
 */
 void DSP_PLII_Firmware_Cfg(void)
{
	DSP_PLII_Control(Enable);
	DSP_PLII_DECODE_MODE(2); /*Music Mode*/
	//DSP_PLII_CENTER_WIDTH_CONFIGURATION(7);
	//DSP_PLII_DIMENSION_CONFIGURATION(6);
}

