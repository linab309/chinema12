/* @file   Eq_Manager.c
 * @brief Eq 固件驱动程序
 *
 * @author chenjianming
 * @date 2007-10-24
 * @refer AN246PPE
 *
 *		Write=0xD500HHHH 0xhhhhhhhh
 *		Read Request=0xD5C0HHHH
 *		Read Response=0x55C0HHHH 0xhhhhhhhh
 *
 *		0xHHHH=index  0xhhhhhhhh=data value
 */
#include "math.h"  
#include "dspdrv.h"
#include "Eq_Manager.h"
#include "BassManager.h"
#include "..\driver\Dsp_Spi.h"
#include "..\driver\timer.h"
#include "..\driver\cs8416.h"
#include "..\driver\channel.h"
#include "..\driver\Eeprom.h"
#include "..\main\debug.h"
#include "..\main\Task.h"

#ifdef _DEBUG  
#define _DEBUG_EQ_MANAGER
#endif

#define POW_2_12  0x1000			//4096
#define POW_2_26  0x3FFFFFF		//67108864L

#define EQ_Ch_L_Band1_Fc 	0x25
#define EQ_Ch_L_Band2_Fc 	0x28
#define EQ_Ch_L_Band3_Fc 	0x2B
#define EQ_Ch_L_Band4_Fc 	0x2E
#define EQ_Ch_L_Band5_Fc 	0x31
#define EQ_Ch_L_Band6_Fc 	0x34
#define EQ_Ch_L_Band7_Fc 	0x37
#define EQ_Ch_L_Band8_Fc 	0x3A
#define EQ_Ch_L_Band9_Fc 	0x3D
#define EQ_Ch_L_Band10_Fc 	0x40
#define EQ_Ch_L_Band11_Fc 	0x43

#define EQ_Ch_C_Band1_Fc 	0x46
#define EQ_Ch_C_Band2_Fc 	0x49
#define EQ_Ch_C_Band3_Fc 	0x4C
#define EQ_Ch_C_Band4_Fc 	0x4F
#define EQ_Ch_C_Band5_Fc 	0x52
#define EQ_Ch_C_Band6_Fc 	0x55
#define EQ_Ch_C_Band7_Fc 	0x58
#define EQ_Ch_C_Band8_Fc 	0x5B
#define EQ_Ch_C_Band9_Fc 	0x5E
#define EQ_Ch_C_Band10_Fc 	0x61
#define EQ_Ch_C_Band11_Fc 	0x64

#define EQ_Ch_R_Band1_Fc 	0x67
#define EQ_Ch_R_Band2_Fc 	0x6A
#define EQ_Ch_R_Band3_Fc 	0x6D
#define EQ_Ch_R_Band4_Fc 	0x70
#define EQ_Ch_R_Band5_Fc 	0x73
#define EQ_Ch_R_Band6_Fc 	0x76
#define EQ_Ch_R_Band7_Fc 	0x79
#define EQ_Ch_R_Band8_Fc 	0x7C
#define EQ_Ch_R_Band9_Fc 	0x7F
#define EQ_Ch_R_Band10_Fc 	0x82
#define EQ_Ch_R_Band11_Fc 	0x85

#define EQ_Ch_LS_Band1_Fc 	0x88
#define EQ_Ch_LS_Band2_Fc 	0x8B
#define EQ_Ch_LS_Band3_Fc 	0x8E
#define EQ_Ch_LS_Band4_Fc 	0x91
#define EQ_Ch_LS_Band5_Fc 	0x94
#define EQ_Ch_LS_Band6_Fc 	0x97
#define EQ_Ch_LS_Band7_Fc 	0x9A
#define EQ_Ch_LS_Band8_Fc 	0x9D
#define EQ_Ch_LS_Band9_Fc 	0xA0
#define EQ_Ch_LS_Band10_Fc 	0xA3
#define EQ_Ch_LS_Band11_Fc 	0xA6

#define EQ_Ch_RS_Band1_Fc 	0xA9
#define EQ_Ch_RS_Band2_Fc 	0xAC
#define EQ_Ch_RS_Band3_Fc 	0xAF
#define EQ_Ch_RS_Band4_Fc 	0xB2
#define EQ_Ch_RS_Band5_Fc 	0xB5
#define EQ_Ch_RS_Band6_Fc 	0xB8
#define EQ_Ch_RS_Band7_Fc 	0xBB
#define EQ_Ch_RS_Band8_Fc 	0xBE
#define EQ_Ch_RS_Band9_Fc 	0xC1
#define EQ_Ch_RS_Band10_Fc 0xC4
#define EQ_Ch_RS_Band11_Fc 0xC7

#define EQ_Ch_LB_Band1_Fc 	0xCA
#define EQ_Ch_LB_Band2_Fc 	0xCD
#define EQ_Ch_LB_Band3_Fc 	0xD0
#define EQ_Ch_LB_Band4_Fc 	0xD3
#define EQ_Ch_LB_Band5_Fc 	0xD6
#define EQ_Ch_LB_Band6_Fc 	0xD9
#define EQ_Ch_LB_Band7_Fc 	0xDC
#define EQ_Ch_LB_Band8_Fc 	0xDF
#define EQ_Ch_LB_Band9_Fc 	0xE2
#define EQ_Ch_LB_Band10_Fc 0xE5
#define EQ_Ch_LB_Band11_Fc 0xE8

#define EQ_Ch_RB_Band1_Fc 	0xEB
#define EQ_Ch_RB_Band2_Fc 	0xEE
#define EQ_Ch_RB_Band3_Fc 	0xF1
#define EQ_Ch_RB_Band4_Fc 	0xF4
#define EQ_Ch_RB_Band5_Fc 	0xF7
#define EQ_Ch_RB_Band6_Fc 	0xFA
#define EQ_Ch_RB_Band7_Fc 	0xFD
#define EQ_Ch_RB_Band8_Fc 	0x100
#define EQ_Ch_RB_Band9_Fc 	0x103
#define EQ_Ch_RB_Band10_Fc 0x106
#define EQ_Ch_RB_Band11_Fc 0x109

#define EQ_Ch_LFE_Band1_Fc 0x10C
#define EQ_Ch_LFE_Band2_Fc 0x10F
#define EQ_Ch_LFE_Band3_Fc 0x112
#define EQ_Ch_LFE_Band4_Fc 0x115
#define EQ_Ch_LFE_Band5_Fc 0x118
#define EQ_Ch_LFE_Band6_Fc 0x11B
#define EQ_Ch_LFE_Band7_Fc 0x11E
#define EQ_Ch_LFE_Band8_Fc 0x121
#define EQ_Ch_LFE_Band9_Fc 0x124
#define EQ_Ch_LFE_Band10_Fc 0x127
#define EQ_Ch_LFE_Band11_Fc 0x12A

float dsp_eq_table[8][10] = {0};
float dsp_eq_music_table[8][10]={0};

const short sw_freq_table[] = {25,40,50,63,70,80,90,100,110,125};
const short freq_table[] = {80,160,250,500,1000,2000,4000,8000,12000,16000};

const short eq_var_table[8][11] = {
{EQ_Ch_L_Band1_Fc,EQ_Ch_L_Band2_Fc,EQ_Ch_L_Band3_Fc,EQ_Ch_L_Band4_Fc,EQ_Ch_L_Band5_Fc,EQ_Ch_L_Band6_Fc,EQ_Ch_L_Band7_Fc,EQ_Ch_L_Band8_Fc,EQ_Ch_L_Band9_Fc,EQ_Ch_L_Band10_Fc,EQ_Ch_L_Band11_Fc,},
{EQ_Ch_C_Band1_Fc,EQ_Ch_C_Band2_Fc,EQ_Ch_C_Band3_Fc,EQ_Ch_C_Band4_Fc,EQ_Ch_C_Band5_Fc,EQ_Ch_C_Band6_Fc,EQ_Ch_C_Band7_Fc,EQ_Ch_C_Band8_Fc,EQ_Ch_C_Band9_Fc,EQ_Ch_C_Band10_Fc,EQ_Ch_C_Band11_Fc,},
{EQ_Ch_R_Band1_Fc,EQ_Ch_R_Band2_Fc,EQ_Ch_R_Band3_Fc,EQ_Ch_R_Band4_Fc,EQ_Ch_R_Band5_Fc,EQ_Ch_R_Band6_Fc,EQ_Ch_R_Band7_Fc,EQ_Ch_R_Band8_Fc,EQ_Ch_R_Band9_Fc,EQ_Ch_R_Band10_Fc,EQ_Ch_R_Band11_Fc,},
{EQ_Ch_LS_Band1_Fc,EQ_Ch_LS_Band2_Fc,EQ_Ch_LS_Band3_Fc,EQ_Ch_LS_Band4_Fc,EQ_Ch_LS_Band5_Fc,EQ_Ch_LS_Band6_Fc,EQ_Ch_LS_Band7_Fc,EQ_Ch_LS_Band8_Fc,EQ_Ch_LS_Band9_Fc,EQ_Ch_LS_Band10_Fc,EQ_Ch_LS_Band11_Fc,},
{EQ_Ch_RS_Band1_Fc,EQ_Ch_RS_Band2_Fc,EQ_Ch_RS_Band3_Fc,EQ_Ch_RS_Band4_Fc,EQ_Ch_RS_Band5_Fc,EQ_Ch_RS_Band6_Fc,EQ_Ch_RS_Band7_Fc,EQ_Ch_RS_Band8_Fc,EQ_Ch_RS_Band9_Fc,EQ_Ch_RS_Band10_Fc,EQ_Ch_RS_Band11_Fc,},
{EQ_Ch_LB_Band1_Fc,EQ_Ch_LB_Band2_Fc,EQ_Ch_LB_Band3_Fc,EQ_Ch_LB_Band4_Fc,EQ_Ch_LB_Band5_Fc,EQ_Ch_LB_Band6_Fc,EQ_Ch_LB_Band7_Fc,EQ_Ch_LB_Band8_Fc,EQ_Ch_LB_Band9_Fc,EQ_Ch_LB_Band10_Fc,EQ_Ch_LB_Band11_Fc,},
{EQ_Ch_RB_Band1_Fc,EQ_Ch_RB_Band2_Fc,EQ_Ch_RB_Band3_Fc,EQ_Ch_RB_Band4_Fc,EQ_Ch_RB_Band5_Fc,EQ_Ch_RB_Band6_Fc,EQ_Ch_RB_Band7_Fc,EQ_Ch_RB_Band8_Fc,EQ_Ch_RB_Band9_Fc,EQ_Ch_RB_Band10_Fc,EQ_Ch_RB_Band11_Fc,},
{EQ_Ch_LFE_Band1_Fc,EQ_Ch_LFE_Band2_Fc,EQ_Ch_LFE_Band3_Fc,EQ_Ch_LFE_Band4_Fc,EQ_Ch_LFE_Band5_Fc,EQ_Ch_LFE_Band6_Fc,EQ_Ch_LFE_Band7_Fc,EQ_Ch_LFE_Band8_Fc,EQ_Ch_LFE_Band9_Fc,EQ_Ch_LFE_Band10_Fc,EQ_Ch_LFE_Band11_Fc}};

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];
extern const float balance_table[] ;
/*
 * @brief 把EQ 的频率值(十进制)转换成十六进制用于寄存器的设置
 *
 * @author chenjianming
 * @date 2007-10-25
 * @refer AN246PPE P7
 */
unsigned long CALC_EQ_FC(float HZ_Value)
{
	unsigned long rtn_value;

	rtn_value=HZ_Value*POW_2_12;
	return rtn_value;
}

/* 
 * @brief 把EQ 的Q系数(十进制)转换成十六进制用于寄存器的设置
 *
 * @author chenjianming
 * @date 2007-10-25
 * @refer AN246PPE P7
 */
unsigned long CALC_EQ_Q(float Q_Value)
{
	unsigned long rtn_value;

	rtn_value=Q_Value*POW_2_26;
	return rtn_value;
}

/* 
 * @brief  把EQ 的G系数(十进制)转换成十六进制用于寄存器的设置
 *
 * @author chenjianming
 * @date 2007-10-25
 * @refer AN246PPE P7
 */
signed long CALC_EQ_G(float G_Value)
{
	unsigned long rtn_value;
  
	if(G_Value>=0)
		{
		rtn_value=G_Value*POW_2_26;
		}
	else
		{  
		rtn_value=(-G_Value)*POW_2_26;
		rtn_value=~rtn_value;
		}    
		
	return rtn_value;
}

/* 
 * @brief Eq Module Control 
 *
 * @author chenjianming
 * @date 2007-10-24
 * @refer AN246PPE p3
 * @note  EQ_Channel_Enable:See Table2-4 for channel mapping
 */
void DSP_Eq_Control(unsigned char EQ_Global_Enable,unsigned char EQ_Global_Reset,unsigned long EQ_Channel_Enable)
{
	short i=0;
	unsigned char value=0;

	if(EQ_Global_Enable)
		value|=0x80;
	if(EQ_Global_Reset)
		value|=0x40;

	dsp_buf[i++] =Eq_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =value;	dsp_buf[i++] =0x00;	dsp_buf[i++] =(EQ_Channel_Enable>>8&0xFF);	dsp_buf[i++] =(EQ_Channel_Enable&0xFF);

	DspSpiWriteNByte(dsp_buf,i);
}

 /* 
 * @brief EQ Mode Select
 *
 * @author chenjianming
 * @date 2007-10-24
 * @refer AN246PPE
 */
 void DSP_Eq_Config(unsigned char mode)
 {
 	short i=0;
	unsigned char value=0;

	if(mode)
		value=0x01;
	else
		value=0x00; 

	dsp_buf[i++] =Eq_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i);
 }

 /*
 * @biref EQ channnels configuration
 * @author chenjianming
 * @data 2007-10-24
 * @refer AN246PPE
 * @note  取默认值0XFF--L/C/R/LS/RS/SLB/SRB/LFE (see table 2-4 for channel mapping)
 */
 void DSP_Eq_Channels(unsigned long Eq_Channel_Configuration)
 {
 	short i=0;

	dsp_buf[i++]=Eq_Manager_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x02;
	dsp_buf[i++]=0x00; 	dsp_buf[i++]=0x00;	dsp_buf[i++]=(Eq_Channel_Configuration>>8&0xFF);	dsp_buf[i++]=(Eq_Channel_Configuration&0xFF);

	DspSpiWriteNByte(dsp_buf,i); 
 }

 /* 
 * @brief  Eq Bands Setting
 *
 * @author chenjianming
 * @date 2007-10-24
 * @refer AN246PPE
 * @note  取默认值11 bands
 */
 void DSP_Eq_Bands(unsigned char bands)
 {
  	short i=0;

	dsp_buf[i++]=Eq_Manager_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x03;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=(bands&0xFF);

	DspSpiWriteNByte(dsp_buf,i);  
 }

/*
 * @brief Eq 固件中的Flash 设置 
 *
 * @author chenjianming
 * @date 2007-10-26
 * @refer AN246PPE
 */
void DSP_Eq_Flash(unsigned char Eq_Flash_Restore,unsigned char Eq_Flash_Save)
{
	short i=0;
	unsigned char value;

	if(Eq_Flash_Restore)
		value|=0x80;
	if(Eq_Flash_Save)
		value|=0x40;

	dsp_buf[i++]=Eq_Manager_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x04;
	dsp_buf[i++]=value;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP中EQ固件的PREATTENUATION 值设置
 *
 * @author chenjianming
 * @date 2007-10-31
 * @refer AN288PPE P4
 */
void DSP_EQ_CH_PREATTENUATION(EQ_Channel channel,unsigned long value)
{
	short i=0;
	unsigned char index_value;

	switch(channel)
		{
			case EQ_CH_L:			index_value=0x05;			break;
			case EQ_CH_C:			index_value=0x07;			break;
			case EQ_CH_R:			index_value=0x09;			break;
			case EQ_CH_LS:			index_value=0x0b;			break;
			case EQ_CH_RS:			index_value=0x0d;			break;
			case EQ_CH_LB:			index_value=0x0f;			break;
			case EQ_CH_RB:			index_value=0x11;			break;
			case EQ_CH_LFE0:		index_value=0x13;			break;
			case EQ_CH_LFE1:		index_value=0x15;			break;
			case EQ_CH_LFE2:		index_value=0x17;			break;
			case EQ_CH_LFE3:		index_value=0x19;			break;
			case EQ_CH_Reserve:		index_value=0x1b;			break;
			case EQ_CH_LD:			index_value=0x1d;			break;
			case EQ_CH_RD:			index_value=0x1f;			break;
			case EQ_CH_LA:			index_value=0x21;			break;
			case EQ_CH_RA:			index_value=0x23;			break;
		}

	dsp_buf[i++] =Eq_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(value>>24 & 0xFF);	dsp_buf[i++] =(value>>16 & 0xFF);	dsp_buf[i++] =(value>>8 & 0xFF);	dsp_buf[i++] =value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief DSP中EQ固件的POSTGAIN 值设置
 *
 * @author chenjianming
 * @date 2007-10-31
 * @refer AN288PPE P4
 */
void DSP_EQ_POSTGAIN(EQ_Channel channel,unsigned long value)
{
	short i=0;
	unsigned char index_value;

	switch(channel)
		{
			case EQ_CH_L:			index_value=0x06;			break;
			case EQ_CH_C:			index_value=0x08;			break;
			case EQ_CH_R:			index_value=0x0a;			break;
			case EQ_CH_LS:			index_value=0x0c;			break;
			case EQ_CH_RS:			index_value=0x0e;			break;
			case EQ_CH_LB:			index_value=0x10;			break;
			case EQ_CH_RB:			index_value=0x12;			break;
			case EQ_CH_LFE0:			index_value=0x14;			break;
			case EQ_CH_LFE1:			index_value=0x16;			break;
			case EQ_CH_LFE2:			index_value=0x18;			break;
			case EQ_CH_LFE3:			index_value=0x1a;			break;
			case EQ_CH_Reserve:		index_value=0x1c;			break;
			case EQ_CH_LD:			index_value=0x1e;			break;
			case EQ_CH_RD:			index_value=0x20;			break;
			case EQ_CH_LA:			index_value=0x22;			break;
			case EQ_CH_RA:			index_value=0x24;			break;
		}
	
	dsp_buf[i++] =Eq_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(value>>24 & 0xFF);	dsp_buf[i++] =(value>>16 & 0xFF);	dsp_buf[i++] =(value>>8 & 0xFF);	dsp_buf[i++] =value & 0xFF;
	
	DspSpiWriteNByte(dsp_buf,i);
}	

 /* 
 * @brief 当DSP 运行时,若要改变Gain 值,则要发送如下命令,使DSP 重新计算Gain值
 *
 * @author chenjianming
 * @date 2007-10-31
 * @refer AN288PPE P9-P10
 * @note 要对应相应的通道和Bands 
 */
 void DSP_EQ_CHANGER(unsigned char index)
 {
 	short i=0;

	dsp_buf[i++]=0xD5;	dsp_buf[i++]=0x40;	dsp_buf[i++]=0x00;	dsp_buf[i++]=index;
	dsp_buf[i++]=0x80;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;

	DspSpiWriteNByte(dsp_buf,i);  
 }

/* 
 * @brief Eq Module Setting 
 *
 * @author chenjianming
 * @date 2007-10-24
 * @refer chenshiguang C11 code
 */
 void EqModuleconfig(unsigned char channel,unsigned char band)
 { 	
 	if(GetNormalFreq() == FREQ_192KHZ)	/*192K不能做EQ*/
		return ;

       if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				//if(((sys_flag.zone1_channel==INPUT_HDMI1)&&(sys_flag.src_mode!=1))||((sys_flag.zone1_channel==INPUT_HDMI2)&&(sys_flag.src_mode!=1))) /*高清流不能做EQ*/
		return ;
	
 	if(dsp_power_flag) 
 		{ 		
#ifdef _DEBUG_EQ_MANAGER 
		debug_printf("EqModule channel:%d\n",channel);
		debug_printf("EqModule Eq_Freq:%d\n",freq_table[band]);
		debug_printf("EqModule Eq_Gain:%f\n",dsp_eq_table[channel][band]);
		debug_printf("EqModule Eq_Fc:%x\n",eq_var_table[channel][band]); 
#endif
		DSP_Eq_Control(Enable,Disable,EQ_8_Channel_Enable); /*EQ Enable*/

		if(channel!=CH_SW)  
			{         
			DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band],CALC_EQ_FC(freq_table[band])); /*EQ_FC 频带设置*/  
			DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+1,CALC_EQ_Q(2.14)); /*EQ_Q 改变幅度大小系数默认设置为1*/
			if(Movie_Music_Speaker_Flag==0)
				DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+2,CALC_EQ_G(dsp_eq_table[channel][band])); /*EQ_Gain 设置*/
			else
				DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+2,CALC_EQ_G(dsp_eq_music_table[channel][band])); /*EQ_Gain 设置*/
			DSP_EQ_CHANGER(eq_var_table[channel][band]); /*EQ_GAIN改变就要多加这条设置-Recompute the coefficients	*/		
			}  
		else
			{    
			DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band],CALC_EQ_FC(sw_freq_table[band])); /*EQ_FC 频带设置*/
			DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+1,CALC_EQ_Q(2.14)); /*EQ_Q 改变幅度大小系数默认设置为1*/
			if(Movie_Music_Speaker_Flag==0)
				DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+2,CALC_EQ_G(dsp_eq_table[channel][band])); /*EQ_Gain 设置*/
			else
				DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+2,CALC_EQ_G(dsp_eq_music_table[channel][band])); /*EQ_Gain 设置*/
			DSP_EQ_CHANGER(eq_var_table[channel][band]); /*EQ_GAIN改变就要多加这条设置-Recompute the coefficients	*/		
			}  
 		}  
 }

/* 
 * @brief DSP Power 加载EQ 菜单设置值
 *
 * @author chenjianming
 * @date 2007-10-24
 * @refer chenshiguang C11 code
 */
 extern unsigned char PreDspMode;
void DSP_Power_on_config(unsigned char channel,unsigned char band)
{
 	if(GetNormalFreq() == FREQ_192KHZ)	/*192K不能做EQ*/
		return ;

	//if((sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))/*高清流不能做EQ*/
	if(((sys_flag.zone1_channel==INPUT_HDMI1)&&(PreDspMode==4))||((sys_flag.zone1_channel==INPUT_HDMI2)&&(PreDspMode==4))||((sys_flag.zone1_channel==INPUT_HDMI3)&&(PreDspMode==4))||((sys_flag.zone1_channel==INPUT_HDMI4)&&(PreDspMode==4))) /*高清流不能做EQ*/
		
	
		return ;
	
	if(dsp_power_flag) 
 		{ 				
		if(channel!=CH_SW)  
			{         
			DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band],CALC_EQ_FC(freq_table[band])); /*EQ_FC 频带设置*/  	
			DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+1,CALC_EQ_Q(2.14)); /*EQ_Q 改变幅度大小系数默认设置为1*/
			if(Movie_Music_Speaker_Flag==0)
				DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+2,CALC_EQ_G(dsp_eq_table[channel][band])); /*EQ_Gain 设置*/
			else
				DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+2,CALC_EQ_G(dsp_eq_music_table[channel][band])); /*EQ_Gain 设置*/
			DSP_EQ_CHANGER(eq_var_table[channel][band]); /*EQ_GAIN改变就要多加这条设置-Recompute the coefficients	*/
			}  
		else
			{    
			DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band],CALC_EQ_FC(sw_freq_table[band])); /*EQ_FC 频带设置*/
			DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+1,CALC_EQ_Q(2.14)); /*EQ_Q 改变幅度大小系数默认设置为1*/
			if(Movie_Music_Speaker_Flag==0)
				DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+2,CALC_EQ_G(dsp_eq_table[channel][band])); /*EQ_Gain 设置*/
			else
				DSPFirmwareWrite(Eq_Manager_Wop,eq_var_table[channel][band]+2,CALC_EQ_G(dsp_eq_music_table[channel][band])); /*EQ_Gain 设置*/
			DSP_EQ_CHANGER(eq_var_table[channel][band]); /*EQ_GAIN改变就要多加这条设置-Recompute the coefficients	*/
			}  
 		}  	
}
 
/* 
 * @brief Eq 固件设置
 *
 * @author chenjianming
 * @date 2007-10-25
 * @note 以下这些参数必须要在DSP KickStart 之前设置
 */
 void DSP_EQ_Firmware_Init(void)
 {
 	unsigned char channel,bank;

	if(enable_eq)
		{
		for(channel=0;channel<8;channel++) /*8 个通道*/
			for(bank=0;bank<10;bank++) /*10 频点*/
				{
				DSP_Power_on_config(channel,bank); /*对各频点的FC/Q/G进行设置*/
				}
		DSP_Eq_Control(Enable,Disable,EQ_8_Channel_Enable); /*EQ Enable*/
		}
 }

/* 
 * @brief Eq on Enable
 *
 * @author chenjianming
 * @date 2008-02-24
 * @note
 */
void EqOn(unsigned char on)
{
#ifdef _DEBUG_EQ_MANAGER
	dsp_printf("%d\n",on);  
#endif
	if(dsp_power_flag) 
		{
		if(on)
			{
			DSP_Eq_Control(Enable,Disable,EQ_8_Channel_Enable);
			DSP_EQ_Firmware_Init(); 
			}
		else
			DSP_Eq_Control(Disable,Disable,EQ_8_Channel_Enable);		
		}	
}  

/* 
 * @brief 读取Eq 固件设置的值
 *
 * @author chenjianming
 * @date 2007-10-27
 */
extern unsigned char SenseDspIrq(void);
void DspReadEq(unsigned char index)
{
	short i=0;
	
#ifdef _DEBUG_EQ_MANAGER 
	unsigned char k;
#endif
	
	dsp_buf[i++]=0xD5;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x00;	dsp_buf[i++]=index;

	DspSpiWriteNByte(dsp_buf,i);

#ifdef _DEBUG_EQ_MANAGER 
	//debug_printf("Before DspReadEq\n");
#endif

	do{
	     if(!SenseDspIrq())
		break;
	     DelayMs(1);
	     }while(1); /*判断DSP_IRQ 是否为低*/
	
#ifdef _DEBUG_EQ_MANAGER 
	//debug_printf("After DspReadEq\n"); 
#endif

	DspSpiReadNByte(dsp_buf,8);  /*读取返回消息并放到dsp_buf*/		

#ifdef _DEBUG_EQ_MANAGER 
	for(k=0;k<8;k++)
	    debug_printf("%02X ",dsp_buf[k]);	
	debug_printf("\n");
#endif
}
	
