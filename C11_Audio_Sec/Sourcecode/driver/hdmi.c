/* @file    Hdmi.c
 * @brief   主板上的LPC2364与HDMI板上的LPC2132 通讯
 *
 * @author chenjianming
 * @date 2008-3-12
 * @note 
 *
 * CPU之间的通讯协议:
 *
 *     LPC2364发送给LPC2132的协议:
 * 		      ADDR  	DATA    	FUNCTION
  *			01		00		HDMI ALL OFF
 *		  	01      	01       	HDMI1
 *		  	01     	02       	HDMI2
 *
 *		  	02     	01       	spdif
 *		  	02     	02       	i2s
 *		  	02     	03       	DSD
 *
 *		  	03     	00       	不认证
 *		 	03     	01       	认证
 *
 *             	04        	00	   	Spdif off
 *             	04        	01       	Spdif on
 *
 * 			05          02		MCLK主时钟(Use only for Dolby Plus)
 *
 *			06		00		LPCM
 *			06		01		spdif
 *			06		02		HDMI 高清流
 *
 *			08		00		HDMI Audio Out OFF
 *                  08		01		HDMI Audio Out ON
 *
 *	 LPC2364从LPC2132读到的协议:
 *		     ADDR	   	DATA    	FUNCTION
 *			01        	00	 	普通流
 *			01        	01      	高清流
 *			01		02		DSD 流
 *
 *			02	    	08 		2通道DSD 流
 *			02        	02		多通道DSD流
 *
 *			03	    	00		44.1K
 *			03	    	01      	Not indicated 	
 *                  03          	02       	48K
 *			03	    	03       	32K
 *			03	    	04       	22.05k
 *			03	    	06       	24k
 *			03	    	08       	88.2K	
 *                  03          	09        	768k
 *                  03          	0A        	96K
 *                  03          	0C        	176.4K
 *                  03          	0E        	192K
 *
 *			04      	其它	HDMI板流输出不稳定
 *			04		06		HDMI板流输出稳定  
 *   
 *                  05		0/1		7.1通道以下
 *			05		8		7.1多通道
 */

#include "..\main\debug.h"
#include "..\cs495300\Dspdrv.h"
#include "..\cs495300\Dsp_API.h"
#include "..\main\Task.h"
#include "..\driver\Dac.h"
#include "..\driver\Volume.h"
#include "..\main\C11_msg_handler.h"
#include "Hdmi.h"
#include "cs8416.h"  
#include "i2cdrv.h"  
#include "ioport.h"
#include "channel.h"

#ifdef _DEBUG
#define _DEBUG_HDMI
#endif

extern unsigned char Hdmi_Stream_tick;
extern unsigned char CD_Pure_Audio;


#define ADDR_HDMI 0x54
IIC_CONFIG const IIC_HDMI = /*LPC2132 IIC 结构体*/
{
	ADDR_HDMI,
	set_scl_hdmi,  
	set_sda_hdmi,
	sense_scl_hdmi,
	sense_sda_hdmi,

	70,   /* tAA     */
	70,   /* tBUF    */
	50,   /* tHD_STA */
	70,   /* tSU_STA */
	70,   /* tLOW    */
	50,   /* tHIGH   */
	1,   	/* tHD_DAT */
	10,   /* tSU_DAT */
	10,   /* tR      */
	10,   /* tF      */
	70,   /* tSU_STO */
	10,   /* tDH     */
	100,   /* tWR*/

	100,	/* ack timeout */
};

/* 
 * @brief 向LPC2132 发数据
 *
 * @author chenjianming
 * @date 2008-3-12
 * @refer 
 */
void HdmiSendByte(unsigned char addr,unsigned char value)
{
	int result;
	
#ifdef _DEBUG_HDMI
	//debug_printf("HdmiSendByte(%02X,%02X);\n",addr,value);
#endif

	result = I2C_bytewrite(IIC_HDMI,addr,value);

#ifdef _DEBUG_HDMI
	if(result == -1)
		debug_printf("HDMI NACK\n");
#endif
}

/* 
 * @brief 从LPC2132 读数据
 *
 * @author chenjianming
 * @date 2008-8-12
 * @refer 
 */
unsigned char HdmiReadByte(unsigned int addr)
{
	return (unsigned char)I2C_byteread(IIC_HDMI,addr);
}

/* 
 * @brief HDMI port1 and port2 select
 *
 * @author chenjianming
 * @date 2008-3-12
 * @refer 
 */
void HDMI_Port_Select(int Port_Select)
{
#ifdef _DEBUG_HDMI
	debug_printf("\nHDMI_Port_Select:%d",Port_Select);
#endif
	switch(Port_Select)
		{
		case 0:
			HdmiSendByte(ADDR_HDMI_SELECT,0); /*选择HDMI 端口1*/
			HdmiSendByte(ADDR_HDMI_ARC_OUT,0); /*HDMI ALL Close*/ 
			break;		
		case 1:
			HdmiSendByte(ADDR_HDMI_SELECT,1); /*选择HDMI 端口2*/
			HdmiSendByte(ADDR_HDMI_ARC_OUT,0); /*HDMI ALL Close*/ 
			break;
		case 2:
			HdmiSendByte(ADDR_HDMI_SELECT,2); /*HDMI ALL Close*/ 
			HdmiSendByte(ADDR_HDMI_ARC_OUT,0); /*HDMI ALL Close*/ 
			break;
		case 3:
			HdmiSendByte(ADDR_HDMI_SELECT,3); /*HDMI ALL Close*/ 
			HdmiSendByte(ADDR_HDMI_ARC_OUT,0); /*HDMI ALL Close*/ 
			break;
		case 4:
//			HdmiSendByte(ADDR_HDMI_SELECT,3); /*HDMI ALL Close*/ 
			HdmiSendByte(ADDR_HDMI_ARC_OUT,1); /*HDMI ALL Close*/ 
			break;			
		default:
			break;
		} 
}


#if 0
/* 
 * @brief HDMI Authentication Setting
 *
 * @author chenjianming
 * @date 2008-3-12
 * @refer 
 */
void HDMI_Authentication(int on)
{
#ifdef _DEBUG_HDMI
	debug_printf("HDMI_Authentication:%d\n",on);
#endif

	if(on)
		HdmiSendByte(ADDR_AUTHENTICATION,1); /*HDMI 不认证*/ 
	else
		HdmiSendByte(ADDR_AUTHENTICATION,0); /*HDMI 认证*/
}
#endif

#if 0
/* 
 * @brief HDMI SPDIF ON or OFF Setting
 *
 * @author chenjianming
 * @date 2008-3-12
 * @refer 
 */
void HDMI_SPDIF_ON(int on)
{
#ifdef _DEBUG_HDMI
	debug_printf("HDMI_SPDIF_ON:%d\n",on);
#endif

	if(on)
		HdmiSendByte(ADDR_SPDIF,1); /*SPDIF OFF*/ 
	else
		HdmiSendByte(ADDR_SPDIF,0); /*SPDIF ON*/ 
}
#endif

#if 0
/* 
 * @brief 读取HDMI 寄存器2的值,DSD2通道和DSD多通道之间的切换
 *
 * @author chenjianming
 * @date 2008-8-19
 * @refer 
 */
void Deal_HDMI_Reg2(unsigned char value)
{
	switch(value)
		{
		case 2: /*2 通道DSD 流*/
			break;
		case 8: /*多声道DSD 流*/
			break;
		default:
			break;
		}
}

/* 
 * @brief 读取HDMI 寄存器3的值,读取LPCM 的频率
 *
 * @author chenjianming
 * @date 2008-8-19
 * @refer 
 */
void Deal_HDMI_Reg3(unsigned char value)
{
	switch(value)
		{
		case 0x00: 	break; /*44.1k*/
		case 0x01:	break; /*Not indicated*/	
		case 0x02:	break; /*48k*/
		case 0x03:	break; /*32k*/
		case 0x04:	break; /*22.05k*/
		case 0x06:	break; /*24k*/
		case 0x08:	break; /*88.2k*/
		case 0x09:	break; /*768k*/
		case 0x0A:	break; /*96k*/
		case 0x0C:	break; /*176.4k*/
		case 0x0E:	break; /*192k*/
		default: 		break;
		}
}
#endif

#if 0
/* 
 * @brief HDMI Loop
 *
 * @author chenjianming
 * @date 2008-8-14
 * @refer 
 */
extern unsigned long last_stream;
extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];
extern unsigned char PreDspMode;
unsigned char Pre_value=8;
void HDMI_Loop(void)
{
	unsigned char value;
	unsigned char value1;
	unsigned char fre_value=0;
	
	if(sys_flag.system_status == POWER_ON) 
		{
		if(GetCurFocusTid() == RADIO_ID)
			return ;
		if((sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
			{
			if(Hdmi_Stream_tick==1) /*1S 查询一次HDMI板上的寄存器*/
				{
				Hdmi_Stream_tick=0;
				value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
#ifdef _DEBUG_HDMI										
				debug_printf("\nHDMI_Return_Value=%d",value);	                       
#endif
				DelayMs(1); /*去掉Debug长时间播放TureHD 无声音问题,故加此延时*/
				if(Pre_value!=value) /*当2次的值不一样时*/
					{  
					Pre_value=value;      
					if(value==5) /*HDMI板状态寄存器值为6--HDMI板流稳定*/ //Change by cjm 2009-07-10
						{
						//DacMute(1); //mute时会产生噪声
						DigiSendByte(ADDR_ZONE1,0X00); /*切到HDMI 板上的Spdif 口,用于当前CS8146锁频用*/
						DelayMs(500); //Change by cjm 2009-07-10   
						if(CheckCs8416Unlock(30) == 1) /*CS8416 没有锁住频率*/
							{
#ifdef _DEBUG_HDMI						
							debug_printf("\nCS8416 is unLock--switch to DSP HDMI Port");  
#endif
							SoftMute(1);
							DelayMs(500);
							SetDspPower(4); /*DSP 切换到HDMI 口*/	
							DSP_Input_Stream_Select(HDMI_Input); /*输入到DSP源--HDMI(I2S)--放到DSP引导起来后,否则DSP处于忙状态*/			
							SoftMute(0);
							}
						else 
							{	
#ifdef _DEBUG_HDMI													
							debug_printf("\nCS8416 is Lock");  
#endif
							fre_value=Cs8416GetFreq();
							if(fre_value!=FREQ_192KHZ) 
								{
#ifdef _DEBUG_HDMI													
								//debug_printf("\nThe freq is not 192K--Switch to DSP spdif port");  
#endif								
								value1=HdmiReadByte(5); /*读HDMI板LPCM通道数*/
								if(value1!=8) /*确定是普通流*/
									{
#ifdef _DEBUG_HDMI													
									debug_printf("\nThe freq is not 192K and is not multi LPCM --switch to DSP spdif port");  
#endif										
									SoftMute(1);
									DelayMs(500);
									SetDspPower(1); /*DSP 切换到spdif 口*/ 	  
									DSP_Input_Stream_Select(AD_Input); /*输入到DSP源--A/D CS8416*/	
									SoftMute(0);
									}
								else /*确定为LPCM 多通道,DSP先却换到HDMI口获取LPCM信息,然后再切换到ANALOG中进行解码*/
									{
#ifdef _DEBUG_HDMI													
									debug_printf("\nThe freq is not 192K and is multi LPCM--Switch to DSP HDMI Port");  
#endif										
									SoftMute(1);
									DelayMs(500);
									SetDspPower(4); /*DSP 切换到HDMI 口*/  	    
									DSP_Input_Stream_Select(HDMI_Input); /*输入到DSP源--HDMI(I2S)--放到DSP引导起来后,否则DSP处于忙状态*/	
									SoftMute(0);
									}
								}
							else /*这时频率为192K 可能是DolbyPlus包*/
								{
#ifdef _DEBUG_HDMI													
								debug_printf("\nThe freq is 192K--Switch to DSP HDMI port");  
#endif								
								SoftMute(1);
								DelayMs(500);
								SetDspPower(4); /*DSP 切换到HDMI 口*/	
								DSP_Input_Stream_Select(HDMI_Input); /*输入到DSP源--HDMI(I2S)--放到DSP引导起来后,否则DSP处于忙状态*/			
								SoftMute(0);
								}	 
							}
						//DacMute(0);  //mute时会产生噪声
						}				
					else /*当前流不稳定*/  
						{
#ifdef _DEBUG_HDMI						
						debug_printf("\nHold On Dsp in Reset");
#endif
						last_stream=0; /*用于解决HDMI 兼容性问题*/
						SoftMute(1); /*add by cjm 2008-12-23 解决HDMI锁流过程中的噪声问题*/
						SetDspPower(0); /*DSP 保持复位状态*/	
						}			
					}  
				}
			}
		}  
}
#endif

/* 
 * @brief HDMI Loop
 *
 * @author chenjianming
 * @date 2009-8-6
 * @Update  
 */
extern unsigned long last_stream;
extern unsigned char PreDspMode;
unsigned char Pre_value=8;
extern unsigned char CD_8416_channel ; // jian add in 12-03-09

#define MAX335_CHIP8 0x0700 
#define INPUT_SELECT_Zone2_Downmix   	(MAX335_CHIP8 | 0X06) //BIT2/BIT1-Zone2 Downmix 

void HDMI_Loop(void)
{
	unsigned char value;
	
	if(sys_flag.system_status == POWER_ON) 
	{
		if(GetCurFocusTid() == RADIO_ID)
			return ;
		if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2)||(sys_flag.zone1_channel==ARC_CHANNEL))
		{
			if(Hdmi_Stream_tick==1) /*1S 查询一次HDMI板上的寄存器*/
			{
				Hdmi_Stream_tick=0;
				value=HdmiReadByte(4); /*读HDMI板状态寄存器*/ 
#ifdef _DEBUG_HDMI										
				debug_printf("\nHDMI_Valid_Return_Value=%d",value);	                       
#endif
				if(Pre_value!=value) /*当2次的值不一样时*/ 
				{  
					Pre_value=value; 
					if(value==5) /*HDMI板状态寄存器值为5--HDMI板Audio 流稳定*/ 
					{
#ifdef _DEBUG_HDMI										
						debug_printf("\nHDMI Stream is valid,DSP Switch to HDMI Port ");	                       
#endif				



                                              if(sys_flag.zone1_channel == ARC_CHANNEL)
                                              		{
							SetDspPower(1); /*DSP 切换到HDMI 口获取当前流信息*/	
							DSP_Input_Stream_Select(AD_Input);  // jian add in 10-08-07
							DigiSendByte(ADDR_ZONE2,0x00); /*数字板切换到HDMI 板上的Spdif 口*/
							if(sys_flag.zone2_channel == ARC_CHANNEL)
								{
								DigiSendByte(ADDR_ZONE2,0X00); /*数字板切换到HDMI 板上的Spdif 口*/
								DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix 使能,从DA02_Channel 1/2 输出*/													
								}
                                              		}
					  	else
					  		{
							SetDspPower(4); /*DSP 切换到HDMI 口获取当前流信息*/	
							DSP_Input_Stream_Select(HDMI_Input);  
								debug_printf("\nHDMI Stream  ");	
							if(sys_flag.zone2_channel == INPUT_HDMI)
								{
								debug_printf("\nHDMI Stream is INPUT HDMI ");	
								DigiSendByte(ADDR_ZONE2,0x00); /*数字板切换到HDMI 板上的Spdif 口*/
								SetMax335Switch(INPUT_SELECT_Zone2_Downmix,1);/*打开downmix开关*/
								UpdateMax335();	/*Update MAX335*/
								DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix 使能,从DA02_Channel 1/2 输出*/													
								}

					  		}

#if 1
					if((Cs8416GetFreq() == FREQ_44_1KHZ)&&(CheckCs8416Unlock(30) == 0)&&(IsCs8416Pcm()))	/*lock*/
						{
					#ifdef 1
						debug_printf("\n HDMI CD STERM0 :%d \n",CD_Pure_Audio);
					#endif 			 	 	         

				                 if(CD_Pure_Audio == 1)
			            	          	{
			            	          	 //切换通道   走非DSP通道
							//cd_dsp_channle_Select(1);
						//	Pcm1796Write(0x1400,DSD_ALL_Select); /*DSD mode Disable*/
						        CD_8416_channel  = 1;
			                      		}
						else
							{
						        CD_8416_channel  = 0;
							}
							
						 }	
					else
					             CD_8416_channel  = 0;
#endif
							InitDSDMode(0); /*DA-PCM mode*/

					}
					else
					{  
#ifdef _DEBUG_HDMI						
						debug_printf("\nHDMI Stream is invalid ,Dsp hold On Reset");  
#endif
						last_stream=0; /*用于解决HDMI 兼容性问题*/
						SoftMute(1); /*add by cjm 2008-12-23 解决HDMI锁流过程中的噪声问题*/
						SetDspPower(0); /*DSP 保持复位状态*/			 		
					}    



				}

#if 1
				if(value==5)
					{					
					if((Cs8416GetFreq() == FREQ_44_1KHZ)&&(CheckCs8416Unlock(30) == 0)&&(IsCs8416Pcm()))	/*lock*/
						{
					
						debug_printf("\n HDMI CD STERM1 :%d \n",CD_Pure_Audio);
							 	 	         

				                 if(CD_Pure_Audio == 1)
			            	          	{
			            	          	 //切换通道   走非DSP通道
							  CD_8416_channel  = 1;
							
			                      		}
						else
							{
                                                             CD_8416_channel  = 0;
						         }
							
						 }	
						else
                                                             CD_8416_channel  = 0;
					}
				#endif
			}
		}
	}
}

/* 
 * @brief DSD Loop
 *
 * @author chenjianming
 * @date 2008-8-14
 * @refer 
 */
extern unsigned char stream_type;
extern unsigned char DSDType;
extern unsigned char DSD_Stream_tick;
void DSD_Loop(void)
 {
 	static unsigned char Pre_value=8;
	unsigned char reg_value=0; /*HDMI 板流稳定状态寄存器*/
	unsigned char reg_value2=0; /*SACD 通道数信息*/
  
	if(sys_flag.system_status == POWER_ON)
		{
		if(GetCurFocusTid() == RADIO_ID)
			return ;		
//		if((sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2)||(sys_flag.zone1_channel==ARC_CHANNEL))
		if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2)||(sys_flag.zone1_channel==ARC_CHANNEL))
			{
			if(DSD_Stream_tick==1)
				{
				DSD_Stream_tick=0;
				reg_value=HdmiReadByte(4);  /*读HDMI板状态寄存器*/
#ifdef _DEBUG_HDMI										
				debug_printf("\nDSD_HDMI_Return_Value=%d",reg_value);	
#endif						
				if(reg_value!=5) //change by cjm 2009-7-10
					{
					last_stream=0; /*解决SACD 出仓无信息问题*/
					SoftMute(1);
					InitDSDMode(0);
				//	SoftMute(0);				
					}
				if(stream_type !=STREAM_DSD)
					Pre_value	 = 0;
				if(Pre_value!=reg_value)
					{			
					Pre_value=reg_value;
					if(reg_value==5) /*DSD 流稳定*/
						{
						SoftMute(1);
						reg_value2=HdmiReadByte(2); /*读HDMI板流通道数寄存器*/
#ifdef _DEBUG_HDMI										
						debug_printf("\nDSD_Stream_Channel=%d",reg_value2);  
#endif							
						if(reg_value2==0) /*reg_value2=0--2声道SACD*/  //change by cjm 2009-7-10
							{
							InitDSDMode(1); /*2channel DSD Enable*/
							stream_type=STREAM_DSD;
							DSDType=_DSD_2CH; 
							MsgSetStream(stream_type,0); 
							}
						else  /*reg_value2=1-多声道SACD*/
							{
							InitDSDMode(2); /*5.1 channel DSD Enable*/
							stream_type=STREAM_DSD;
							DSDType=_DSD_Multi_channels;
							MsgSetStream(stream_type,0); 
							}		
						SoftMute(0);
						}
					}
				}
			}

		}
 }

/* 
 * @brief 
 *  当非常频繁切换DolbyTureHD流,估计由于时钟的原因,会导致DSP处于死机状态
 *  故平均3秒读下DSP的寄存器,如果无法读DSP数据,表示DSP已死机,需重启DSP  
 *
 * @author chenjianming
 * @date 2008-8-14
 * @refer   
 */
extern unsigned char DSP_Busy_tick;
void HDMI_Busy(void)
{
	if(sys_flag.system_status == POWER_ON) 
		{
		if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))//||(sys_flag.zone1_channel==ARC_CHANNEL))
			{
			if(dsp_power_flag) /*DSP已经引导起来*/
				{
				if(DSP_Busy_tick>5)
					{
					DSP_Busy_tick=0; 
					DSP_API_Read(0x13);
					}					
				}
			}
		}
}  

/* 
 * @brief HDMI DSD Loop
 *
 * @author chenjianming
 * @date 2008-8-14
 * @refer HDMI各种输入流的切换
 */
extern unsigned char HDMI_tick;


void HDMI_secret_loop(void) // jian add in 10-09-09
{

        static uchar hdmi_loop_time = 0;
	unsigned char reg_value=0; /*HDMI 板流稳定状态寄存器*/
	
	if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2)||(sys_flag.zone1_channel==ARC_CHANNEL))
		{
                if(hdmi_loop_time > 50)
                	{
			reg_value=HdmiReadByte(4);  /*读HDMI板状态寄存器*/
			
			if(reg_value !=5)
				SoftMute(1);
			
			hdmi_loop_time = 0;
                	}
                 else
			hdmi_loop_time++;
		}
	else
		hdmi_loop_time = 0;
}

//
void HDMI_DSD_Loop(void)
{
	unsigned char reg_value=0;

	if(sys_flag.system_status == POWER_ON)
		{
		if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2)||(sys_flag.zone1_channel==ARC_CHANNEL))
			{
			if(HDMI_tick==1) /*1s 查询1次*/
				{
				HDMI_tick=0;
 				reg_value=HdmiReadByte(1); /*读HDMI板流通道数寄存器*/
#ifdef _DEBUG_HDMI				
				debug_printf("\nHDMI_Stream_type_value=%d",reg_value);
#endif
				if(reg_value==1) /*DSD 流*/ 
					{
					DSD_Loop();
					}
				else /*普通和高清流*/ 
					{					
					InitDSDMode(0); /*DA-PCM mode*/
	 				HDMI_Loop();	 	  	
					HDMI_Busy();
					}								
				}
			}  
		}
 }

