/* @file   Dsp_task.c
 * @brief DSP 主循环
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang C11 code
 */
#include "dspdrv.h"
#include "DolbyManager.h"
#include "..\driver\Dsp_Spi.h"
#include "..\driver\ioport.h"
#include "..\driver\Cs8416.h"
#include "..\driver\Dac.h"
#include "..\main\Task.h"
#include "..\main\Debug.h"
#include "..\cs495300\DTS_Manager.h"
#include "..\driver\Hdmi.h"
#include "..\driver\channel.h"

#ifdef _DEBUG
#define _DEBUG_DSP_TASK
#endif

#define DSP_MSG_SIZE	8

char dsp_power_flag = 0;


extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];
extern unsigned char dsp_time_tick ;
extern unsigned char psm_8416_time_tick; // jian add in 12-01-31
extern unsigned char CurrentSampleFreq;
extern unsigned char stream_type;
extern SYSTEM_FLAG sys_flag;
extern unsigned char Analog_Input_To_Dsp_Flag; 
extern void ProcInputSelect(unsigned char direction);


unsigned char CD_8416_channel = 0;  //jian add in 12-03-08

#ifdef _DEBUG_DSP_TASK
void DispBuffer(unsigned char *buffer,short count)
{
	while(count)
		{
		dsp_printf("%02X ",* buffer ++);
		count --;
		}
	dsp_printf("\n");
}
#endif

#ifdef _DEBUG_DSP_TASK
void DisplayBuffer(unsigned char * buffer)
{
	unsigned char k; 
	unsigned char * display_buf;

	display_buf=buffer;

	debug_printf("\n");
	
	for(k=0;k<64;k++)
		{
		debug_printf("%02X ",display_buf[k]);
		if(k==31) /*分2行显示*/
			{
			debug_printf("\n");
			}
		}
}
#endif

void ClearDspBuffer(void)
{
	unsigned char i;

	for(i=0;i<64;i++)
		dsp_buf[i]=0;
}

/* 
 * @brief DSP 返回消息处理
 *
 *		Autodetection:0x8100 	0000 0xhhhhhhhh
 *		ACCN:		0xMM00   0000 0xhhhhhhhh 
 *					Dolby: 0xmm=0x90
 *					DTS:   0xmm=0x91
 *		PLL Out-of-lock:0x8100 0002 0xhhhhhhhh
 *					
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang C11 code
 * 		 AN288 p23
 */
 _AC3_STREAM current_ac3_stream;
_DTS_STREAM current_dts_stream;
extern unsigned char Pre_AC3_Type;
extern unsigned char Pre_DTS_Type; 
void AnalyseDspMsg(unsigned char *uCmdBuf, unsigned char uSize)
{
	unsigned char *msg_buf;
	msg_buf=uCmdBuf;	

#ifdef _DEBUG_DSP_TASK
	debug_printf("\nAnalyseDspMessage");
	DisplayBuffer(dsp_buf);
#endif    

	DelayMs(200); /*去掉DEBUG,造成高清流切换时出问题,故加此延时*/          
	while(uSize>=DSP_MSG_SIZE)       
		{
		switch(msg_buf[0])
			{
			case UNSOLICITED_MSG_ID: /*unsolicited message process*/
#ifdef _DEBUG_DSP_TASK
				debug_printf("\nunsolicited message process");		
#endif			
				DspProcUnsolicitedMsg(msg_buf);  
				break;
			case DOLBY_MSG_ID: /*ACCN-Dolby*/
#ifdef _DEBUG_DSP_TASK
				debug_printf("\nDolby Change Cfg");
#endif
				Pre_DTS_Type=0; /*清掉当前dts流信息*/
				if(DolbyType!=_DOLBY_PLUS) /*Dolby plus 高清流信息不需要分析流信息(也是0X90开头)*/
					{
					current_ac3_stream.acmode=(unsigned long)(msg_buf[7]&0x07);
					current_ac3_stream.lfeon=(unsigned long)(msg_buf[4]&0x01);
					current_ac3_stream.dsurmod=(unsigned long)((msg_buf[7]>>4)&0x03);
					current_ac3_stream.bsid=(unsigned long)(msg_buf[6]&0x1F);
					current_ac3_stream.timecod2=(unsigned long)((msg_buf[4]>>6)&0x03); //DSUREXMOD=Dolby Surround EX mode=10时			
					AnalyseAc3Cfg(current_ac3_stream);
					}				
				break;
			case DTS_MSG_ID: /*ACCN-DTS*/
#ifdef _DEBUG_DSP_TASK
				debug_printf("\nDTS Change Cfg");
#endif
				Pre_AC3_Type=0; /*清掉当前Dolby流信息*/
				current_dts_stream.amode =(unsigned long)msg_buf[7];			
				current_dts_stream.lfeon =(unsigned long)(msg_buf[4]&0x0F);
				current_dts_stream.pcmr =(unsigned long)((msg_buf[5]>>4)&0x0F);			
				current_dts_stream.ext_audio_id =(unsigned long)((msg_buf[6]>>4)&0x0F);
				current_dts_stream.ext_audio=(unsigned long)(msg_buf[6]&0x0F);			
				AnalyseDtsCfg(current_dts_stream);		 
				break;  
			case DOLBY_TURE_HD_ID:
#ifdef _DEBUG_DSP_TASK
				debug_printf("\nDOLBY TURE HD Change Cfg");
#endif				
				AnalyseTureHDCfg(msg_buf);
				break;
			case DTS_HD_MA_ID:
#ifdef _DEBUG_DSP_TASK
				debug_printf("\nDTS HD MA Change Cfg"); 
#endif					
				AnalyseDtsHDCfg(msg_buf);
				break;
			case DTS_HD_HRA_ID:
#ifdef _DEBUG_DSP_TASK
				debug_printf("\nDTS HD HRA Change Cfg"); 
#endif
				AnalyseDtsHRACfg(msg_buf);  
				break;
			default://2  //  2.4+
				break;
			}
		uSize-=DSP_MSG_SIZE;
		msg_buf+=DSP_MSG_SIZE;  
		}
}

/*
 * @brief  DSP 主循环
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang C11 code
 */
extern unsigned char PcmType;
extern unsigned char CD_Pure_Audio; // jian add in 12-01-31

#define DIGI_CHECK_TIME 30

void DspLoop(void)
{
	static unsigned char old_freq,cnter,old_cd_pcm = 0;
	unsigned char temp_freq;
	unsigned char Byte_num; 	  
	extern unsigned char Pre_value;

	if(!dsp_power_flag)	return;    

 	 /*实时检测模拟输入通道并处理*/ 
	 if((SenseAnalogSignal() == 1)&&Analog_Input_To_Dsp_Flag==1)  
	 	{
#ifdef _DEBUG_DSP_TASK	 	
	 	debug_printf("\nAnalog Input AutoDetect");
#endif  
		PcmType=_PCM_2_1;
	 	ProcInputSelect(sys_flag.zone1_channel);    
		sys_flag.stream_type =STREAM_PCM;
		PcmType=_PCM_2_1;
		Analog_Input_To_Dsp_Flag=0; 
		MsgSetDspChannel(ZONE1_ID,0); /*Channel Setting*/ 
		MsgSetDSPSetting(ZONE1_ID,0); /*故重新加载DSPSETTING*/ /*add by cjm 2009-4-7*/
	 	}

	/*如果DSP_Irq为低,表明DSP有消息返回--检测到输入流*/
	if(!SenseDspIrq())
		{	
		//SoftMute(1);  
		Byte_num=DspSpiReadNByte(dsp_buf,64); /*读DSP 返回的输入流消息*/		
		AnalyseDspMsg(dsp_buf, Byte_num); /*分析dspab_rd_buf 中DSPAB 消息*/ 
		ClearDspBuffer();

		//SoftMute(0); 
		}

	/*PCM 流切换时解码  

	*/
	if(dsp_time_tick)
		{
		dsp_time_tick=0;
		temp_freq=Cs8416GetFreq();

		if(old_freq!=temp_freq)
			{
			cnter++;
			if(cnter==10)
				{  
				old_freq=temp_freq;
				CurrentSampleFreq=temp_freq;
				if(stream_type==STREAM_PCM) /*由于Dolby 和DTS 的频率不会变，故这里就只做对PCM的处理*/
					{
#ifdef _DEBUG_DSP_TASK
					dsp_printf("\nnew freq:%d",temp_freq);
#endif 
					MsgSetStream(stream_type,0); 
					//CurrentIputPort=Digital_INPUT;
					//DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode); 	
					//DelayMs(500);	
					}
				}
			if(cnter>10)
				cnter=11;
			}
		else
			{
			cnter=0;
			}
		}	
 if(sys_flag.zone1_channel<=INPUT_TUNER)
 	{
	if(psm_8416_time_tick) // BY pass   JIAN ADD IN 12-01-31
		{
		psm_8416_time_tick = 0;

			#ifdef _DEBUG_DSP_TASK
			/*	dsp_printf("\n CD STERM1 :%d\n",Cs8416ReadByte(REG8416_CLK_RATIO));					*/
/*				dsp_printf("\n CD STERM2 :%d\n",Cs8416GetFreq());*/
			#endif 			 
           
			if((Cs8416GetFreq() == FREQ_44_1KHZ)&&(CheckCs8416Unlock(DIGI_CHECK_TIME) == 0)&&(IsCs8416Pcm()))	/*lock*/
				{
			#ifdef _DEBUG_DSP_TASK
	//			dsp_printf("\n CD STERM :%d \n",CD_Pure_Audio);
			#endif 			 	 	         

		                 if(CD_Pure_Audio == 1)
                	          	{
                	          	 //切换通道   走非DSP通道
					cd_dsp_channle_Select(1);
                          		}
				else
					{
					DSP_Select(1);
					}
					
				 }		         
			else
				{
					DSP_Select(1);
					#ifdef _DEBUG_DSP_TASK
//						dsp_printf("\n NO CD STERM \n");
					#endif 		
				 //切换通道 走DSP通道
				}
		}
}

}


