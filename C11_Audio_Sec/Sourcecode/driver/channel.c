/* @file   Channel.c
 * @brief  通道切换中间层接口函数
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
#include "..\main\macro.h"
#include "..\main\task.h"
#include "..\main\debug.h"
#include "..\main\vfd_disp.h"

#include "channel.h"
#include "max335.h"
#include "cs8416.h"
#include "ioport.h"
#include "i2cdrv.h"
#include "eeprom.h"

#include "..\cs495300\Dspdrv.h"

#ifdef _DEBUG  
#define _DEBUG_CHANNEL  
#endif

unsigned char Analog_Input_To_Dsp_Flag; /*add cjm 2007-08-02*/

#define ADDR_DIGI 0x52
IIC_CONFIG const IIC_DIGI = /*lpc922 IIC通讯结构体*/
{
	ADDR_DIGI,
	set_scl2,
	set_sda2,
	sense_scl2,
	sense_sda2,

	70,   /* tAA */
	70,   /* tBUF */
	50,   /* tHD_STA */
	70,   /* tSU_STA */
	70,   /* tLOW*/
	50,   /* tHIGH*/
	1,   	/* tHD_DAT */
	10,   /* tSU_DAT */
	10,   /* tR*/
	10,   /* tF */  
	70,   /* tSU_STO */
	10,   /* tDH */
	100, /* tWR*/

	100,	/* ack timeout */
};

/*
 * @brief LPC2364与LPC922进行I2C通讯进行数字通道的切换( 向LPC922写数据)
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 * @note 
 *
 * 	LPC2364与LPC922之间规定的协议:
 *		ADDR	DATA	FUNCTION
 *		01		00		TR_OUT_LOW
 *		01		01		TR_OUT_HIGH
 *	
 *		02		0X		ZONE1 DIGI IN SELECT (HDMI SPDIF X=0,COAXIAL X=1 (NO USE),TOSLINK X=2 (NO USE),RADIO X=3)
 *		02		1X		ZONE1 DIGI IN SELECT (COAXIAL X,X=0 TO 7)
 *		02		2X		ZONE1 DIGI IN SELECT (TOSLINK X,X=1 TO 7)
 *
 *		03		0X		ZONE2 DIGI IN SELECT (HDMI SPDIF X=0,COAXIAL X=1 (NO USE),RADIO X=2)
 *		03		1X		ZONE2 DIGI IN SELECT (COAXIAL X,X=0 TO 7)
 *		03		2X		ZONE2 DIGI IN SELECT (TOSLINK X,X=1 TO 7)
 */
void DigiSendByte(unsigned char addr,unsigned char value)
{
	int result;  
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nDigiSendByte(%02X,%02X);",addr,value);
#endif
	result = I2C_bytewrite(IIC_DIGI,addr,value);
#ifdef _DEBUG_CHANNEL
	if(result == -1)
		debug_printf("\nLPC922 NACK\n");
#endif
}

/* 
 * @brief Triger Function Control
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
 //
void SetTrigerOut(int high)
{
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nTrigger:%d",high);
#endif
	if(high) 
		DigiSendByte(ADDR_TROUT,1); 
	else 
		DigiSendByte(ADDR_TROUT,0); 
}

/*****jian add in 10-09-13*******/
void SetTrigerOut1(int high)
{
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nTrigger:%d",high);
#endif
	if(high) 
		DigiSendByte(ADDR_TROUT1,1); 
	else 
		DigiSendByte(ADDR_TROUT1,0); 
}

void SetTrigerOut2(int high)
{
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nTrigger:%d",high);
#endif
	if(high) 
		DigiSendByte(ADDR_TROUT2,1); 
	else 
		DigiSendByte(ADDR_TROUT2,0); 
}

/*Zone1 Digital channel table*/
const unsigned char Digital_channel_table[] = 
{
0x10, //BALANCE
0x21, //Toslink 2
0x22, //Toslink 3
0x23, //Toslink 4
0x24, //Toslink 5
0x25, //Toslink 6
0x26, //Toslink 7
0x27, //Toslink 8
0x11, //Coaxial 2 
0x12, //Coaxial 3
0x13, //Coaxial 4
0x14, //Coaxial 5
0x15, //Coaxial 6
0x16, //Coaxial 7
0x17, //Coaxial 8
0x00, //HDMI
0x03, //Zone1_HD_Radio
0x02, //Zone2_HD_Radio
};

/*
 * @brief  1 区数字输入通道切换
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void DigitalInSelectChannel(DIGITAL_IN_CHANNEL channel_value)
{
	DigiSendByte(ADDR_ZONE1,Digital_channel_table[channel_value]);
	
}


void DigitalInSelect71Channel(void)
{
	DigiSendByte(ADDR_ZONE1,20);
}
/*Zone2 Digital channel table*/
const unsigned char Zone2_Digital_channel_table[] = 
{
0x10, //BALANCE
0x21, //Toslink 2
0x22, //Toslink 3
0x23, //Toslink 4
0x24, //Toslink 5
0x25, //Toslink 6
0x26, //Toslink 7
0x27, //Toslink 8
0x11, //Coaxial 2 
0x12, //Coaxial 3
0x13, //Coaxial 4
0x14, //Coaxial 5
0x15, //Coaxial 6
0x16, //Coaxial 7
0x17, //Coaxial 8
0x00, //HDMI
0x02, //Zone2_HD_Radio
0x03, //开光关掉
};

/*
 * @brief  2 区数字输入通道切换
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void Zone2DigiSelectChannel(DIGITAL_IN_CHANNEL channel_value)
{
	DigiSendByte(ADDR_ZONE2,Zone2_Digital_channel_table[channel_value]);
}

/*Max335 chip select*/
#define MAX335_CHIP1 0x0000
#define MAX335_CHIP2 0x0100
#define MAX335_CHIP3 0x0200
#define MAX335_CHIP4 0x0300
#define MAX335_CHIP5 0x0400
#define MAX335_CHIP6 0x0500
#define MAX335_CHIP7 0x0600
#define MAX335_CHIP8 0x0700 

/*Max335 chip mission*/
#define INPUT_SELECT_MIC				(MAX335_CHIP8 | 0x80) //BIT7-MIC
#define INPUT_SELECT_2CH_RL			(MAX335_CHIP8 | 0x50) //BIT6/BIT4-2CHL/R
#define INPUT_SELECT_7CH1_RL			(MAX335_CHIP8 | 0x28) //BIT5/BIT3-7.1L/R
#define INPUT_SELECT_Zone2_Downmix   	(MAX335_CHIP8 | 0X06) //BIT2/BIT1-Zone2 Downmix 

#define ANALOG_SELECT_TUNER			(MAX335_CHIP7 | 0x05)  //Zone 1 TUNER(L/R)
#define ANALOG_SELECT_BALANCE 		(MAX335_CHIP7 | 0xA0)  //Zone 1 Balance(L/R)
#define ANALOG_SELECT_ANALOG_2		(MAX335_CHIP6 | 0x05)  //Zone 1 Analog2(L/R)
#define ANALOG_SELECT_ANALOG_3		(MAX335_CHIP6 | 0xA0)  //Zone 1 Analog3(L/R)
#define ANALOG_SELECT_ANALOG_4		(MAX335_CHIP5 | 0x05)  //Zone 1 Analog4(L/R)
#define ANALOG_SELECT_ANALOG_5		(MAX335_CHIP5 | 0xA0)  //Zone 1 Analog5(L/R)
#define ANALOG_SELECT_ANALOG_6		(MAX335_CHIP4 | 0x05)  //Zone 1 Analog6(L/R)
#define ANALOG_SELECT_ANALOG_7		(MAX335_CHIP4 | 0xA0)  //Zone 1 Analog7(L/R)
#define ANALOG_SELECT_ANALOG_8		(MAX335_CHIP3 | 0x05)  //Zone 1 Analog8(L/R)

#define ZONE2_SELECT_TUNER				(MAX335_CHIP7 | 0x0A)  //Zone 2 TUNER(L/R)
#define ZONE2_SELECT_BALANCE			(MAX335_CHIP7 | 0x50)  //Zone 2 Balance(L/R)
#define ZONE2_SELECT_ANALOG_2 		(MAX335_CHIP6 | 0x0A)  //Zone 2 Analog2(L/R)
#define ZONE2_SELECT_ANALOG_3 		(MAX335_CHIP6 | 0x50)  //Zone 2 Analog3(L/R)
#define ZONE2_SELECT_ANALOG_4 		(MAX335_CHIP5 | 0x0A)  //Zone 2 Analog4(L/R)
#define ZONE2_SELECT_ANALOG_5 		(MAX335_CHIP5 | 0x50)  //Zone 2 Analog5(L/R)
#define ZONE2_SELECT_ANALOG_6 		(MAX335_CHIP4 | 0x0A)  //Zone 2 Analog6(L/R)
#define ZONE2_SELECT_ANALOG_7 		(MAX335_CHIP4 | 0x50)  //Zone 2 Analog7(L/R)
#define ZONE2_SELECT_ANALOG_8 		(MAX335_CHIP3 | 0x0A)  //Zone 2 Analog8(L/R)
#define ZONE2_SELECT_7CH1_RL			(MAX335_CHIP3 | 0x30)  //Zone2 7.1 

#define OUTPUT_7CH1_SELECT_2CH		(MAX335_CHIP3 | 0xC0)  //Zone 1 2 CH ByPass(L/R) 
#define OUTPUT_7CH1_SELECT_7CH1		(MAX335_CHIP2 | 0xFF)  //Analog 7.1CH BY PASS
#define OUTPUT_7CH1_SELECT_DSP		(MAX335_CHIP1 | 0xFF)  //D/A 输出控制的八个声道

/*Input Select Table*/
const unsigned int input_select_table[]=
{
INPUT_SELECT_MIC,
INPUT_SELECT_2CH_RL,	
INPUT_SELECT_7CH1_RL,
INPUT_SELECT_Zone2_Downmix,
0,
};

/*Zone1 Analog table*/
const unsigned int analog_in_table[] = 
{
ANALOG_SELECT_BALANCE,		//ANALOG_IN_BALANCE
ANALOG_SELECT_ANALOG_2,	//ANALOG 2
ANALOG_SELECT_ANALOG_3,	//ANALOG 3
ANALOG_SELECT_ANALOG_4,	//ANALOG 4
ANALOG_SELECT_ANALOG_5,	//ANALOG 5
ANALOG_SELECT_ANALOG_6,	//ANALOG 6
ANALOG_SELECT_ANALOG_7,	//ANALOG 7
ANALOG_SELECT_ANALOG_8,	//ANALOG 8 
ANALOG_SELECT_TUNER,		//ANALOG_IN_TUNER
0,
};

/*Zone2 Analog table*/
const unsigned int zone2_analog_in_table[] = 
{
ZONE2_SELECT_BALANCE,		//ANALOG_IN_BALANCE
ZONE2_SELECT_ANALOG_2,		//ANALOG 2
ZONE2_SELECT_ANALOG_3,		//ANALOG 3
ZONE2_SELECT_ANALOG_4,		//ANALOG 4
ZONE2_SELECT_ANALOG_5,		//ANALOG 5
ZONE2_SELECT_ANALOG_6,		//ANALOG 6
ZONE2_SELECT_ANALOG_7,		//ANALOG 7
ZONE2_SELECT_ANALOG_8,		//ANALOG 8
ZONE2_SELECT_7CH1_RL,    	//ANALOG 7.1 R/L
ZONE2_SELECT_TUNER,			//ANALOG_IN_TUNER
0,
};

/*
 * @brief  模拟通道信号检测
 *
 * @author chenjianming
 * @date 2008-1-21
 * @refer chenshiguang C11 code
 */
unsigned char CheckAnalogIn(unsigned int ms_value)
{
	uchar retry_time;
	for(retry_time=0;retry_time<10;retry_time++)
		{
		DelayMs(ms_value);
		if(!SenseAnalogSignal()) /*unlock*/
			{
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog In none\n");
#endif  
			return 1;	/*unlock*/			
			}   
		}
	  
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nAnalog In valid\n");  
#endif
	return 0;	/*Lock*/	
}

/* 
 * @brief 关闭Zone1 所有模拟输入通道
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void AnalogInOff(void)
{
	unsigned char i = 0;

#ifdef _DEBUG_CHANNEL
	//debug_printf("\nAnalogInOff();");
#endif

	while(analog_in_table[i])
		{
		SetMax335Switch(analog_in_table[i],0);	
		i++;
		}
	UpdateMax335();	/*Update MAX335*/
}

/* 
 * @brief 关闭Zone2 模拟输入通道
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void Zone2AnalogInOff(void)
{
	unsigned char i=0;
	
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nZone2AnalogInOff();\n");
#endif		

	while(zone2_analog_in_table[i])  
		{
		SetMax335Switch(zone2_analog_in_table[i],0);
		i++;
		}
	UpdateMax335();	/*Update MAX335*/
}

/* 
 * @brief Zone 1 Analog Channel Select
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void AnalogInSelectChannel(ANALOG_IN_CHANNEL channel_value)
{
	static ANALOG_IN_CHANNEL pre_channel;	
	
	if(pre_channel != channel_value)
		{
		SetMax335Switch(analog_in_table[pre_channel],0);	/*先关闭analog in 之前打开的开关*/
		pre_channel = channel_value;
		}
	
	SetMax335Switch(analog_in_table[pre_channel],1);	/*打开当前选择的analog in开关*/	
	UpdateMax335();/*Update MAX335*/
}

/* 
 * @brief Zone 2 Channel Select
 *
 * @author chenjianming
 * @date 2008-2-27
 * @refer 
 */
 void Zone2SelectChannel(SYSTEM_CHANNEL channel_value)
{
	unsigned char select_channel;
	//
	if(channel_value!=INPUT_CH71)
		{
#ifdef _DEBUG_CHANNEL
		debug_printf("\nchannel_value=%d",channel_value);  
		debug_printf("\nASSIGNED_INPUT1=%d",EepromReadByte(ASSIGNED_INPUT1+channel_value));
#endif
		select_channel=EepromReadByte(ASSIGNED_INPUT1 + channel_value);
		if(channel_value==INPUT_1)
			{
			select_channel=select_channel+1; /*Input1 只有Digital =0和Analog =1输入*/
			}
		
		if(select_channel==2) /*Analog 输入时,打开相应的模拟输出*/
			{
			Zone2AnalogInOff(); /*Zone2 的所有模拟输入都关掉*/
			SetMax335Switch(INPUT_SELECT_Zone2_Downmix,0);/*downmix开关关掉*/
			SetMax335Switch(zone2_analog_in_table[channel_value],1); /*打开当前选择的analog in开关*/
			UpdateMax335();	/*Update MAX335*/
			}  
		else /*Toslink 和Coaxial 数字输入时,当1区和2区选择的输入不一样时,直接打开2区的数字输出.当1区和2区的输入选择一样时,模拟downmix 输出且还要数字输出*/
			{
			Zone2AnalogInOff(); /*Zone2 的所有模拟输入都关掉*/
			if(sys_flag.zone2_channel !=sys_flag.zone1_channel) /*如果1区的输入通道和2区的不一样(Bypass)*/
				{
#ifdef _DEBUG_CHANNEL
				//debug_printf("\nZone2 Select Bypass\n");
#endif				
				if((channel_value >= INPUT_2) && (channel_value <= INPUT_8)) /*Input2-Input8 Toslink和Coaxial输入*/
					{
					if(EepromReadByte(ASSIGNED_INPUT1 + channel_value)==0)
						Zone2DigiSelectChannel(channel_value); /*Toslink输入--前8个地址为1个平衡地址+ 7个Toslink 地址*/
					else 
						Zone2DigiSelectChannel(channel_value+7); /*coaxial输入--后面接着7个地址为Coaxial 地址*/
					}
				else  if(channel_value == INPUT_1)
					Zone2DigiSelectChannel(0); /*Input1 平衡输入*/		
				else	
					{
					    if(((sys_flag.zone1_channel >= INPUT_HDMI1)&&(sys_flag.zone1_channel <= INPUT_HDMI4))&&(sys_flag.zone2_channel == INPUT_HDMI ||sys_flag.zone2_channel == ARC_CHANNEL))
					    {
						DigiSendByte(ADDR_ZONE2,0x00);
						DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix 使能,从DA02_Channel 1/2 输出*/					
						}  // jain add in 12-03-07
					else if(sys_flag.zone2_channel == INPUT_TUNER)  // jian add in 12-04-17
						{
							SetMax335Switch(INPUT_SELECT_Zone2_Downmix,0);/*打开downmix开关*/				
							SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
							UpdateMax335();	/*Update MAX335*/
							DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/

						}						
					}
				}
			else  /*如果1区的输入通道和2区的一样(Downmix)*/
				{
				if(dsp_power_flag)
					{
#ifdef _DEBUG_CHANNEL
					debug_printf("\nZone2 Select Downmix\n");
#endif					
					//打开digital 输出
					if((channel_value >= INPUT_2) && (channel_value <= INPUT_8)) /*Input2-Input8 Toslink和Coaxial输入*/
						{
						if(EepromReadByte(ASSIGNED_INPUT1 + channel_value)==0)
							Zone2DigiSelectChannel(channel_value); /*Toslink输入--前8个地址为1个平衡地址+ 7个Toslink 地址*/
						else 
							Zone2DigiSelectChannel(channel_value+7); /*coaxial输入--后面接着7个地址为Coaxial 地址*/
						}
					else  if(channel_value == INPUT_1)
						Zone2DigiSelectChannel(0); /*Input1 平衡输入*/		
					else	
						DigiSendByte(ADDR_ZONE2,0x00);
					//模拟Downmix 下来并打开输出
					DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix 使能,从DA02_Channel 1/2 输出*/					
					SetMax335Switch(INPUT_SELECT_Zone2_Downmix,1);/*打开downmix开关*/
					UpdateMax335();	/*Update MAX335*/
					}
				}
			}
		}
	else /* 打开Input 7.1 输出*/
		{
		Zone2AnalogInOff(); /*Zone2 的模拟输入都关掉*/
		SetMax335Switch(zone2_analog_in_table[channel_value],1); /*打开当前选择的analog in开关*/
		UpdateMax335();	/*Update MAX335*/
		}
}

/*
 * @brief 模拟输入类型选择
 *
 * @author chenjianming
 * @date 2008-1-25
 * @refer 
 *	0--MIC_INPUT
 *    1--2CH_RL_INPUT
 *    2--7.1CH_RL_INPUT
 *    3--ZONE2_DOWNMIX_INPUT
 */
void AnalogInputTypeSelect(unsigned char channel_value)
{
	SetMax335Switch(INPUT_SELECT_MIC,0);
	SetMax335Switch(INPUT_SELECT_2CH_RL,0);
	SetMax335Switch(INPUT_SELECT_7CH1_RL,0);
	SetMax335Switch(INPUT_SELECT_Zone2_Downmix,0);
	switch(channel_value)
		{
		case Select_MIC_INPUT:
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog MIC Input");
#endif			
			SetMax335Switch(INPUT_SELECT_MIC,1);
			break;
		case Select_2CH_RL_INPUT:
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog 2 Channel Input");
#endif			
			SetMax335Switch(INPUT_SELECT_2CH_RL,1);
			break;
		case Select_7CH1_RL_INPUT:
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog 7.1 Channel Input");
#endif			
			SetMax335Switch(INPUT_SELECT_7CH1_RL,1);
			break;
		case Select_Zone2_Downmix_INPUT:
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog Zone2 Downmix Input");
#endif			
			SetMax335Switch(INPUT_SELECT_Zone2_Downmix,1);
			break;
		}
	UpdateMax335();
}

/*
 * @brief 选择7.1通道输出的源
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 //
 */

 #define DIGI_CHECK_TIME 30

void Analog7CH1OutSelectChannel(OUTPUT_7CH1_CHANNEL channel_value)
{
	SetMax335Switch(OUTPUT_7CH1_SELECT_2CH,0);
	SetMax335Switch(OUTPUT_7CH1_SELECT_7CH1,0);
	SetMax335Switch(OUTPUT_7CH1_SELECT_DSP,0);
	switch(channel_value)
		{
		case SOURCE_2CH:	/*选择2CH 通道(2CH Bypass)*/
			SetMax335Switch(OUTPUT_7CH1_SELECT_2CH,1);
			break;
		case SOURCE_CH71_IN:/*选择7.1CH 通道(7.1CH Bypass)*/
			SetMax335Switch(OUTPUT_7CH1_SELECT_7CH1,1);
			break;
		case SOURCE_DSP:	/*选择DSP解码通道*/
			SetMax335Switch(OUTPUT_7CH1_SELECT_DSP,1);
			break;
		}
	UpdateMax335();	/*Set MAX335*/
}

#define ANALOG_CHECK_TIME 50
/* 
 * @brief 输入通道信号检测
 *
 * @author chenjianming
 * @date 2008--
 * @refer chenshiguang Cll code
 * @note
 *   硬件上面应该保证切换完通道之后,detect的充放电时间小于ANALOG_CHECK_TIME,
 *   如果在系统中还要定时检测输入的话,那么可以用类似按键去抖动的原理来判断,
 *   这样可以兼顾通道切换的迅速以及实时检测输入是否有信号
 */
static unsigned char CheckInputChannel(unsigned char channel,unsigned char sub_channel)
/*Select input channel
 *input:
 	channel: input 1~8, 
 	sub_channel:1->toslink,2->coaxial,3->analog
 *output:
 	0:unlock
 	1:toslink lock
 	2:coaxial lock
 	3:analog lock
 */
{
	unsigned char input_channel;

	if(channel != INPUT_CH71)
		{
		channel = channel << 4;
		channel &= 0xF0;
		channel |= sub_channel;

		switch(channel)
			{
			case ((INPUT_1 << 4) | 1):
			case ((INPUT_1 << 4) | 2):
				input_channel = DIGITAL_IN_BALANCE; /*平衡数字输入*/
				break;
			case ((INPUT_1 << 4) | 3):
				input_channel = ANALOG_IN_BALANCE; /*平衡模拟输入*/
				break;
			case ((INPUT_2 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_2;
				break;
			case ((INPUT_2 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_2;
				break;
			case ((INPUT_2 << 4) | 3):
				input_channel = ANALOG_IN_2;
				break;
			case ((INPUT_3 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_3;
				break;
			case ((INPUT_3 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_3;
				break;
			case ((INPUT_3 << 4) | 3):
				input_channel = ANALOG_IN_3;
				break;
			case ((INPUT_4 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_4;
				break;
			case ((INPUT_4 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_4;
				break;
			case ((INPUT_4 << 4) | 3):
				input_channel = ANALOG_IN_4;
				break;
			case ((INPUT_5 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_5;
				break;
			case ((INPUT_5 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_5;
				break;
			case ((INPUT_5 << 4) | 3):
				input_channel = ANALOG_IN_5;
				break;
			case ((INPUT_6 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_6;
				break;
			case ((INPUT_6 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_6;
				break;
			case ((INPUT_6 << 4) | 3):
				input_channel = ANALOG_IN_6;
				break;
			case ((INPUT_7 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_7;
				break;
			case ((INPUT_7 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_7;
				break;
			case ((INPUT_7 << 4) | 3):
				input_channel = ANALOG_IN_7;
				break;
			case ((INPUT_8 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_8;
				break;
			case ((INPUT_8 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_8;
				break;
			case ((INPUT_8 << 4) | 3):
				input_channel = ANALOG_IN_8;
				break;
			}


//                debug_printf("input_channel1:%x\n",input_channel);
				  
		if(sub_channel == 3)
			{
			AnalogInputTypeSelect(Select_2CH_RL_INPUT);/*输入为2 channel R/L*/ 
			Analog7CH1OutSelectChannel(SOURCE_DSP); /*输出为D/A 8通道输出*/
			AnalogInSelectChannel(input_channel); /*打开相应输入通道的模拟通道*/
			DigitalInSelectChannel(0); /*add cjm 2007-08-01*/
			Analog_Input_To_Dsp_Flag=1; /*add cjm 2007-08-02*/	
			//if(CheckAnalogIn(ANALOG_CHECK_TIME)==0) //Dele by cjm 2008-12-23
				return sub_channel;
			} 
		else
			{
			
			DigitalInSelectChannel(input_channel); /*切到相应输入通道的Toslink或者Coaxial*/
		//	  debug_printf("input_channel2:%x\n",input_channel);
			if(CheckCs8416Unlock(DIGI_CHECK_TIME) == 0)	/*lock*/
				return sub_channel;
			}
		}
	else
		{
		DigitalInSelect71Channel();
		AnalogInputTypeSelect(Select_7CH1_RL_INPUT); /*输入为7 channel R/L*/ 
		if(CheckAnalogIn(ANALOG_CHECK_TIME) == 0)
			return 1;
		}
	return 0;
}

/*input:channel,从INPUT_1到INPUT_8的任何一个
 函数将自动搜索该input对应的toslink,coaxial,analog,
 如果有信号输入，则返回当前的input(toslink,coaxial,analog),否则返回0
 */
unsigned char SystemSelectInputChannel(unsigned char channel)
{
	unsigned char i = 1;
	unsigned char status;

	AnalogInOff();	/*关闭所有的模拟通道，确保模拟通道的输入检测电路尽快放电*/

	if(channel == INPUT_1)
		i = 2;
	
	for(;i<=3;i++)
		{
		status = CheckInputChannel(channel,i);
		if(status)
			return i;
		}
	return 0;
}

/*add cjm 2007-07-30*/
/*input:channel,只搜索从INPUT_1到INPUT_8 Assign 中设置的通道
函数将自动搜索ASSIGN 设置的输入接口类型
如果有信号输入，则返回当前的INPUT(toslink,coaxial,analog),否则返回0
*/
unsigned char SystemSearchInputChannel(unsigned char channel)
{
	unsigned char status;
	unsigned char sub_channel;

	AnalogInOff();	/*关闭所有的模拟通道，确保模拟通道的输入检测电路尽快放电*/
	
	switch(channel)
		{
			case INPUT_HDMI1: /*HDMI1 HDMI2 直接跳出信号检测*/
			case INPUT_HDMI2:
			case INPUT_HDMI3: /*HDMI1 HDMI2 直接跳出信号检测*/
			case INPUT_HDMI4:

				return 0;  
				
			case INPUT_1:  /*Input 1输入只有Digital=2 和Analog=3 两种输入*/
				sub_channel=EepromReadByte(ASSIGNED_INPUT1)+2;
				break;
	
			default:
				sub_channel=EepromReadByte(ASSIGNED_INPUT1+channel)+1;
				break;		
		}

	status=CheckInputChannel(channel,sub_channel);
	
	if(status)
		return sub_channel;
	else
		return 0;	
}/*add cjm 2007-07-30*/

unsigned char CheckInput(unsigned char channel,unsigned char sub_channel)
{
	unsigned char status;
	AnalogInOff();
	status = CheckInputChannel(channel,sub_channel);  
	if(status)
		return 1;
	return 0;
}

