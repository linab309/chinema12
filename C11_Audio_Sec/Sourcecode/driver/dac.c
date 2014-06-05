/* @file	DSD driver File
 * @brief DSD Çý¶¯ÎÄ¼þºÍÓ¦ÓÃ³ÌÐò
 *
 * @author chenjianming
 * @date 2008-08-07
 * @refer chenshiguang C11 code
 */
#include "ioport.h"
#include "dac.h"
#include "Eeprom.h"
#include "..\cs495300\Dspdrv.h"  

extern void wait_timer(int timer);

void SelectDSD(unsigned char Sele_DSD)
{
	SetDacCs4(1);
	SetDacCs3(1);
	SetDacCs2(1);
	SetDacCs1(1);
	SetDacCs0(1);
	switch(Sele_DSD)
		{
		case DSD_CS0:
			SetDacCs0(0);
			break;
		case DSD_CS1: //Zone1 FL/FR
			SetDacCs1(0);
			break;
		case DSD_CS2: //Zone1 SL/SR
			SetDacCs2(0);
			break;
		case DSD_CS3: //Zone1 CEN/SW
			SetDacCs3(0);
			break;
		case DSD_CS4: //Zone1 SBR/SBL
			SetDacCs4(0);  
			break;
		case DSD_ALL_None:
			break;	
		case DSD_ALL_Select:
		default:
			SetDacCs0(0);
			SetDacCs1(0);  
			SetDacCs2(0);
			SetDacCs3(0);
			SetDacCs4(0);
			break;		
		}
}
//Ö
void write_word(int iData,unsigned char DSD_Sele)
{
	unsigned char i;

	wait_timer(300);	/* tMHH. Might not need this. */

	SelectDSD(DSD_Sele); /*ÖÃµÍÑ¡ÖÐDSD*/

	for (i = 0; i < 16; i++) /* 16 bit data */
		{
		SetDacMc(0);
		SetDacMd(iData & 0x8000);
		wait_timer(50);	/* tMCL */
		SetDacMc(1);
		wait_timer(50);	/* tMCH */
		iData <<= 1;
		}
	
	SelectDSD(DSD_ALL_None); /*ÖÃ¸ßÊÍ·ÅÈ«²¿DSD*/
}
//
void Pcm1796Write(int iData,unsigned char DSD_CS)
{
	write_word(iData,DSD_CS);
}

void DacInvert(int mode,unsigned char channel)
{
	switch(channel) /*2¸öÍ¨µÀ¹²ÓÃÒ»Æ¬D/A*/
		{
		case CH_L: //0
		case CH_R: // 2
			if(mode)
				Pcm1796Write(0x1380,DSD_CS1); //·´Ïò
			else
				Pcm1796Write(0x1300,DSD_CS1); //ÕýÏò
			break;
		case CH_LS: // 3
		case CH_RS: // 4
			if(mode)
				Pcm1796Write(0x1380,DSD_CS2);
			else
				Pcm1796Write(0x1300,DSD_CS2);		
			break;
		case CH_C: // 1
		case CH_SW: // 5
			if(mode)
				Pcm1796Write(0x1380,DSD_CS3);
			else
				Pcm1796Write(0x1300,DSD_CS3);			
			break;
		case CH_LB: // 6
		case CH_RB: // 7
			if(mode)
				Pcm1796Write(0x1380,DSD_CS4);
			else
				Pcm1796Write(0x1300,DSD_CS4);		
			break;
		case DSD_ALL_Select: //8 All channel 
			if(mode)
				Pcm1796Write(0x1380,DSD_ALL_Select); //·´Ïò
			else
				Pcm1796Write(0x1300,DSD_ALL_Select);	//ÕýÏò	
			break;
		default:
			break;
		}
}
//
void Auto_Setup_DAC_Phase(void)
{
	Pcm1796Write(0x1300,DSD_ALL_Select);	//ÕýÏò	
	SetCH1RELAY(0); // 4Æ¬È«²¿²»·´Ïò
	SetCH2RELAY(0);
	SetCH3RELAY(0);
	SetCH4RELAY(0);	
}

void InitDACPhase(void)
{
	unsigned char i;
	unsigned char bvalue;

	for(i=0;i<8;i++)
		{
		bvalue = EepromReadByte(SPK_PHASE_POSITION+i);
		SetSpkPhase(i,bvalue);
		}
}

void DacMute(int on)
{
	if(on)
		Pcm1796Write(0x1251,DSD_ALL_Select);
	else
		Pcm1796Write(0x1250,DSD_ALL_Select);
}

void InitDACReg(void)
{
	/* register 0x10 */
	Pcm1796Write(0x10ff,DSD_ALL_Select);	/* L channel no digital attentuation */

	/* register 0x11 */
	Pcm1796Write(0x11ff,DSD_ALL_Select);	/* R channel no digital attentuation */

	/* register 0x12 */
	Pcm1796Write(0x1250,DSD_ALL_Select);

	/* register 0x13 */
	//Pcm1796Write(0x1300);
	InitDACPhase();  
}
//
void DSD_Choose(unsigned char mode)
{
	if(1)
		{

		if(mode)
			{
			DSD_Select(1);
			}
		else
			{
			DSD_Select(0);
			}
		}
}

void InitDSDMode(unsigned char mode)
{
/*	static unsigned char Pre_mode=8;*/

	if(1)
		{
/*		Pre_mode=mode;*/
		mode = 2;

		debug_printf("InitDSDMode  : %d   \r\n",mode);
		debug_printf("0 ----PCM mode-DSD Disable   \r\n");
		debug_printf("1 ----2channel DSD Enable    \r\n");
		debug_printf("2 ----Multi channels DSD Enable    \r\n");
		switch(mode)
			{
			case 0: /*PCM mode-DSD Disable*/
				DSP_Select(1);  /*1Îª¹Ø;0-Îª¿ª*/
				Pcm1796Write(0x1400,DSD_ALL_Select); /*DSD mode Disable*/
				break;
			case 1: /*2channel DSD Enable*/
				DSD_Choose(1);  /*1Îª¹Ø;0-Îª¿ª*/
				Pcm1796Write(0x1420,DSD_CS1);
				Pcm1796Write(0x1400,DSD_CS2);
				Pcm1796Write(0x1400,DSD_CS3);
				Pcm1796Write(0x1400,DSD_CS4);
				break;
			case 2: /*Multi channels DSD Enable*/
				DSD_Choose(1);  /*1Îª¹Ø;0-Îª¿ª*/

				Pcm1796Write(0x1420,DSD_CS1);
				Pcm1796Write(0x1420,DSD_CS2);
				Pcm1796Write(0x1420,DSD_CS3);
				Pcm1796Write(0x1400,DSD_CS4);		 		
				break;
			default:
				break;		
			}
		}
}

