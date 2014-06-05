/* @file   	DelayManager.c
 * @brief 		Delay Manager Driver File
 *
 * @author 	chenjianming
 * @date 		2007-11-1
 * @refer 	AN246PPA
 *
 *			Write=0xD900HHHH 0xhhhhhhhh
 *			Read Request=0xD9C0HHHH
 *			Read Response=0x59C0HHHH 0xhhhhhhhh
 *
 *			0xHHHH=index 0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "DelayManager.h"
#include "..\driver\Dsp_Spi.h"
#include "..\driver\Eeprom.h"
#include "..\main\debug.h"

#ifdef _DEBUG
#define _DEBUG_Delay_MANAGER
#endif

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

_DELAY_TIME dsp_delay_time = {0};
_DELAY_TIME dsp_delay_music_time={0};
/*
 * @brief Delay 模块使能控制
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA P3
 * @note  在DataSheet 中Bit0 默认为Enable,但实际上是Disable.
 *		 当使能External_Memory_Enable =1 时,这时使用的是外部SDRAM 做为Delay Buffer.
 *		 当为0 时则使用的是内部的Memory 做为Delay Buffer.
 *		 
 *		内部的Memory 最大可以Delay 115 ms,外部的根据SDRAM的大小而定.
 */
void DSP_MAIN_PPM_CONTROL(unsigned char Delay_PPM_Master_Enable,unsigned char MidProc_Delay_Offset_Enable,unsigned char Externnal_Memory_Enable)
{
	short i=0;
	unsigned char value1=0,value2=0;

	if(Delay_PPM_Master_Enable)
		value1|=0x01;
	if(MidProc_Delay_Offset_Enable)
		value1|=0x10;
	if(Externnal_Memory_Enable)  
		value2|=0x01;

	dsp_buf[i++] =Delay_Manager_Wop;  dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value2;	dsp_buf[i++] =value1;

	DspSpiWriteNByte(dsp_buf,i);   
}

/*
 * @brief  Delay Unit Select (用默认设置Millisenconds)
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer  AN246PPA 
 */
void DSP_Delay_Unit(unsigned char Dly_Unit_Select)
{
	short i=0;
	unsigned char value;
	
	if(Dly_Unit_Select)
		value=0x01;
	else
		value=0x00;

	dsp_buf[i++] =Delay_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i); 	
}

/*
 * @brief Channels Delay time setting
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA P3
 */
 #define MS_UNIT 0x20000	/*0x20000 = 2^17, 15.17 format*/ /*延时1ms 寄存器设置的值*/
void DSP_Delay_Channel_Setting(Dsp_Channel channel,unsigned long Delay_Ms)
{
	short i=0;
	unsigned long time;
	unsigned char index_value;

	time = Delay_Ms * MS_UNIT;

	switch(channel)
		{
			case CH_L:		index_value=0x02;			break;
			case CH_C:		index_value=0x03;			break;
			case CH_R:		index_value=0x04;			break;
			case CH_LS:		index_value=0x05;			break;
			case CH_RS:		index_value=0x06;			break;
			case CH_LB:		index_value=0x07;			break;
			case CH_RB:		index_value=0x08;			break;
			case CH_SW:		index_value=0x09;			break;
		}

	dsp_buf[i++] =Delay_Manager_Wop;  dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;		
	dsp_buf[i++] =(time>>24&0xFF);	dsp_buf[i++] =(time>>16&0xFF);	dsp_buf[i++] =(time>>8&0xFF);	dsp_buf[i++] =(time&0xFF);		

	DspSpiWriteNByte(dsp_buf,i);  		
}

/* 
 * @brief  Channels Max Delay Time Setting 
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA P3
 */
void DSP_Delay_Max_Setting(Dsp_Channel channel,unsigned long Max_Delay_Time)
{
	short i=0;
	unsigned char index_value;

	switch(channel)
		{
			case CH_L:		index_value=0x0a;			break;
			case CH_C:		index_value=0x0b;			break;
			case CH_R:		index_value=0x0c;			break;
			case CH_LS:		index_value=0x0d;			break;
			case CH_RS:		index_value=0x0e;			break;
			case CH_LB:		index_value=0x0f;			break;
			case CH_RB:		index_value=0x10;			break;
			case CH_SW:		index_value=0x11;			break;
		}

	dsp_buf[i++] =Delay_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(Max_Delay_Time>>24&0xFF);	dsp_buf[i++] =(Max_Delay_Time>>16&0xFF);	dsp_buf[i++] =(Max_Delay_Time>>8&0xFF);	dsp_buf[i++] =(Max_Delay_Time&0xFF);

	DspSpiWriteNByte(dsp_buf,i);			
}

/*
 * @brief Externnal Memory Buffer Address Setting (Address 默认从0x00000000 开始)
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA p4
 */
void DSP_EXT_BUFF_ADDR(unsigned long Externnal_Buff_Addr)
{
	short i=0;

	dsp_buf[i++] =Delay_Manager_Wop;  dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x12;
	dsp_buf[i++] =(Externnal_Buff_Addr>>24&0xFF);	dsp_buf[i++] =(Externnal_Buff_Addr>>16&0xFF);	dsp_buf[i++] =(Externnal_Buff_Addr>>8&0xFF);	dsp_buf[i++] =(Externnal_Buff_Addr&0xFF);

	DspSpiWriteNByte(dsp_buf,i);				
}

/* 
 * @brief  Externnal Memory Buffer Size Setting 
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA
 * @note  该寄存器的值为外部可存储字节数
 *		 如现在用的SDRAM 为64M bits =8M Bytes
 *
 *		8*1024*1024=8388608  
 *		转换成16 进制=0x00800000
 */
void DSP_EXT_BUFF_SIZE(unsigned long External_Buff_Size)
{
	short i=0;

	dsp_buf[i++]=Delay_Manager_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x13;
	dsp_buf[i++]=(External_Buff_Size>>24&0xFF);	dsp_buf[i++]=(External_Buff_Size>>16&0xFF);	dsp_buf[i++]=(External_Buff_Size>>8&0xFF);	dsp_buf[i++]=(External_Buff_Size&0xFF);

	DspSpiWriteNByte(dsp_buf,i);  					
}

/* 
 * @brief  Delay module 0x0014 寄存器设置
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA
 * @note  
 */
void DSP_DLYAUXCNTL(unsigned long reg_value)
{
	short i=0;

	dsp_buf[i++]=Delay_Manager_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x14;
	dsp_buf[i++]=(reg_value>>24&0xFF);	dsp_buf[i++]=(reg_value>>16&0xFF);	dsp_buf[i++]=(reg_value>>8&0xFF);	dsp_buf[i++]=(reg_value&0xFF);

	DspSpiWriteNByte(dsp_buf,i);  					
}

/* 
 * @brief  DSP 引导后加载菜单中关于Delay的设置值
 *
 * @author chenjianming
 * @date 2008-9-2
 * @refer AN246PPA
 * @note  
 */
 extern void DelayMs(int delay_time);
void DSP_Delay_Firmware_Init(void)
{
	DelayMs(50); /*切换完Code 时需要的延时,不然会引起左声道无声音*/
	DSP_Delay_Channel_Setting(CH_L,dsp_delay_time.spk_fl_dtime);  
	DSP_Delay_Channel_Setting(CH_C,dsp_delay_time.spk_c_dtime);
	DSP_Delay_Channel_Setting(CH_R,dsp_delay_time.spk_fr_dtime);
	DSP_Delay_Channel_Setting(CH_LS,dsp_delay_time.spk_ls_dtime);
	DSP_Delay_Channel_Setting(CH_RS,dsp_delay_time.spk_rs_dtime);
	DSP_Delay_Channel_Setting(CH_LB,dsp_delay_time.spk_sbl_dtime);
	DSP_Delay_Channel_Setting(CH_RB,dsp_delay_time.spk_sbr_dtime);
	DSP_Delay_Channel_Setting(CH_SW,dsp_delay_time.spk_lfe_dtime);

	DSP_MAIN_PPM_CONTROL(1,1,1); /*Enable Delay*/
}

void DSP_AV_Delay(unsigned char Delay_time)
{
	if(dsp_power_flag) 
		{
		DSP_Delay_Channel_Setting(CH_L,Delay_time);
		DSP_Delay_Channel_Setting(CH_C,Delay_time);
		DSP_Delay_Channel_Setting(CH_R,Delay_time);
		DSP_Delay_Channel_Setting(CH_LS,Delay_time);
		DSP_Delay_Channel_Setting(CH_RS,Delay_time);
		DSP_Delay_Channel_Setting(CH_LB,Delay_time);
		DSP_Delay_Channel_Setting(CH_RB,Delay_time);
		DSP_Delay_Channel_Setting(CH_SW,Delay_time);		
		}  
}

/*
 * @brief 向DSP 发送读请求命令并读取DSP 返回的数据再打印出来
 *
 * @author chenjianming
 * @date 2007-11-2
 * @note 主要用来读取index (0x0014) 这个寄存器中BIT12 的值.
 *		当为1时,且index(0x0015) 寄存器中BIT1 也为1,则表示已经
 *		超过了外部存储的容量。
 */
unsigned long DspReadDelay(unsigned char index)
{
	short i=0;

	dsp_buf[i++]=0xD9;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x00;	dsp_buf[i++]=index;

	DspSpiWriteNByte(dsp_buf,i);	/*发送读请求命令*/
	return ReadDsp(); /*读取DSP 返回的数据*/
}

#if 0
/* 
 * @brief  Dealy Firmware Configuration (引导之前加载的Delay设置值)
 *
 * @author chenjianming
 * @date 2007-11-2
 * @note 对于寄存器0x000a-0x0011(通道最大的延时设置)
 *		和0x0012-0x13(外部存储起始地址和大小)	这些寄存器最好在
 *		发送Kick_Start 命令之前设置,如果在kick_Start 之后设置一定要把
 *		index (0x0014) 中的BIT4 和BIT8 这两位置1.
 *
 */
void DSP_Delay_Firmware_Init(void)
{	
	DSP_MAIN_PPM_CONTROL(Enable,Enable,Enable);
	//DSP_Delay_Unit(0); //default

	DSP_Delay_Max_Setting(CH_L,0x00C80000);  
	DSP_Delay_Max_Setting(CH_C,0x00C80000); 
	DSP_Delay_Max_Setting(CH_R,0x00C80000);     
	DSP_Delay_Max_Setting(CH_LS,0x00C80000);
	DSP_Delay_Max_Setting(CH_RS,0x00C80000);
	DSP_Delay_Max_Setting(CH_SW,0x00C80000);
	DSP_Delay_Max_Setting(CH_LB,0x00C80000);  
	DSP_Delay_Max_Setting(CH_RB,0x00C80000);	
		
	//DSP_EXT_BUFF_ADDR(0x00000000); //default
	DSP_EXT_BUFF_SIZE(0x00800000); 
}
#endif
