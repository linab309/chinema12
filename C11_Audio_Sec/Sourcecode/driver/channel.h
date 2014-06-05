#ifndef _CHANNEL_H
#define _CHANNEL_H

enum{
ADDR_TROUT = 1, /*控制触发命令的地址*/
ADDR_ZONE1, /*控制Zone1 数字通道切换命令的地址*/
ADDR_ZONE2, /*控制Zone2数字通道切换命令的地址*/
ADDR_TROUT1, /*控制触发命令的地址*/
ADDR_TROUT2, /*控制触发命令的地址*/
};

/* 用DIGITAL_IN_CHANNEL 代替枚举类型,枚举类型可以用来定义变量*/
typedef enum  
{
DIGITAL_IN_BALANCE = 0,
DIGITAL_IN_TOSLINK_2,
DIGITAL_IN_TOSLINK_3,
DIGITAL_IN_TOSLINK_4,
DIGITAL_IN_TOSLINK_5,
DIGITAL_IN_TOSLINK_6,
DIGITAL_IN_TOSLINK_7,
DIGITAL_IN_TOSLINK_8,
DIGITAL_IN_COAXIAL_2,
DIGITAL_IN_COAXIAL_3,
DIGITAL_IN_COAXIAL_4,
DIGITAL_IN_COAXIAL_5,
DIGITAL_IN_COAXIAL_6,
DIGITAL_IN_COAXIAL_7,
DIGITAL_IN_COAXIAL_8,
DIGITAL_IN_HDMI,
DIGITAL_IN_Zone1_HD_Radio,
DIGITAL_IN_Zone2_HD_Radio,
}DIGITAL_IN_CHANNEL;

typedef enum
{
ANALOG_IN_BALANCE = 0,
ANALOG_IN_2,
ANALOG_IN_3,
ANALOG_IN_4,
ANALOG_IN_5,
ANALOG_IN_6,
ANALOG_IN_7,
ANALOG_IN_8,
ANALOG_IN_TUNER,
}ANALOG_IN_CHANNEL;

typedef enum
{
ZONE2_ANALOG_IN_BALANCE = 0,
ZONE2_ANALOG_IN_2,
ZONE2_ANALOG_IN_3,
ZONE2_ANALOG_IN_4,
ZONE2_ANALOG_IN_5,
ZONE2_ANALOG_IN_6,
ZONE2_ANALOG_IN_7,
ZONE2_ANALOG_IN_8,
ZONE2_ANALOG_IN_TUNER,
ANALOG_7CH1_RL,
Zone2_Downmix,
}ZONE2_ANALOG_IN_CHANNEL;

typedef enum
{
SOURCE_2CH = 0,
SOURCE_CH71_IN,
SOURCE_DSP,
}OUTPUT_7CH1_CHANNEL;

typedef enum
{
INPUT_1 = 0,
INPUT_2,
INPUT_3,
INPUT_4,
INPUT_5,
INPUT_6,
INPUT_7,
INPUT_8,
INPUT_CH71,
INPUT_TUNER,
INPUT_HDMI1,
INPUT_HDMI2,
INPUT_HDMI3,// jian add in 10-09-13
INPUT_HDMI4, // jian add in 10-09-13
ARC_CHANNEL, // JIAN ADD IN 10-08-06
INPUT_HDMI,
}SYSTEM_CHANNEL;

enum
{
HDMI_1=0,
HDMI_2,
HDMI_3,
HDMI_4,

};
//
#define Select_MIC_INPUT				0
#define Select_2CH_RL_INPUT				1
#define Select_7CH1_RL_INPUT			2
#define Select_Zone2_Downmix_INPUT   	3
#define Select_None_Input				4

#define Zone2_Channel_Select_None		17


void DigitalInSelect71Channel(void);
extern void Zone2AnalogInOff(void);
extern void AnalogInOff(void);
extern unsigned char CheckAnalogIn(unsigned int ms_value);
extern void DigiSendByte(unsigned char addr,unsigned char value);
extern void AnalogInputTypeSelect(unsigned char channel_value);
extern unsigned char SystemSearchInputChannel(unsigned char channel); /*add cjm 2007-08-03*/
extern void DigitalInSelectChannel(DIGITAL_IN_CHANNEL channel_value);
extern void AnalogInSelectChannel(ANALOG_IN_CHANNEL channel_value);
extern void Zone2SelectChannel(SYSTEM_CHANNEL channel_value);
extern void Analog7CH1OutSelectChannel(OUTPUT_7CH1_CHANNEL channel_value);
extern unsigned char SystemSelectInputChannel(unsigned char channel);
extern unsigned char CheckInput(unsigned char channel,unsigned char sub_channel);
extern void Zone2DigiSelectChannel(DIGITAL_IN_CHANNEL channel_value);
#endif

