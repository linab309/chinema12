/* @file   DolbyHeadphone.c
 * @brief 	Dolby Headphone Firmware Module Manager
 *
 * @author chenjianming
 * @date    2008-09-04
 * @refer   AN246MPK P3
 *
 *		  Write Message=0xC000HHHH 0xhhhhhhhh
 *		  Read Request Message=0x40C0HHHH
 *		  Read Response Message=0xC0C0HHHH 0xhhhhhhhh
 *		  0xHHHH=index,0xhhhhhhhh=data value
 */
#include "Dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "DolbyHeadphone.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief  DolbyHeadpone Control
 *
 * @author chenjianming
 * @date 2008-09-04
 * @refer AN246MPK P3
 */
void DH2_ENABLE(unsigned char DH2_Enable)
{
	short i=0;
	unsigned char reg_value;

	if(DH2_Enable)
		reg_value=0x01;
	else
		reg_value=0x00;

	dsp_buf[i++] =Dolby_Headphone_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  DolbyHeadpone Room Select
 *
 * @author chenjianming
 * @date 2008-09-04
 * @refer AN246MPK P3
 * @note Dolby Headphone mode
 */
void DH2_ROOM_SELECT(unsigned char DH2_Select)
{
	short i=0;
	unsigned char reg_value;

	if(DH2_Select==1)
		reg_value=0x01;
	else
		reg_value=0x00;

	dsp_buf[i++] =Dolby_Headphone_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  DolbyHeadpone Heap Mode
 *
 * @author chenjianming
 * @date 2008-09-04
 * @refer AN246MPK P3
 * @note when the stream is DolbyTrueHD most select mode1.
 */
void DH2_HEAP_MODE(unsigned char mode)
{
	short i=0;
	unsigned char reg_value;

	if(mode)
		reg_value=0x01;
	else
		reg_value=0x00;

	dsp_buf[i++] =Dolby_Headphone_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x35;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);	
}

