/*@file	 	Audio Manager File
 * @brief  	Audio Manager Firmware Driver File
 *
 * @author 	chenjianming
 * @date 		2007-09-25
 * @refer 	AN288 P25
 *
 * 			Write Command=0x8300HHHH 0xhhhhhhhh
 * 			Read Request Command=0x83c0HHHH
 * 			Read Response Message=0x03c0HHHH 0xhhhhhhhh
 *
 * 			0xHHHH=index	0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "..\main\Debug.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief: DSP Audio Mute Control
 *
 * @author: chenjianming
 * @date: 2007-09-24
 * @refer: AN288 p25
 */
void DSPMute(unsigned char mode)
{
	if(dsp_power_flag) 
	{
		if(mode)
		{
#ifdef _DEBUG
			debug_printf("\nDSPMute11111111111111111");
#endif
			DSPFirmwareWrite(Audio_Manager_Wop,0x0001,0x00000001); /*mute*/
		}
		else
		{
#ifdef _DEBUG
			debug_printf("\nDSPMute000000000000000000");
#endif
			DSPFirmwareWrite(Audio_Manager_Wop,0x0001,0x00000000); /*unmute*/
		}
	}
}

/* 
 * @brief: channel remap
 *
 * @author: chenjianming
 * @date: 2007-09-06
 * @refer: 在默认情况下,通道映射刚好跟硬件匹配,故不需要设置.
 *
 *		 在495中，其默认的通道如下：
 *		 DA01_Data3	DA01_Data2	DA01_Data1	DA01_Data0
 *			↓			↓			↓			↓
 *		 SBL 和SBR	CEN 和LFE	  SL 和SR	    L 和R
 */
void ConfigChannelRemap(void)
{
	DSPFirmwareWrite(Audio_Manager_Wop,0x0012,0x00000000); //L	
	DSPFirmwareWrite(Audio_Manager_Wop,0x0013,0x00000001); //CEN
	DSPFirmwareWrite(Audio_Manager_Wop,0x0014,0x00000002); //R	
	DSPFirmwareWrite(Audio_Manager_Wop,0x0015,0x00000003); //LS,SL
	DSPFirmwareWrite(Audio_Manager_Wop,0x0016,0x00000004); //RS,SR	
	DSPFirmwareWrite(Audio_Manager_Wop,0x0017,0x00000005); //LB,LSB
	DSPFirmwareWrite(Audio_Manager_Wop,0x0018,0x00000006); //RB,SBR
	DSPFirmwareWrite(Audio_Manager_Wop,0x0019,0x00000007); //LFE
}

/*
 * @brief  读取Audio Manger固件中寄存器的值
 *
 *		 先向DSP 发送一个 Read Request Message 后，DSP回返回一个Read Response Message
 *		 Read Request Message=0x83c0HHHH
 * 		 Read Response Message =0x03c0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
unsigned long DspReadAudioManger(unsigned char index)
{
	short i=0;

	dsp_buf[i++] =0x83;	dsp_buf[i++] =0xc0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}

