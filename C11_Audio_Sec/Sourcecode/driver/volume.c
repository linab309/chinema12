#include "..\main\macro.h"
#include "mas9116.h"
#include "volume.h"
#include "..\main\debug.h"
#include "..\main\task.h"
#include "..\cs495300\dspdrv.h"

#ifdef _DEBUG
#define _DEBUG_VOLUME
#endif

float main_volume = 0;
float balance[8] = {0};

extern void Mas9116Reset(void);
extern void DelayMs(int delay_time);

/* 
 * @brief 设置Zone2 音量 
 *
 * @author chenjianming
 * @date 2007-12-24
 * @refer chenshiguang C11 code
 */
void SetZone2Volume(float volume)
{
#ifdef _DEBUG_VOLUME
	debug_printf("Zone2 vol:%3.1f\n",volume);
#endif

	Mas9116SetGain(MAS9116_CHIP_ZONE2 | MAS9116_BOTH,volume);
}

/* 
 * @brief 设置2 channel 音量 
 *
 * @author chenjianming
 * @date 2007-12-24
 * @refer chenshiguang C11 code
 */
void Set2ChVolume(float volume)
{
#ifdef _DEBUG_VOLUME
	debug_printf("\n2CH vol:%3.1f",volume);
#endif
	Mas9116SetGain(MAS9116_CHIP_2CH | MAS9116_BOTH,volume);    
}

/*关联到dspdrv.h中的Dsp_Channel*/
const unsigned char BalanceChannel[] = 
{
MAS9116_CHIP_7CH1_F | MAS9116_LEFT,			/*CH_FL*/
MAS9116_CHIP_7CH1_CEN_SW | MAS9116_LEFT,	/*CH_CEN*/
MAS9116_CHIP_7CH1_F | MAS9116_RIGHT,		/*CH_FR*/
MAS9116_CHIP_7CH1_SF| MAS9116_LEFT,			/*CH_SL*/
MAS9116_CHIP_7CH1_SF | MAS9116_RIGHT,		/*CH_SR*/
MAS9116_CHIP_7CH1_CEN_SW | MAS9116_RIGHT,	/*CH_SW*/
MAS9116_CHIP_7CH1_SB | MAS9116_LEFT,		/*CH_SBL*/
MAS9116_CHIP_7CH1_SB | MAS9116_RIGHT,		/*CH_SBR*/
};

/* 
 * @brief 设置7.1 channels 音量 
 *
 * @author chenjianming  
 * @date 2007-12-24
 * @refer chenshiguang C11 code
 */
void SetBalanceVolume(unsigned char channel, float volume)
{
	balance[channel] = volume;
	Mas9116SetGain(BalanceChannel[channel], main_volume+volume);
//	debug_printf("channel :%d , volume :%d  \n",channel,volume);
}
void Set7Ch1MainVolume(short volume)
{
	main_volume = volume;

	SetBalanceVolume(CH_LB,balance[CH_LB]);		
	SetBalanceVolume(CH_RB,balance[CH_RB]);		
	SetBalanceVolume(CH_LS,balance[CH_LS]);		
	SetBalanceVolume(CH_RS,balance[CH_RS]);			
	SetBalanceVolume(CH_SW,balance[CH_SW]);
	SetBalanceVolume(CH_C,balance[CH_C]);	
	SetBalanceVolume(CH_L,balance[CH_L]);		
	SetBalanceVolume(CH_R,balance[CH_R]);	

   	
};

/*auto sound测试的时候，必须将平衡全部设置为0，并且将main volume设置为-20 dB,用此函数实现*/
void AutoSoundSetVolume(short volume)
{
	uchar channel;
	for(channel = 0;channel<8;channel++)
		Mas9116SetGain(BalanceChannel[channel], volume);
}

/* 
 * @brief soft mute
 *
 * @author chenjianming
 * @date 2007-12-24
 * @refer chenshiguang C11 code
 */
void SoftMute(uchar on)
{
	static unsigned char Pre_Mute_mode=8;

	if(Pre_Mute_mode!=on)
		{
		Pre_Mute_mode=on;
		if(on)
			{
#ifdef _DEBUG		
			debug_printf("\nSoftMute_______1111111111");
#endif
			//Set7Ch1MainVolume(-100); //软件mute
			Set_FLFR_Mas9116_Mute(0); //硬件mute
			Set_SLSR_Mas9116_Mute(0);
			Set_CENLFE_Mas9116_Mute(0);
			Set_SBLSBR_Mas9116_Mute(0);
			}
		else
			{
#ifdef _DEBUG					
			debug_printf("\nSoftMute_______0000000000");
#endif
				while(Mas9116ReadRC5(2))
				{
					Mas9116Reset();
				}
				while(Mas9116ReadRC5(3))
				{
					Mas9116Reset();
				}
				while(Mas9116ReadRC5(4))
				{
					Mas9116Reset();
				}
				while(Mas9116ReadRC5(5))
				{
					Mas9116Reset();
				}

				/* //软件mute
				if(sys_flag.zone1_volume>-60)
				{
					Set7Ch1MainVolume(sys_flag.zone1_volume-30);
					Set7Ch1MainVolume(sys_flag.zone1_volume-20);
					Set7Ch1MainVolume(sys_flag.zone1_volume-10);
					Set7Ch1MainVolume(sys_flag.zone1_volume);
				} 
				else
				{
					Set7Ch1MainVolume(sys_flag.zone1_volume);
				}
				*/
				Set_FLFR_Mas9116_Mute(1); //硬件mute
				Set_SLSR_Mas9116_Mute(1);
				Set_CENLFE_Mas9116_Mute(1);
				Set_SBLSBR_Mas9116_Mute(1);
			}		
		}
}

/* 
 * @brief 初始化Mas9116寄存器
 *
 * @author chenjianming
 * @date 2008-08-15
 * @refer 
 */
 //
 extern void Mas9116Reset(void);
void VolumeInit(void)
{
	Mas9116Reset();
	DelayMs(300); 

	while(Mas9116ReadRC5(0))
	{
		Mas9116Reset();
	}
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nmake2");
#endif

	while(Mas9116ReadRC5(1))
	{
		Mas9116Reset();
	}
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nmake3");
#endif

	while(Mas9116ReadRC5(2))
	{
		Mas9116Reset();
	}
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nmake4");
#endif

	while(Mas9116ReadRC5(3))
	{
		Mas9116Reset();
	}
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nmake5");
#endif

	while(Mas9116ReadRC5(4))
	{
		Mas9116Reset();
	}
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nmake6");
#endif

	while(Mas9116ReadRC5(5))
	{
		Mas9116Reset();
	}
#ifdef _DEBUG_ZONE1_TASK
	debug_printf("\nmake1");
#endif
}

