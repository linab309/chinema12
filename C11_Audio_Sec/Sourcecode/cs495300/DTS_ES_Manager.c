/* @file   DTS_ES_Manager.c
 * @brief DTS_ES Manager File
 *
 * @author chenjianming
 * @date 2007-1-9
 * @refer AN246DC
 */

#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "DTS_ES_Manager.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/* @file  
 * @brief DTS_ES 使能控制
 *
 * @author chenjianming
 * @date 2007-1-9
 * @refer AN246DC
 *           由于DTS_ES是基于DTS固件,所以要使能DTS_ES,也必须使能DTS.
 *		所以bit0 必须为1声能DTS
 */
void DTS_ES_Control(unsigned char Matrix_6dot1,unsigned char Discrete_6dot1_Enalbe)
{
	short i=0;
	unsigned char reg_value;

	switch(Matrix_6dot1)
		{
			case DTS_ES_Matrix_6dot1_OFF:
				reg_value=0x00;
				break;
			case DTS_ES_Matrix_6dot1_ON:
				reg_value=0x20;
				break;
			case DTS_ES_Matrix_6dot1_Autodetect:
				reg_value=0x40;
				break;
			default:
				reg_value=0x00;
				break;
		}

	if(Discrete_6dot1_Enalbe)
		reg_value |=0x10; 

	dsp_buf[i++]=DTS_ES_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=reg_value;	dsp_buf[i++]=0x01; 
	
	DspSpiWriteNByte(dsp_buf,i);  
}

