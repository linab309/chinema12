/* @file   Irc1_Manager.c
 * @brief  Irc1 Firmware Manager
 *
 * @author chenjianming
 * @date 2007-11-14
 * @refer AN246PPI
 *
 *		Write=0x9C00HHHH 0xhhhhhhhh
 *		Read Request=0x9CC0HHHH;
 *		Read Response=0x1C0HHHH 0xhhhhhhhh
 *
 *           0xHHHH=index 0xhhhhhhhh=data value
 */
 #include "Dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "..\main\Macro.h"
 #include "..\main\Debug.h"

 #ifdef _DEBUG
//#define _DEBUG_IRC_1_MANAGER
#endif

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

 long Convert_8_24_Value(float in_value)
{
	unsigned long rtn_value;
	
	rtn_value = in_value * POW_2_24;
	return rtn_value;
}

struct MIC_PARA{
unsigned char addr;
float db_value;
};


#if 0
const struct MIC_PARA MIC_YOGE_BM38_parameter[] = 
{
	{0xA0,15},	/* 0xA0 20 Hz */
	{0xA1,14},	/* 0xA1 25 Hz */
	{0xA2,13},	/* 0xA2 31.5 Hz */
	{0xA3,12},	/* 0xA3 40 Hz */
	{0xA4,12},	/* 0xA4 50 Hz */
	{0xA5,12},	/* 0xA5 63 Hz */
	{0xA6,12},	/* 0xA6 80 Hz */
	{0xA7,11},	/* 0xA7 100 Hz */
	{0xA8,10},	/* 0xA8 125 Hz */
	{0xA9,10},	/* 0xA9 160 Hz */
	{0xAA,9},	/* 0xAA 200 Hz */
	{0xAB,9},	/* 0xAB 250 Hz */
	{0xAC,7.5},	/* 0xAC 315 Hz */
	{0xAD,7},	/* 0xAD 400 Hz */
	{0xAE,6.5},	/* 0xAE 500 Hz */
	{0xAF,6},	/* 0xAF 630 Hz */
	{0xB0,5},	/* 0xB0 800 Hz */
	{0xB1,5},	/* 0xB1 1000 Hz */
	{0xB2,5},	/* 0xB2 1250 Hz */
	{0xB3,5},	/* 0xB3 1600 Hz */
	{0xB4,4},	/* 0xB4 2000 Hz */
	{0xB5,3.5},	/* 0xB5 2500 Hz */
	{0xB6,3},	/* 0xB6 3150 Hz */
	{0xB7,2},	/* 0xB7 4000 Hz */
	{0xB8,1.5},	/* 0xB8 5000 Hz */
	{0xB9,1},	/* 0xB9 6300 Hz */
	{0xBA,0},	/* 0xBA 8000 Hz */
	{0xBB,2},	/* 0xBB 10000 Hz */
	{0xBC,-2},	/* 0xBC 12500 Hz */
	{0xBD,-2},	/* 0xBD 16000 Hz */
	{0xBE,1.7},	/* 0xBE 20000 Hz */
	{0,0},
};

40Hz -2dB
50Hz -1dB
63Hz -1dB
80Hz -1dB
3150Hz -3dB
4000Hz +2dB
5000Hz -6dB
6300Hz -4dB
8000Hz -8dB
10000Hz -4dB
12500Hz -2dB
16000Hz -6dB
#else
	const struct MIC_PARA MIC_YOGE_BM38_parameter[] = 
{
	{0xA0,0},	/* 0xA0 20 Hz */
	{0xA1,0},	/* 0xA1 25 Hz */
	{0xA2,0},	/* 0xA2 31.5 Hz */
	{0xA3,-2},	/* 0xA3 40 Hz */
	{0xA4,-1},	/* 0xA4 50 Hz */
	{0xA5,-1},	/* 0xA5 63 Hz */
	{0xA6,0},	/* 0xA6 80 Hz */
	{0xA7,0},	/* 0xA7 100 Hz */
	{0xA8,0},	/* 0xA8 125 Hz */
	{0xA9,0},	/* 0xA9 160 Hz */
	{0xAA,0},	/* 0xAA 200 Hz */
	{0xAB,0},	/* 0xAB 250 Hz */
	{0xAC,0},	/* 0xAC 315 Hz */
	{0xAD,0},	/* 0xAD 400 Hz */
	{0xAE,0},	/* 0xAE 500 Hz */
	{0xAF,0},	/* 0xAF 630 Hz */
	{0xB0,0},	/* 0xB0 800 Hz */
	{0xB1,0},	/* 0xB1 1000 Hz */
	{0xB2,0},	/* 0xB2 1250 Hz */
	{0xB3,0},	/* 0xB3 1600 Hz */
	{0xB4,0},	/* 0xB4 2000 Hz */
	{0xB5,0},	/* 0xB5 2500 Hz */
	{0xB6,-3},	/* 0xB6 3150 Hz */
	{0xB7,2},	/* 0xB7 4000 Hz */
	{0xB8,-6},	/* 0xB8 5000 Hz */
	{0xB9,-4},	/* 0xB9 6300 Hz */
	{0xBA,-8},	/* 0xBA 8000 Hz */
	{0xBB,-4},	/* 0xBB 10000 Hz */
	{0xBC,-2},	/* 0xBC 12500 Hz */
	{0xBD,-6},	/* 0xBD 16000 Hz */
	{0xBE,0},	/* 0xBE 20000 Hz */
	{0,0},
};
#endif

/*
 * @brief  设置麦克风增益参数(compensation for various microphone )
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer chenshiguang C11 code
 *            
 */
void InitMicParameter(void)
{
	unsigned char i;
	unsigned long set_value;

	for(i=0;MIC_YOGE_BM38_parameter[i].addr != 0;i++)
		{
		set_value = Convert_8_24_Value(MIC_YOGE_BM38_parameter[i].db_value);
		DSPFirmwareWrite(IRC_1_Manager_Wop,MIC_YOGE_BM38_parameter[i].addr,set_value);
		}
}


/* 
 * @brief IRC-1模块使能控制
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer AN246PPI 
 */
void DSP_IRC_1_Control(unsigned char enable)
{
	short i=0;
	unsigned char value;

	if(enable)
		value=0x01;
	else
		value=0x00;

	dsp_buf[i++]=0x9C;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=value;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief 设置麦克风数据输入的通道 
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer AN246PPI  
 */
void DSP_Rx_Channel(unsigned char channel)
{
	short i=0;

	dsp_buf[i++]=0x9C;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x01;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=channel;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief Gain Setting
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer AN246PPI
 */
void DSP_Rx_Gain(void)
{
	short i=0;

	dsp_buf[i++]=0x9C;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x02;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x80;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;

	DspSpiWriteNByte(dsp_buf,i);                  
}

/*  
 * @brief Setting channels to transmit test signal
 *
 * @author chenjianming
 * @date 2008-08-15
 * @refer 
 */
void DSP_IRC1_Tx_Channel(void)
{
	short i=0;

	dsp_buf[i++]=0x9C;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x03;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;

	DspSpiWriteNByte(dsp_buf,i);
}

/*  
 * @brief  number of seconds to run test
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer AN246PPI
 */
void DSP_IRC1_Test_Duration(unsigned char second)
{
	short i=0;

	dsp_buf[i++]=0x9C;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x06;
	dsp_buf[i++]=second;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;

	DspSpiWriteNByte(dsp_buf,i);
}

/*  
 * @brief  Test Noise to use
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer AN246PPI
 */
void DSP_IRC_1_Test_Signal(unsigned char value)
{
	short i=0;

	dsp_buf[i++]=0x9C;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x07;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=value;
	
	DspSpiWriteNByte(dsp_buf,i);		
}

/*
 * @brief 读取IRC-1 中固件寄存器的值 
 *
 * @author chenjianming
 * @date 2007-11-16
 * @note 要读取固件寄存器的值,必须先发送读请求命令再读取DSP 的返回信息
 */
unsigned long DspReadIrc1(unsigned char index)
{
	short i=0;
	
	dsp_buf[i++]=0x9C;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x00;	dsp_buf[i++]=index;

	DspSpiWriteNByte(dsp_buf,i);	//发送读请求命令
	return ReadDsp();
}

/*  
 * @brief  Read Distance
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer AN246PPI
 */
unsigned short GetSpkDistance(void)
{
	return DspReadIrc1(0x18) & 0xFFFF;	/*get speaker distance*/
}

/* 
 * @brief  判断IRC 反馈的信息是否有效
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer chenshiguang C11 code
 * @note  在IRC 中当通道分析完毕,固件会把使能标志位清掉--表示IRC测量已完成且测量分析的结果可以被读取了
 */
int IsIrc1ValidResult(void)
{
	if(DspReadIrc1(0x00))      
		return FALSE;  
	return TRUE;
}

