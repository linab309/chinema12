/* @file   Irc2_Manager.c
 * @brief Irc2 Firmware Manager
 *
 * @author chenjianming
 * @date 2007-11-14
 * @refer AN246PPJ
 *
 *		Write=0x9D00HHHH 0xhhhhhhhh
 *		Read Request=0x9DC0HHHH
 *		Read Response=0x1D0HHHH 0xhhhhhhhh
 *
 *		0xHHHH=index	0xhhhhhhhh=data value
 */

#include "Dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "..\main\Macro.h"
#include "..\main\Debug.h"

#ifdef _DEBUG
//#define _DEBUG_IRC_2_MANAGER
#endif

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief  IRC2 Control
 *
 * @author chenjianming
 * @date 2007-11-16
 * @note Bit16 0-Enable 1-Disable 默认为0 Enable passthrough ,
 *           必须Disable,否则输出的数据又重新输入到DSP,形成正反馈,影响IRC测试。
 *           即麦克风接收的声音会从DSP输出.
 *           在实际的IRC测试过程中,必须把它关掉,否则会影响测试结果。
 */
void Dsp_IRC2_Control(unsigned char value)
{	
	short i=0;

	dsp_buf[i++]=0x9D;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x01;	dsp_buf[i++]=0x00;	dsp_buf[i++]=value;

	DspSpiWriteNByte(dsp_buf,i);		
}

/*
 * @brief  IRC2 Speaker Select
 *
 * @author chenjianming    
 * @date 2007-11-16
 * @note
 */
 void Dsp_IRC2_Speaker_Select(unsigned char value,unsigned char eq_enable)
{
	short i=0;

	dsp_buf[i++]=0x9D;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x01;
	dsp_buf[i++]=0x00;
	if(eq_enable)
		dsp_buf[i++]=0x01;
	else
		dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=value;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief  IRC2 Speaker Select
 *
 * @author chenjianming
 * @date 2007-11-16
 * @note
 */
 void Dsp_IRC2_Max_EQ_Fc(unsigned char value)
{
	short i=0;

	dsp_buf[i++]=0x9D;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x04;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=value;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief 读取IRC-2 中固件寄存器的值 
 *
 * @author chenjianming
 * @date 2007-11-16
 * @note 要读取固件寄存器的值,必须先发送读请求命令再读取DSP 的返回信息
 */
unsigned long DspReadIrc2(unsigned char index)
{
	short i=0;

	dsp_buf[i++]=0x9D;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x00;	dsp_buf[i++]=index;

	DspSpiWriteNByte(dsp_buf,i);	/*发送读请求命令*/
	return ReadDsp();
}

/* 
 * @brief  判断IRC 反馈的信息是否有效
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer chenshiguang C11 code
 * @note  在IRC 中当通道分析完毕,固件会把使能标志位(低4位)清掉--表示
 *            IRC测量已完成且测量分析的结果可以被读取了。
 */
int IsIrc2ValidResult(void)
{
	if(DspReadIrc2(0x00)&0x000f)         
		return FALSE;  
	return TRUE;
}

