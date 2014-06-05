/* @file   DolbyTrueHD.c
 * @brief 	Dolby True HD Firmware Module Manager
 *
 * @author chenjianming
 * @date    2008-05-09
 * @refer   AN304DC P3
 *
 *		  Write Message=0x9000HHHH 0xhhhhhhhh
 *		  Read Request Message=0x90C0HHHH
 *		  Read Response Message=0x90CNHHHH 0xhhhhhhhh
 *		  0xHHHH=index,0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "DolbyTrueHD.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief  Dolby TureHD Control
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DC P3
 */
void DSP_TRUEHD_Control(unsigned char TrueHD_Enable)
{
	short i=0;
	unsigned char reg_value=0;

	if(TrueHD_Enable)
		reg_value=0x01;

	dsp_buf[i++] =Dolby_True_HD_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby TureHD Decode Mode Control
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DC P3
 */
void DSP_TRUEHD_Decode_Mode_Control(unsigned char channel)
{
	short i=0;
	unsigned char reg_value=0;

	switch(channel)
		{
			case decode_no_ch:
				reg_value=0x00;
				break;
			case decode_2chs:
				reg_value=0x01;
				break;
			case decode_6chs:
				reg_value=0x02;
				break;
			case decode_6and2_chs:
				reg_value=0x03;
				break;
			case decode_8_chs:
				reg_value=0x04;
				break;
			case decode_8and2_chs:
				reg_value=0x05;
				break;
		}

	dsp_buf[i++] =Dolby_True_HD_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =reg_value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief  Dolby TureHD DRC CONTROL
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DC P3
 */
void DSP_TRUEHD_DRC_Control(void)
{
	short i=0;

	dsp_buf[i++] =Dolby_True_HD_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x02;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief  Dolby TureHD DRC CUT SCALEFACTOR
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DC P3
 */
void DSP_TRUEHD_Cut_Scalefactor(unsigned long value)
{
	short i=0;

	dsp_buf[i++] =Dolby_True_HD_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x03;
	dsp_buf[i++] =(value >> 24) & 0xFF;	dsp_buf[i++] =(value >> 16) & 0xFF;	dsp_buf[i++] =(value >> 8) & 0xFF;	dsp_buf[i++] =value  & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief  Dolby TureHD DRC BOOST SCALEFACTOR
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer AN304DC P3
 */
void DSP_TRUEHD_Boost_Scalefactor(unsigned long value)
{
	short i=0;

	dsp_buf[i++] =Dolby_True_HD_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x04;
	dsp_buf[i++] =(value >> 24) & 0xFF;	dsp_buf[i++] =(value >> 16) & 0xFF;	dsp_buf[i++] =(value >> 8) & 0xFF;	dsp_buf[i++] =value  & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief  TRUEHD dynamics compress 设置为最大
 *
 * @author chenjianming
 * @date 2009-02-12
 * @refer 
 */
void DSP_TRUEHD_DRC_INIT(void)
{
	DSP_TRUEHD_DRC_Control();
	DSP_TRUEHD_Cut_Scalefactor(0x00);
	DSP_TRUEHD_Boost_Scalefactor(0x00);
}

/*
 * @brief  读取Dolby TRUEHD固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0x90C0HHHH
 * 		 Read Response Message =0x90CNHHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2008-05-09
 * @refer chenshiguang C11 code
 */
unsigned long DspReadTRUEHD(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0x9D;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i); 
	return ReadDsp();
}

