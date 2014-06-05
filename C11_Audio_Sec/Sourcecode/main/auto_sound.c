/*
测试IRC的时候，必须将BASS MANAGER 和 DELAY取消。
否则会影响测试结果。
*/
#include <string.h>
#include <stdio.h>            
#include "math.h"  

#include "macro.h"
#include "Task.h"
#include "Message.h"
#include "Vfd_disp.h"
#include "keydefine.h"
#include "setup_menu.h"  
#include "setup_func.h"
#include "Debug.h"
#include "C11_msg_handler.h"

#include "..\cs495300\dspdrv.h"
#include "..\cs495300\Irc2_Manager.h"
#include "..\cs495300\Irc1_Manager.h"  

#include "..\driver\Channel.h"
#include "..\driver\Volume.h"
#include "..\driver\Dac.h"

#ifdef _DEBUG
#define _DEBUG_AUTO_SOUND
#endif

#define DISTANCE_GAIN1 0.1711 /*实际测量使用值*/
#define DISTANCE_GAIN2 0.1711        

unsigned char auto_sound_testing = 0;  

enum{
IRC_METHOD_INIT = 0,
IRC_TEST_START,
IRC_TEST_DOING,
IRC_TEST_FINISH,
IRC_VEICEY_VOLMA_START1,
IRC_VEICEY_VOLMA_START2,
};

const unsigned short eq_freq[] = {
20,21,22,24,25,27,28,30,32,34,	/*0..9*/
36,38,40,42,45,47,50,53,56,60,	/*10..19*/
63,67,71,75,79,84,89,94,100,106,	/*20..29*/
112,119,126,133,141,150,159,168,178,188,	/*30..39*/
200,211,224,237,251,266,282,299,316,335,	/*40..49*/
355,376,398,422,447,473,501,531,563,596,	/*50..59*/
631,668,708,750,794,841,891,944,1000,1059,/*60..69*/
1122,1189,1259,1334,1413,1496,1585,1679,1778,1884,/*70..79*/
1995,2114,2239,2371,2512,2661,2818,2985,3162,3350,/*80..89*/
3548,3758,3981,4217,4467,4732,5012,5309,5623,5957,/*90..99*/
6310,6683,7080,7499,7943,8414,8913,9441,10000,10593,	/*100..109*/
11220,11885,12589,13335,14125,14962,15849,16788,17783,18837,	/*110..119*/
19953
};

TASK_RESULT ASProcSetupConfig(unsigned char *buffer);
TASK_RESULT ASProcSpeakerCheck(unsigned char *buffer);
TASK_RESULT ASProcSetDistance(unsigned char *buffer);
TASK_RESULT ASProcSetBalance(unsigned char *buffer);
TASK_RESULT ASProcSetCrossoverPoints(unsigned char *buffer);
TASK_RESULT ASProcSetRoomEQ(unsigned char *buffer);
TASK_RESULT ASProcSetSpeakerphase(unsigned char *buffer);

TASK_ID bak_id;

char temp_buffer[] = "                    ";
unsigned char current_test_spk = 0;
unsigned char bar_counter = 0;

char NameAutoSoundSetup[] = "Auto Sound SETUP?   ";
char NameEnterStart[] = "Press OK to Start";
char NameEnterNow[] = "Press OK now     ";
char NameConnectMic[] = "Connect Microphone? ";
char NameSpeakerCheck[] = "Speaker Check       ";
char NameSetDistance[] = "Set Distance        ";
char NameSetBalance[] = "Set Balance         ";
char NameSetCrossoverPoint[] = "Set Crossover Points";
char NameSetRoomEq[] = "Set Room EQ         ";
char NameSetSpeakerPhase[] = "Set Speaker phase   ";

typedef struct{
char * name;
TASK_PROC run;
}IRC_TASK;

IRC_TASK irc_setupconfig = {NameAutoSoundSetup,ASProcSetupConfig};
IRC_TASK irc_speakercheck = {NameSpeakerCheck,ASProcSpeakerCheck};
IRC_TASK irc_setdistance= {NameSetDistance,ASProcSetDistance};
IRC_TASK irc_setbalance= {NameSetBalance,ASProcSetBalance};
IRC_TASK irc_setroomeq = {NameSetRoomEq,ASProcSetRoomEQ};
IRC_TASK irc_setcrossoverpoint= {NameSetCrossoverPoint,ASProcSetCrossoverPoints};
IRC_TASK irc_setspeakerphase= {NameSetSpeakerPhase,ASProcSetSpeakerphase};
extern float ConvertBalanceValue(float float_value);

unsigned char current_step = 0;

const IRC_TASK * IrcTaskTable[] = 
{
&irc_setupconfig,
&irc_speakercheck,
&irc_setdistance,
&irc_setbalance,
&irc_setroomeq,
&irc_setcrossoverpoint,
&irc_setspeakerphase,
NULL,
};

BOOL MsgSetIrc(unsigned char msg_type,unsigned char channel)
{
	unsigned char buffer[MESSAGE_SIZE] = {0};
	buffer[0] = MSG_Irc;
	buffer[1] = msg_type;
	buffer[2] = channel;
	return SendMessage(AUTO_SOUND_ID,buffer);
}

void AutoSoundInit(void)
{
	bak_id = GetCurFocusTid();
	SetCurFocusTid(AUTO_SOUND_ID);
	current_step = 0;
	MsgSetIrc(IRC_METHOD_INIT,0);
	SoftMute(0);
}

void AutoSoundExit()
{
	auto_sound_testing = 0;
	SetCurFocusTid(bak_id); 
	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_NONE);
	MsgSetVfdShow(bak_id,0);
	AnalogInputTypeSelect(Select_None_Input);
	MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
	Set7Ch1MainVolume(sys_flag.zone1_volume);	/*设置为系统音量*/
}

void InitSpkPara()
{
	unsigned char num = 0;
	for(num = 0;speaker_table[num];num++)
		{
		speaker_table[num]->balance = 0;
		speaker_table[num]->valid = 0;
		speaker_table[num]->phase = 0;
		speaker_table[num]->distance = 0;
		speaker_table[num]->crossover_point[0] = 0;
		speaker_table[num]->crossover_point[1] = 0;
		speaker_table[num]->crossover_point[2] = 0;
		speaker_table[num]->eq[0] = 0;
		speaker_table[num]->eq[1] = 0;
		speaker_table[num]->eq[2] = 0;
		speaker_table[num]->eq[3] = 0;
		speaker_table[num]->eq[4] = 0;
		speaker_table[num]->eq[5] = 0;
		speaker_table[num]->eq[6] = 0;
		speaker_table[num]->eq[7] = 0;
		speaker_table[num]->eq[8] = 0;
		speaker_table[num]->eq[9] = 0;
		}
}

uchar IsAnySpkValid(void)
{
	unsigned char num = 0;
	for(num = 0;speaker_table[num];num++)
		{
		if(speaker_table[num]->valid)
			return num + 1;
		}
	return 0;
}

void DisplayConnectHelp(void)
{
	VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,"SPEAKER NOT FOUND   ");
	VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,"PLEASE CHECK...     ");
}

void DisplayNA(uchar line_start)
{
	while(line_start < 17)
		{
		VfdDisplay(VFD_LINE2_START_POSITION+line_start,VFD_UPDATE_NONE,".");
		line_start ++;
		}
	VfdDisplay(VFD_LINE2_START_POSITION+line_start,VFD_UPDATE_LINE2,"N/A");
}

extern unsigned long DspReadIrc1(unsigned char index);
unsigned short GetIrc2EqValue(uchar channel)
{
	short i = 0;
	float gain,crossover_point = 100;
	uchar crossover_index = 0x0;
	unsigned long temp_value = 0;
	float temp_eq[128];  

	for(i=0;i<121;i++)
		{
		temp_value = DspReadIrc1(i+0x20);
		temp_eq[i] = ReConvert_8_24_Value(temp_value);
		}

#if 0//def _DEBUG_AUTO_SOUND  
	for(i=0;i<121;i++)  
		debug_printf("%02.1f, ",temp_eq[i]);
	debug_printf("\n");
#endif
	//fir(temp_eq,121,coe,COE_SIZE);
#if 0//def _DEBUG_AUTO_SOUND
	debug_printf("FIR:");
	for(i=0;i<121;i++)
		debug_printf("%02.1f, ",temp_eq[i]);
	debug_printf("\n");
#endif
	if(channel == CH_SW)
		{
		i=0x23;
		crossover_point = fabs(temp_eq[i]);
		crossover_index = i;
		for(;i>=0x0c;i--)
			{
			gain = fabs(temp_eq[i]);	//查找0dB 点
			if(gain == 0)	//查找到第一个点，就退出
				{
				crossover_index = i;
				break;
				}
			if(gain < crossover_point)
				{
				crossover_point = gain;
				crossover_index = i;
				}
			}
		}
	else
		{
		i=0x0c;
		crossover_point = fabs(temp_eq[i]);
		crossover_index = i;
	//	i=0x21;		

		for(;i<=0x23;i++) // jian chang it in 12-03-15
		//for(;i>=0;i--)
			{
			gain = fabs(temp_eq[i] + 3);	/*查找-3dB 点*/
			if(gain == 0)	/*查找到第一个点，就退出*/
				{
				//
				crossover_index = i;
				break;
				}
			if(gain < crossover_point)
				{
				crossover_point = gain;
				crossover_index = i;
				}
			}
		
		}
		
	if(speaker_table[current_test_spk]->channel->spk_index == CH_SW)
		{
		speaker_table[current_test_spk]->eq[0] += temp_eq[0x4];	//25.1Hz
		speaker_table[current_test_spk]->eq[1] += temp_eq[0xC];	//39.8Hz
		speaker_table[current_test_spk]->eq[2] += temp_eq[0x10];	//50.1Hz
		speaker_table[current_test_spk]->eq[3] += temp_eq[0x14];	//63.1Hz
		speaker_table[current_test_spk]->eq[4] += temp_eq[0x16];	//70.8Hz
		speaker_table[current_test_spk]->eq[5] += temp_eq[0x18];	//79.4Hz
		speaker_table[current_test_spk]->eq[6] += temp_eq[0x1a];	//89.1Hz
		speaker_table[current_test_spk]->eq[7] += temp_eq[0x1c];	//100.0Hz
		speaker_table[current_test_spk]->eq[8] += temp_eq[0x1e];	//112.2	
		speaker_table[current_test_spk]->eq[9] += temp_eq[0x20];   //125.9
		}
	else
		{
		speaker_table[current_test_spk]->eq[0] += temp_eq[0x18];	//79.4Hz
		speaker_table[current_test_spk]->eq[1] += temp_eq[0x24];	//158.5Hz
		speaker_table[current_test_spk]->eq[2] += temp_eq[0x2c];	//251.2Hz
		speaker_table[current_test_spk]->eq[3] += temp_eq[0x38];	//501.2Hz
		speaker_table[current_test_spk]->eq[4] += temp_eq[0x44];	//1000.0Hz
		speaker_table[current_test_spk]->eq[5] += temp_eq[0x50];	//1995.3Hz
		speaker_table[current_test_spk]->eq[6] += temp_eq[0x5c];	//3981.3Hz
		speaker_table[current_test_spk]->eq[7] += temp_eq[0x68];	//7943.3Hz
		speaker_table[current_test_spk]->eq[8] += temp_eq[0x6f];	//11885.0Hz
		speaker_table[current_test_spk]->eq[9] += temp_eq[0x74];   //15849.0Hz		
		}

	return eq_freq[crossover_index];
}
/*

*/
TASK_RESULT ASProcSetupConfig(unsigned char *buffer)
{
	static uchar enable = 0;
	static uchar setup_config_status = 0;
	switch(buffer[0])
		{
		case MSG_KeyPress:
			switch(buffer[1])
				{
				case _REMOTE_ENTER_KEY:
				case _PANEL_ENTER_KEY:
					MsgSetIrc(IRC_TEST_START,setup_config_status);  
					break;
				case _REMOTE_TEST_KEY:
					AutoSoundExit();
					break;
				}
			break;
		case MSG_Irc:
			switch(buffer[1])
				{
				case IRC_METHOD_INIT:	
					enable = 1;
					setup_config_status = 1;
					VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,IrcTaskTable[current_step]->name); /*Auto Sound SETUP? */
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,NameEnterStart); /*Press ENTER to Start*/
					break;
				case IRC_TEST_START:
					if(!enable)
						return FALSE;

					switch(setup_config_status)
						{
						case 1:
							VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,NameConnectMic); /*Connect Microphone?*/
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,NameEnterStart); /*Press ENTER to Start*/
							setup_config_status++;
							break;
						case 2:
							MsgSetIrc(IRC_TEST_FINISH,0);
							break;
						}
					break;/**/
				case IRC_TEST_FINISH:
					enable = 0;
					current_step++;
					if(IrcTaskTable[current_step])
						{
						MsgSetIrc(IRC_METHOD_INIT,0);
						}
					else
						{
						assert(IrcTaskTable[current_step] == NULL);
						}
					break;
				}
			break;
		}
	return TRUE;
}
//
TASK_RESULT ASProcSpeakerCheck(unsigned char *buffer)
{
	static uchar enable = 0,as_key_enable = 0;
	static unsigned char total_spk = 0;
	unsigned long spk_status;
	uchar i;
	float balance = 0.0;
	static short save_vom = 0;
	static char timer_cnt= 0;
	switch(buffer[0])
		{
		case MSG_KeyPress:
			switch(buffer[1])
				{
				case _REMOTE_ENTER_KEY:
				case _PANEL_ENTER_KEY:
					if(as_key_enable)
						{
						if(total_spk)
							{
							MsgSetIrc(IRC_TEST_FINISH,0);
	//						IRC_VEICEY_VOLMA
						//	MsgSetIrc(IRC_VEICEY_VOLMA_START1,0); // jian add 12-03-20
							
							}
						else
							{
							for(i=0;i<3;i++)
								{
								DisplayConnectHelp();
								DelayMs(300);
								VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_LINE_ALL);
								DelayMs(200);
								}
							AutoSoundExit();
							}
						}
					break;
				case _REMOTE_TEST_KEY:
					AutoSoundExit();
					break;
				}
			break;//
		case MSG_Irc:
			switch(buffer[1])
				{
				case IRC_METHOD_INIT:
            				save_vom = sys_flag.zone1_volume;
					AnalogInputTypeSelect(Select_MIC_INPUT); /*MIC Input*/
					//AutoSoundSetVolume(-20); /*强制-20dB输出*/        
					Auto_Setup_DAC_Phase(); /*所有通道设置dac in phase*/
					auto_sound_testing = 1;
					SetDspIrc(IRC_TEST_INIT,0); /*Load IRC2 code and init*/
					Analog7CH1OutSelectChannel(SOURCE_DSP);  

					enable = 1;       
					as_key_enable = 0;    
					total_spk = 0;  
  //
					VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Set Test Tone Level"); /*Speaker check*/  
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2); /*清空第二行*/
					InitSpkPara();	/*从此处开始测试，清除所有的状态*/
					
					current_test_spk = 0;  
					bar_counter = 0;
					MsgSetIrc(IRC_VEICEY_VOLMA_START1,current_test_spk);  					
//					MsgSetIrc(IRC_TEST_START,current_test_spk);  
					break;  
				case IRC_TEST_START:         
					if(!enable)  
						return FALSE;     
					VfdDisplayBlank(VFD_LINE1,VFD_UPDATE_LINE1); /*清空第二行*/
					VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,IrcTaskTable[current_step]->name); /*Speaker check*/
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE); /*清空第二行*/
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,speaker_table[current_test_spk]->channel->name);
					
					current_test_spk = buffer[2];    
					bar_counter = strlen(speaker_table[current_test_spk]->channel->name);

					SetDspIrc(IRC_TEST_AND_SETUP_DELAY,speaker_table[current_test_spk]->channel->spk_bit);
					MsgSetIrc(IRC_TEST_DOING,current_test_spk);
					break;//
				case IRC_TEST_DOING:
					if(!enable)   
						return FALSE;      

					if(IsIrc2ValidResult() == TRUE) /*看是否是有效的结果*/
						{
#ifdef _DEBUG_AUTO_SOUND						
						//debug_printf("\nIRC2_Speaker_Status:");
#endif
						spk_status = DspReadIrc2(0x02);
						if(spk_status & speaker_table[current_test_spk]->channel->spk_bit)
							{
							VfdDisplay(VFD_LINE2_START_POSITION+17,VFD_UPDATE_LINE2," No");
							speaker_table[current_test_spk]->valid = 0;
							}
						else
							{
							total_spk ++;
							VfdDisplay(VFD_LINE2_START_POSITION+17,VFD_UPDATE_LINE2,"Yes");
							speaker_table[current_test_spk]->valid = 1;

#ifdef _DEBUG_AUTO_SOUND						
							//debug_printf("\nIRC-1_Distance:");
#endif
							speaker_table[current_test_spk]->distance = GetSpkDistance(); /*Get SPK Distance*/
							if(speaker_table[current_test_spk]->channel->spk_index == CH_SW) 
								speaker_table[current_test_spk]->distance *= DISTANCE_GAIN1;/*得到英寸*/	
							else
								speaker_table[current_test_spk]->distance *= DISTANCE_GAIN2;/*得到英寸*/	
							speaker_table[current_test_spk]->distance=speaker_table[current_test_spk]->distance/12; /*换算成英尺*/ 
							if(speaker_table[current_test_spk]->distance > 100)	/*限定范围*/
								speaker_table[current_test_spk]->distance = 100;
#ifdef _DEBUG_AUTO_SOUND						
							//debug_printf("\nIRC-1_Polarity:");
#endif
							spk_status = DspReadIrc1(0x1A); /*Get SPK Phase*/
#ifdef _DEBUG_AUTO_SOUND
							//debug_printf("\nspk_status=%d",spk_status); 
#endif							
							speaker_table[current_test_spk]->phase = (uchar)spk_status; /*强制转换*/ 
							}
						DelayMs(800);
						current_test_spk ++;
						if(speaker_table[current_test_spk])
							{
							MsgSetIrc(IRC_TEST_START,current_test_spk);
							}
						else
							{
							as_key_enable = 1;
							VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,"Total %1d Speakers    ",total_spk);
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,NameEnterNow);
							}
						}
					else /*不是有效的结果则显示"."*/
						{
						MsgSetIrc(IRC_TEST_DOING,current_test_spk);
						if(bar_counter < 17)
							{
							VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_LINE2,".");
							bar_counter ++;
							}
						else
							{
							DspMasterBoot();// jian add in 10-09-02
							}

						DelayMs(300);
						}  
					break;
				case IRC_TEST_FINISH: 
					enable = 0;
					current_step++;
					if(IrcTaskTable[current_step])
						{
						MsgSetIrc(IRC_METHOD_INIT,0);  
						}
					else
						{
						assert(IrcTaskTable[current_step] == NULL);
						}
					break;

				case IRC_VEICEY_VOLMA_START1:
						SetDspIrc(IRC_TEST_AND_SETUP_TRIMS,speaker_table[0]->channel->spk_bit);
						MsgSetIrc(IRC_VEICEY_VOLMA_START2,current_test_spk);
						bar_counter  = 0;
						DelayMs(1000);						
	
					break;
				case IRC_VEICEY_VOLMA_START2:
					     if(IsIrc2ValidResult() == TRUE)
						{  
#ifdef _DEBUG_AUTO_SOUND
						debug_printf("\n CHANGE TEST :");
#endif

						spk_status = DspReadIrc1(0x19);        
						balance = ReConvert_8_24_Value(spk_status); /*得到当前喇叭反馈的DB值*/													
						balance = ConvertBalanceValue(balance);
						timer_cnt++;	
						if(timer_cnt>10)
						{
							timer_cnt	 = 0;
							Set7Ch1MainVolume(sys_flag.zone1_volume); 							       
							SoftMute(1);
							DspMasterBoot();// jian add in 10-09-02										
							VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE); /*清空第二行*/
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"NO F-Left SPK");
							SoftMute(0);
						}
						else
						{
						if(fabs(7.0- balance)>=0.5)
							{
							save_vom = save_vom+(7-(uchar)balance);
							if(save_vom  <= -10.0) // 修改了退出DB值，由原来的0.0到 -10.0
								{
								Set7Ch1MainVolume(save_vom); 
								MsgSetIrc(IRC_VEICEY_VOLMA_START1,current_test_spk);						
								
								}
							else
								{
	 							Set7Ch1MainVolume(sys_flag.zone1_volume); 							       
								SoftMute(1);
								DspMasterBoot();// jian add in 10-09-02										
								VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE); /*清空第二行*/
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"NO F-Left SPK");
								timer_cnt = 0;
								SoftMute(0);								       
								}
							}
						else
						
							{
							MsgSetIrc(IRC_TEST_START,current_test_spk);		
							timer_cnt = 0;
							}

						}	
#ifdef _DEBUG_AUTO_SOUND
//
						debug_printf("\n MainVolume :%d\n",save_vom);
#endif

						}		
					else
					{

						if(bar_counter < 10)
							{
							MsgSetIrc(IRC_VEICEY_VOLMA_START2,current_test_spk);
							bar_counter ++;
							DelayMs(700);      
							}
						else
							{
								Set7Ch1MainVolume(sys_flag.zone1_volume); 							       
								SoftMute(1);
								DspMasterBoot();// jian add in 10-09-02										
								VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE); /*清空第二行*/
								VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,"NO F-Left SPK");
								timer_cnt = 0;
								SoftMute(0);							
							}




					}
					break;
				}
			break;
		}
	return TRUE;
}

TASK_RESULT ASProcSetDistance(unsigned char *buffer)
{
	static uchar enable = 0;
	float _distance = 0;
	uchar i;
	
	switch(buffer[0])
		{
		case MSG_KeyPress:
			switch(buffer[1])
				{
				case _REMOTE_ENTER_KEY:
				case _PANEL_ENTER_KEY:
					MsgSetIrc(IRC_TEST_FINISH,0);
					break;
				case _REMOTE_TEST_KEY:
					AutoSoundExit();
					break;
				}
			break;
		case MSG_Irc:
			switch(buffer[1])
				{
				case IRC_METHOD_INIT:
					enable = 1;
					VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,IrcTaskTable[current_step]->name); /*Set Distance*/
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2);

					current_test_spk = 0;
					MsgSetIrc(IRC_TEST_START,current_test_spk);
					break;
				case IRC_TEST_START:
					if(!enable)
						return FALSE;

					bar_counter = strlen(speaker_table[current_test_spk]->channel->name);
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_NONE,speaker_table[current_test_spk]->channel->name);

					MsgSetIrc(IRC_TEST_DOING,current_test_spk);
					break;
				case IRC_TEST_DOING:
					if(!enable)
						return FALSE;
//
					if(speaker_table[current_test_spk])
						{
						if(speaker_table[current_test_spk]->valid) /*Speaker 存在*/
							{
#ifdef _DEBUG_AUTO_SOUND
							//debug_printf("Distance_TEST:%s\n",speaker_table[current_test_spk]->channel->name);
#endif							
							_distance = speaker_table[current_test_spk]->distance / METER2FEET;	/*换算成meter单位*/

							sprintf(temp_buffer,"%3.1fft %2.1fM",speaker_table[current_test_spk]->distance,_distance);
							i = strlen(temp_buffer);
							while(bar_counter < (VFD_LINE2_LENGTH - i))
								{
								VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_NONE,".");
								bar_counter ++;
								}
							VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_LINE2,temp_buffer);
							}
						else
							{
							DisplayNA(bar_counter);
							}
						DelayMs(1000);  
						}

					current_test_spk ++;
					if(speaker_table[current_test_spk])
						{
						MsgSetIrc(IRC_TEST_START,current_test_spk);
						}
					else
						{
						MsgSetIrc(IRC_TEST_FINISH,0);
						}
					break;
				case IRC_TEST_FINISH:
					enable = 0;
					current_step++;
					if(IrcTaskTable[current_step])
						{
						MsgSetIrc(IRC_METHOD_INIT,0);
						}
					else
						{
						assert(IrcTaskTable[current_step] == NULL);
						}
					break;
				}
			break;
		}
	return TRUE;
}


TASK_RESULT ASProcSetBalance(unsigned char *buffer)
{
	static uchar enable = 0;
	unsigned long spk_status;
	uchar i;
	float temp_blance = 0.0;
	switch(buffer[0])
		{
		case MSG_KeyPress:
			switch(buffer[1])
				{
				case _REMOTE_ENTER_KEY:
				case _PANEL_ENTER_KEY:
					MsgSetIrc(IRC_TEST_FINISH,0);
					break;
				case _REMOTE_TEST_KEY:
					AutoSoundExit();
					break;
				}
			break;
		case MSG_Irc:
			switch(buffer[1])
				{
				case IRC_METHOD_INIT:
					enable = 1;
					VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,IrcTaskTable[current_step]->name); /*Set Balance*/
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2);

					current_test_spk = 0;
					MsgSetIrc(IRC_TEST_START,current_test_spk);  
					break;
				case IRC_TEST_START:
					if(!enable)
						return FALSE;

					bar_counter = strlen(speaker_table[current_test_spk]->channel->name);
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,speaker_table[current_test_spk]->channel->name);

					if(speaker_table[current_test_spk]->valid)
						{
						SetDspIrc(IRC_TEST_AND_SETUP_TRIMS,speaker_table[current_test_spk]->channel->spk_bit);
						MsgSetIrc(IRC_TEST_DOING,current_test_spk);
						DelayMs(1000);						
						}
					else  
						{
						DisplayNA(bar_counter);
						DelayMs(1000);

						current_test_spk++;
						if(speaker_table[current_test_spk]) /*还要继续测试*/
							{
							MsgSetIrc(IRC_TEST_START,current_test_spk);
							}
						else
							{
							MsgSetIrc(IRC_TEST_FINISH,0);
							}
						}
					break;
				case IRC_TEST_DOING:
					if(!enable)
						return FALSE;

					if(IsIrc2ValidResult() == TRUE)
						{  
#ifdef _DEBUG_AUTO_SOUND
						debug_printf("Balance_TEST:%s\n",speaker_table[current_test_spk]->channel->name);
#endif
#ifdef _DEBUG_AUTO_SOUND
						debug_printf("\nIRC-1_Level:");
#endif
						spk_status = DspReadIrc1(0x19);        
						speaker_table[current_test_spk]->balance = ReConvert_8_24_Value(spk_status); /*得到当前喇叭反馈的DB值*/													
						speaker_table[current_test_spk]->balance = ConvertBalanceValue(speaker_table[current_test_spk]->balance);

#if 0					      
						if(speaker_table[current_test_spk]->channel->spk_index == CH_SW) /*进行DB 值补偿*/
							speaker_table[current_test_spk]->balance = speaker_table[current_test_spk]->balance * (-0.75) - 12; /*根据实际的测试做的调整*/
						else
							//speaker_table[current_test_spk]->balance = speaker_table[current_test_spk]->balance * (-1.75);    	
							speaker_table[current_test_spk]->balance = speaker_table[current_test_spk]->balance * (-2);  
				  
						speaker_table[current_test_spk]->balance = ConvertBalanceValue(speaker_table[current_test_spk]->balance); 	
						if(speaker_table[current_test_spk]->balance > 15)	/*限定范围*/
							speaker_table[current_test_spk]->balance = 15;
						if(speaker_table[current_test_spk]->balance < -15)
							speaker_table[current_test_spk]->balance = -15;    
#endif									
#ifdef _DEBUG_AUTO_SOUND
						debug_printf("Balance:%f dB\n",speaker_table[current_test_spk]->balance);
#endif		

#if 0
						speaker_table[0]->balance  =                                                 //fl
						speaker_table[1]->balance  =                                                 //cen
						speaker_table[2]->balance  =                                                 //fr
						speaker_table[3]->balance  =                                                 //sr
						speaker_table[4]->balance  =                                                 //sbr
						speaker_table[5]->balance  =                                                 //sbl
						speaker_table[6]->balance  =                                                 //sl
						speaker_table[7]->balance  =                                                 //sw

#endif
                                                 switch(current_test_spk)
                                                 {
                                                 case 0://fl
                                                          temp_blance = 0.0;
                                               		  break;
                                                 case 1://cen
                                                          speaker_table[1]->balance  = speaker_table[0]->balance -speaker_table[1]->balance ;
                                                          temp_blance = speaker_table[1]->balance;
                                               	  	break;
                                                 case 2://fr
                                                          speaker_table[2]->balance  = speaker_table[0]->balance -speaker_table[2]->balance ;
                                                          temp_blance = speaker_table[2]->balance;
                                                	 break;
                                                 case 3://sr
                                                          speaker_table[3]->balance  = speaker_table[0]->balance -speaker_table[3]->balance ;
                                                          temp_blance = speaker_table[3]->balance;
                                                 	break;
                                                 case 4://sbr
                                                          speaker_table[4]->balance  = speaker_table[0]->balance -speaker_table[4]->balance ;
                                                          temp_blance = speaker_table[4]->balance;
                                                	 break;
                                                 case 5://sbl
                                                          speaker_table[5]->balance  = speaker_table[0]->balance -speaker_table[5]->balance ;
                                                          temp_blance = speaker_table[5]->balance;
                                                 	break;
                                                 case 6://sl
                                                          speaker_table[6]->balance  = speaker_table[0]->balance -speaker_table[6]->balance ;
                                                          temp_blance = speaker_table[6]->balance;
                                                 	break;
                                                 case 7://sw
                                                          speaker_table[7]->balance  = speaker_table[0]->balance -speaker_table[7]->balance ;
                                                          temp_blance = speaker_table[7]->balance;
                                                 	break;

                                                 }
 #ifdef _DEBUG_AUTO_SOUND
						debug_printf("Balance:%f dB\n",temp_blance);
#endif	                                               

						sprintf(temp_buffer,"%3.1fdB",temp_blance);  
						i = strlen(temp_buffer);
						
						speaker_table[current_test_spk]->crossover_point[0] = GetIrc2EqValue(speaker_table[current_test_spk]->channel->spk_index);					
#ifdef _DEBUG_AUTO_SOUND
						//debug_printf("crossover:%dHz\n",speaker_table[current_test_spk]->crossover_point[0]); 
#endif					
						while(bar_counter < (VFD_LINE2_LENGTH - i))
							{
							VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_NONE,".");
							bar_counter ++;
							}
						VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_LINE2,temp_buffer);
						DelayMs(1000);
						current_test_spk ++;  
						if(speaker_table[current_test_spk])
							{
							MsgSetIrc(IRC_TEST_START,current_test_spk);
							}
						else
							{
							MsgSetIrc(IRC_TEST_FINISH,0);
							speaker_table[0]->balance = 0.0;
							}
						}
					else
						{
						MsgSetIrc(IRC_TEST_DOING,current_test_spk);
						if(bar_counter < 12)
							{
							VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_LINE2,".");
							bar_counter ++;
							}
						else
							{
							DspMasterBoot();// jian add in 10-09-02 
							}

						DelayMs(700);      
						}
					break;
				case IRC_TEST_FINISH:
					enable = 0;
					current_step++;
					if(IrcTaskTable[current_step])
						{
						MsgSetIrc(IRC_METHOD_INIT,0);
						}
					else
						{
						assert(IrcTaskTable[current_step] == NULL);
						}
					break;
				}
			break;  
		}
	return TRUE;
}

uchar sort(uchar *buf)
{
	uchar i,j,max_value;

	for(i=0;i<3;i++)
		{
		max_value = buf[i];
		for(j=i+1;j<3;j++)
			{
			if(max_value < buf[j])
				{
				buf[i] = buf[j];	/*交换*/
				buf[j] = max_value;
				max_value = buf[i];
				}
			}
		}
	return buf[1];
}

TASK_RESULT ASProcSetCrossoverPoints(unsigned char *buffer)
{
	static uchar enable = 0;
	uchar point = 0;
	switch(buffer[0])
		{
		case MSG_KeyPress:
			switch(buffer[1])
				{
				case _REMOTE_ENTER_KEY:
				case _PANEL_ENTER_KEY:
					MsgSetIrc(IRC_TEST_FINISH,0);
					break;
				case _REMOTE_TEST_KEY:
					AutoSoundExit();
					break;
				}
			break;
		case MSG_Irc:
			switch(buffer[1])
				{
				case IRC_METHOD_INIT:
					enable = 1;
					VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,IrcTaskTable[current_step]->name); /*Set Crossover Points*/
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2);

					current_test_spk = 0;
					MsgSetIrc(IRC_TEST_START,current_test_spk);
					break;
				case IRC_TEST_START:
					if(!enable)
						return FALSE;

					bar_counter = strlen(speaker_table[current_test_spk]->channel->name);
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,speaker_table[current_test_spk]->channel->name);

					if(speaker_table[current_test_spk]->valid)
						{
						SetDspIrc(IRC_TEST_AND_SETUP_EQ,speaker_table[current_test_spk]->channel->spk_bit);
						MsgSetIrc(IRC_TEST_DOING,current_test_spk);
						}
					else
						{
						DisplayNA(bar_counter);
						DelayMs(1000);

						current_test_spk++;
						if(speaker_table[current_test_spk])	/*还要继续测试*/
							{
							MsgSetIrc(IRC_TEST_START,current_test_spk);
							}
						else
							{
							MsgSetIrc(IRC_TEST_FINISH,0);
							}
						}
					break;
				case IRC_TEST_DOING:
					if(!enable)
						return FALSE;

					if(IsIrc2ValidResult() == TRUE)
						{
#ifdef _DEBUG_AUTO_SOUND
						//debug_printf("CrossoverPoints_TEST:%s\n",speaker_table[current_test_spk]->channel->name);
#endif
						point=speaker_table[current_test_spk]->crossover_point[0];
						VfdDisplay(VFD_LINE2_START_POSITION+15,VFD_UPDATE_LINE2,"%3dHz",point);
#ifdef _DEBUG_AUTO_SOUND
						//debug_printf("%s:%dHz\n",speaker_table[current_test_spk]->channel->name,point);
#endif						
						DelayMs(1000);
						current_test_spk ++;
						if(speaker_table[current_test_spk])
							{
							MsgSetIrc(IRC_TEST_START,current_test_spk);
							}
						else
							{
							MsgSetIrc(IRC_TEST_FINISH,0);
							}
						}
					else
						{
						MsgSetIrc(IRC_TEST_DOING,current_test_spk);
						if(bar_counter < 15)
							{
							VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_LINE2,".");
							bar_counter ++;
							}
						DelayMs(1500);
						}
					break;
				case IRC_TEST_FINISH:
					enable = 0;
					current_step++;
					if(IrcTaskTable[current_step])
						{
						MsgSetIrc(IRC_METHOD_INIT,0);
						}
					else
						{
						assert(IrcTaskTable[current_step] == NULL);
						}
					break;
				}
			break;
		}
	return TRUE;
}
//
TASK_RESULT ASProcSetRoomEQ(unsigned char *buffer)
{
	static uchar enable = 0;
	
	switch(buffer[0])
		{
		case MSG_KeyPress:  
			switch(buffer[1])
				{
				case _REMOTE_ENTER_KEY:
				case _PANEL_ENTER_KEY:
					MsgSetIrc(IRC_TEST_FINISH,0);
					break;
				case _REMOTE_TEST_KEY:
					AutoSoundExit();
					break;
				}
			break;
		case MSG_Irc:
			switch(buffer[1])
				{
				case IRC_METHOD_INIT:
					enable = 1;
					VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,IrcTaskTable[current_step]->name); /*Set Room EQ*/
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2);

					current_test_spk = 0;
					MsgSetIrc(IRC_TEST_START,current_test_spk);
					break;
				case IRC_TEST_START:
					if(!enable)
						return FALSE;

					bar_counter = strlen(speaker_table[current_test_spk]->channel->name);
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,speaker_table[current_test_spk]->channel->name);

					if(speaker_table[current_test_spk]->valid)
						{
						SetDspIrc(IRC_TEST_AND_SETUP_EQ,speaker_table[current_test_spk]->channel->spk_bit);
						MsgSetIrc(IRC_TEST_DOING,current_test_spk);
						}
					else
						{
						DisplayNA(bar_counter);
						DelayMs(1000);

						current_test_spk++;
						if(speaker_table[current_test_spk])	/*还要继续测试*/
							{
							MsgSetIrc(IRC_TEST_START,current_test_spk);
							}
						else
							{
							MsgSetIrc(IRC_TEST_FINISH,0);
							}
						}
					break;
				case IRC_TEST_DOING:
					if(!enable)
						return FALSE;

					if(IsIrc2ValidResult() == TRUE)
						{
#ifdef _DEBUG_AUTO_SOUND
						//debug_printf("RoomEQ_TEST:%s\n",speaker_table[current_test_spk]->channel->name);
#endif
						speaker_table[current_test_spk]->eq[0] /= 3;
						speaker_table[current_test_spk]->eq[1] /= 3;
						speaker_table[current_test_spk]->eq[2] /= 3;
						speaker_table[current_test_spk]->eq[3] /= 3;
						speaker_table[current_test_spk]->eq[4] /= 3;
						speaker_table[current_test_spk]->eq[5] /= 3;
						speaker_table[current_test_spk]->eq[6] /= 3;
						speaker_table[current_test_spk]->eq[7] /= 3;
						speaker_table[current_test_spk]->eq[8] /= 3;
						speaker_table[current_test_spk]->eq[9] /= 3;
						VfdDisplay(VFD_LINE2_START_POSITION+17,VFD_UPDATE_LINE2,"SET");

						DelayMs(1000);
						current_test_spk ++;
						if(speaker_table[current_test_spk])
							{
							MsgSetIrc(IRC_TEST_START,current_test_spk);
							}
						else 
							{
							MsgSetIrc(IRC_TEST_FINISH,0);
							}
						}
					else
						{
						MsgSetIrc(IRC_TEST_DOING,current_test_spk);
						if(bar_counter < 17)
							{
							VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_LINE2,".");
							bar_counter ++;
							}
						DelayMs(1500);
						}
					break;
				case IRC_TEST_FINISH:
					enable = 0;
					current_step++;
					if(IrcTaskTable[current_step])
						{
						MsgSetIrc(IRC_METHOD_INIT,0);
						}
					else
						{
						assert(IrcTaskTable[current_step] == NULL);
						}
					break;
				}
			break;
		}
	return TRUE;
}

static uchar GetCrossoverPoint(uchar crossover)
{
	float temp_value = crossover;

	temp_value = temp_value/10-2;

	return (uchar)temp_value;
}

extern const float balance_table[]; 
unsigned char FindBalancePosition(float balance_value)
{
	unsigned char i;

	balance_value *= 2;
	balance_value = (short)balance_value;
	balance_value /= 2;

	for(i=0;i<61;i++)
		{
		if(balance_value == balance_table[i])
			{
			return i;
			}
		}
	if(balance_value < balance_table[0])
		return 0;
	if(balance_value < balance_table[60])
		return 60;
	return 30;
}

void SPK_Phase_Display(void)
{
	unsigned char channel_counter=0;
	unsigned char all_counter=0;
	unsigned char channel=0;
	char *channel_name[8] = {"L ","C ","R ","SR ","RB ","LB ","SL ","SW "}; /*字符串数组--用指针来表示*/

	VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_LINE_ALL); /*清屏*/    
	VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,"Out PHASE SPK:");

	for(channel=0;channel<8;channel++)
		{
		if(speaker_table[channel]->phase)
			{
			VfdDisplay(VFD_LINE2_START_POSITION+all_counter,VFD_UPDATE_LINE2,channel_name[channel]);		
			channel_counter = strlen(channel_name[channel]);
			all_counter+=channel_counter;
			}
		}
}

extern char dsp_power_flag;
extern unsigned char Movie_Music_Speaker_Flag; /*add cjm 2007-08-03*/
TASK_RESULT ASProcSetSpeakerphase(unsigned char *buffer)
{
	static uchar enable = 0,as_key_enable;
	static uchar any_spk_outphase = 0;
	uchar i = 0;
	switch(buffer[0])
		{
		case MSG_KeyPress:
			switch(buffer[1])
				{
				case _REMOTE_ENTER_KEY:
				case _PANEL_ENTER_KEY:
					switch(as_key_enable)
						{
						case 1:
							break;
						case 2:	/*要做的事情包括save irc to setup menu*/
#ifdef _DEBUG_AUTO_SOUND
							//debug_printf("Saving:\n");
							//for(i = 0;speaker_table[i];i++)
							//	{
							//	debug_printf("%s:\n",speaker_table[i]->channel->name);
							//	debug_printf("valid:%d\n",speaker_table[i]->valid);
							//	debug_printf("distance:%3.1fft,%2.1fM\n",speaker_table[i]->distance,speaker_table[i]->distance/METER2FEET);
							//	debug_printf("balance:%2.1f\n",speaker_table[i]->balance);
							//	debug_printf("eq:%2.1f,%2.1f,%2.1f,%2.1f,%2.1f,%2.1f,%2.1f,%2.1f,%2.1f,%2.1f\n",speaker_table[i]->eq[0],speaker_table[i]->eq[1],speaker_table[i]->eq[2],speaker_table[i]->eq[3],speaker_table[i]->eq[4],speaker_table[i]->eq[5],speaker_table[i]->eq[6],speaker_table[i]->eq[7],speaker_table[i]->eq[8],speaker_table[i]->eq[9]);
							//	debug_printf("crossover:%d\n",speaker_table[i]->crossover_point[0]);
							//	debug_printf("phase:%d\n",speaker_table[i]->phase);
							//	debug_printf("\n");
							//	}
							//debug_printf("\n");
#endif
							Movie_Music_Speaker_Flag=0; /*add cjm 2007-08-03*/
							for(i=0;speaker_table[i];i++)
							{
								SpkConfigSetup(WRITE_VALUE_MODE,speaker_table[i]->valid,speaker_table[i]->channel->spk_index);	/*save spk config*/
								SpkDistanceSetup(WRITE_VALUE_MODE,(uchar)speaker_table[i]->distance,speaker_table[i]->channel->spk_index);	/*save spk distance*/
								SpkBlanceSetup(WRITE_VALUE_MODE,FindBalancePosition(speaker_table[i]->balance),speaker_table[i]->channel->spk_index);	/*save spk balance*/
								SpkCrossoverSetup(WRITE_VALUE_MODE,GetCrossoverPoint(speaker_table[i]->crossover_point[0]),speaker_table[i]->channel->spk_index); /*save spk crossover point*/
								SpkPhaseSetup(WRITE_VALUE_MODE,speaker_table[i]->phase,speaker_table[i]->channel->spk_index); /*save spk phase*/
								if(CH_SW != speaker_table[i]->channel->spk_index)
									{
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[0]),speaker_table[i]->channel->spk_index | EQ_0);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[1]),speaker_table[i]->channel->spk_index | EQ_1);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[2]),speaker_table[i]->channel->spk_index | EQ_2);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[3]),speaker_table[i]->channel->spk_index | EQ_3);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[4]),speaker_table[i]->channel->spk_index | EQ_4);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[5]),speaker_table[i]->channel->spk_index | EQ_5);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[6]),speaker_table[i]->channel->spk_index | EQ_6);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[7]),speaker_table[i]->channel->spk_index | EQ_7);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[8]),speaker_table[i]->channel->spk_index | EQ_8);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[9]),speaker_table[i]->channel->spk_index | EQ_9);
									}
							}
							Movie_Music_Speaker_Flag=1; /*add cjm 2007-08-03*/
							/*add cjm 2007-08-03*/
							for(i=0;speaker_table[i];i++)
							{
								SpkConfigSetup(WRITE_VALUE_MODE,speaker_table[i]->valid,speaker_table[i]->channel->spk_index);	/*save spk config*/
								SpkDistanceSetup(WRITE_VALUE_MODE,(uchar)speaker_table[i]->distance,speaker_table[i]->channel->spk_index);	/*save spk distance*/
								SpkBlanceSetup(WRITE_VALUE_MODE,FindBalancePosition(speaker_table[i]->balance),speaker_table[i]->channel->spk_index);	/*save spk balance*/
								SpkCrossoverSetup(WRITE_VALUE_MODE,GetCrossoverPoint(speaker_table[i]->crossover_point[0]),speaker_table[i]->channel->spk_index);	/*save spk crossover point*/
								SpkPhaseSetup(WRITE_VALUE_MODE,speaker_table[i]->phase,speaker_table[i]->channel->spk_index); /*save spk phase*/
								if(CH_SW != speaker_table[i]->channel->spk_index)
									{
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[0]),speaker_table[i]->channel->spk_index | EQ_0);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[1]),speaker_table[i]->channel->spk_index | EQ_1);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[2]),speaker_table[i]->channel->spk_index | EQ_2);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[3]),speaker_table[i]->channel->spk_index | EQ_3);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[4]),speaker_table[i]->channel->spk_index | EQ_4);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[5]),speaker_table[i]->channel->spk_index | EQ_5);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[6]),speaker_table[i]->channel->spk_index | EQ_6);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[7]),speaker_table[i]->channel->spk_index | EQ_7);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[8]),speaker_table[i]->channel->spk_index | EQ_8);
									SpkEqSetup(WRITE_VALUE_MODE,FindBalancePosition(0-speaker_table[i]->eq[9]),speaker_table[i]->channel->spk_index | EQ_9);								
									}
							}/*add cjm 2007-08-03*/
							Movie_Music_Speaker_Flag=0;  /*add cjm 2007-08-03*/
							
							dsp_power_flag = 0;
							for(i=0;speaker_table[i];i++)    
								{
								SpkConfigSetup(EXECUTE_MODE,speaker_table[i]->valid,speaker_table[i]->channel->spk_index);	/*save spk config*/
								SpkDistanceSetup(EXECUTE_MODE,(uchar)speaker_table[i]->distance,speaker_table[i]->channel->spk_index);	/*save spk distance*/
								SpkBlanceSetup(EXECUTE_MODE,FindBalancePosition(speaker_table[i]->balance),speaker_table[i]->channel->spk_index);	/*save spk balance*/
								SpkCrossoverSetup(EXECUTE_MODE,GetCrossoverPoint(speaker_table[i]->crossover_point[0]),speaker_table[i]->channel->spk_index);/*save spk crossover point*/
								SpkPhaseSetup(EXECUTE_MODE,speaker_table[i]->phase,speaker_table[i]->channel->spk_index); /*save spk phase*/
								if(CH_SW != speaker_table[i]->channel->spk_index)
									{
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[0]),speaker_table[i]->channel->spk_index | EQ_0);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[1]),speaker_table[i]->channel->spk_index | EQ_1);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[2]),speaker_table[i]->channel->spk_index | EQ_2);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[3]),speaker_table[i]->channel->spk_index | EQ_3);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[4]),speaker_table[i]->channel->spk_index | EQ_4);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[5]),speaker_table[i]->channel->spk_index | EQ_5);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[6]),speaker_table[i]->channel->spk_index | EQ_6);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[7]),speaker_table[i]->channel->spk_index | EQ_7);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[8]),speaker_table[i]->channel->spk_index | EQ_8);
									SpkEqSetup(EXECUTE_MODE,FindBalancePosition(0-speaker_table[i]->eq[9]),speaker_table[i]->channel->spk_index | EQ_9);
									}
								}
							dsp_power_flag = 1;
							
							AutoSoundExit();    
							break;
						}
					break;
				case _REMOTE_TEST_KEY:
					AutoSoundExit();
					break;
				}
			break;
		case MSG_Irc:
			switch(buffer[1])
				{
				case IRC_METHOD_INIT:
					enable = 1;
					as_key_enable = 0;
					VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_LINE1,IrcTaskTable[current_step]->name); /*Set Speaker phase*/
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_LINE2);

					any_spk_outphase = 0;
					current_test_spk = 0;
					MsgSetIrc(IRC_TEST_START,current_test_spk);
					break;
				case IRC_TEST_START:
					if(!enable)
						return FALSE;

					bar_counter = strlen(speaker_table[current_test_spk]->channel->name);
					VfdDisplayBlank(VFD_LINE2,VFD_UPDATE_NONE);
					VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,speaker_table[current_test_spk]->channel->name);
					MsgSetIrc(IRC_TEST_DOING,current_test_spk);
					break;
				case IRC_TEST_DOING:
					if(!enable)
						return FALSE;

					if(speaker_table[current_test_spk])
						{
						if(speaker_table[current_test_spk]->valid)
							{
							if(speaker_table[current_test_spk]->phase)
								{
								any_spk_outphase = 1;
								//sprintf(temp_buffer,"180%c",0xBB);	/*0xBB 是度的符号*/
								sprintf(temp_buffer,"Out phase");
								}
							else
								{
								//sprintf(temp_buffer,"0%c",0xBB);	/*0xBB 是度的符号*/
								sprintf(temp_buffer,"In phase");
								}
							i = strlen(temp_buffer);
							while(bar_counter < (VFD_LINE2_LENGTH - i))
								{
								VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_NONE,".");
								bar_counter ++;
								}
							VfdDisplay(VFD_LINE2_START_POSITION+bar_counter,VFD_UPDATE_LINE2,temp_buffer);
							}
						else
							{
							DisplayNA(bar_counter);
							}
						DelayMs(1000);
						}

					current_test_spk ++;
					if(speaker_table[current_test_spk])
						{
						MsgSetIrc(IRC_TEST_START,current_test_spk);
						}
					else
						{
						if(any_spk_outphase)	/*有spk out phase，显示出所有out phase SPK*/
							{
							SPK_Phase_Display();
							DelayMs(5000);
							MsgSetIrc(IRC_TEST_FINISH,0);
							}
						else
							{
							VfdDisplayBlank(VFD_LINE_ALL,VFD_UPDATE_LINE_ALL); /*清屏*/    
							VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,"Out PHASE SPK:");
							VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,"No SPK Out PHASE");
							DelayMs(5000);
							MsgSetIrc(IRC_TEST_FINISH,0);
							}
						}
					break;
				case IRC_TEST_FINISH:
					enable = 0;

					//current_step++;
					if(IrcTaskTable[current_step+1])
						{
						MsgSetIrc(IRC_METHOD_INIT,0);
						}
					else
						{
						as_key_enable = 2;
						VfdDisplay(VFD_LINE1_START_POSITION,VFD_UPDATE_NONE,"Press OK restore ");
						VfdDisplay(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE_ALL,"to Setup Menu       ");
						}
					break;
				}
			break;
		}
	return TRUE;  
}

TASK_RESULT AutoSoundProcMessage(unsigned char *buffer)
{
	if(IsCurFocusTid(AUTO_SOUND_ID) == FALSE)
		return FALSE;
	return IrcTaskTable[current_step]->run(buffer);
}

