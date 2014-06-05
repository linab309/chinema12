#include "macro.h"
#include <string.h>

#include "rs232_protocol.h"
#include "queue.h"
#include "task.h"
#include "debug.h"
#include "setup_func.h"  
#include "setup_menu.h" 
#include "message.h"
#include "radio_task.h"   
#include "c11_msg_handler.h" 
#include "keydefine.h"
#include "..\cs495300\dspdrv.h"
#include "..\cs495300\Eq_Manager.h"
#include "..\cs495300\DelayManager.h"
#include "..\main\Vfd_disp.h"
#include "..\main\Radio_task.h"
#include "..\driver\uart.h"
#include "..\driver\volume.h" 
#include "..\driver\tuner.h" 
#include "..\driver\channel.h" 
#include "..\driver\eeprom.h"
#include "..\driver\max335.h"
#include "..\driver\Hdmi.h"
#include "..\driver\Ioport.h" 
#include "..\driver\Dac.h" 
#include "..\driver\Cs8416.h"

#ifdef _DEBUG
#define _DEBUG_RS232
#endif

extern unsigned char key_enable;
extern uchar SystemTriggerMode;
extern unsigned char zone2_ir_mode;
extern unsigned char rear_ir_mode ;
extern unsigned char HDMI_Audio_Out;
extern uchar pwd_mode;
extern unsigned char Movie_Music_Speaker_Flag; 
extern unsigned char FindBalancePosition(float balance_value); 

#define IsProtocolEqValid(eq) IsBalanceValid(eq)
BOOL IsBalanceValid(float volume);

extern pSTRUCT_QUEUE protocol_queue_pointer;

#define PROTOCOL_BUF_SIZE 20

unsigned char recv_buffer[PROTOCOL_BUF_SIZE];

BOOL GetProtocolFloatValue(unsigned char *buffer, float * rtn_value)
{
	float temp_value = 0;
	uchar sign_bit = 0;
	uchar position_bit = 0;
	if(buffer[0] == '-')
		{
		sign_bit = 1;
		buffer ++;
		}
	else if(buffer[0] == '+')
		{
		buffer ++;
		}
	else if((buffer[0] >= '0') && (buffer[0] <= '9'))
		{}
	else
		return FALSE;

	while(*buffer)
		{
		if((*buffer >= '0') && (*buffer <= '9'))
			{
			if((position_bit == 1) || (position_bit == 0))
				{
				temp_value *= 10;
				temp_value += *buffer - '0';
				if(position_bit == 1)
					{
					temp_value /= 10;
					position_bit = 2;
					}
				}
			else
				return FALSE;
			}
		else if(*buffer == '.')
			{
			if(position_bit == 0)
				{
				position_bit = 1;
				}
			else
				return FALSE;
			}
		else
			return FALSE;
		buffer ++;
		}
	if(sign_bit)
		temp_value = -temp_value;
#ifdef _DEBUG_RS232
	rs232_printf("float:%f\n",temp_value);
#endif
	*rtn_value = temp_value;
	return TRUE;
}

BOOL IsProtocolStringValid(unsigned char *buffer, uchar len)
{
	unsigned char cnter = 0;
	while(*buffer)
		{
		if((*buffer >= 0x20) && (*buffer <= 0x7F))
			{
			cnter ++;
			buffer ++;
			}
		else
			return FALSE;
		}
	if(cnter > len)
		return FALSE;
	return TRUE;
}

unsigned char cur_dis_unit = 1;
BOOL ProcCmdSPU(unsigned char *buffer,unsigned char size)
{
//#ifdef _DEBUG_RS232
	//rs232_printf("ProcCmdSPU,%d\n",size);
//#endif
/*
	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':
				cur_dis_unit = 0;
				break;
			case '1':
				cur_dis_unit = 1;
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}
*/		
	return FALSE;
}

unsigned char GetCurrentDecodeStatus(void) 
{
	unsigned char type = 0;

	if(sys_flag.zone1_channel == INPUT_CH71)
		{
		type = 4;
		}
	else if(sys_flag.src_mode == 2)
		{
		type = 0;
		}
	else
		{
		switch(sys_flag.stream_type)
			{
			case STREAM_NONE:
				type = 6;
				break;
			case STREAM_PCM:
				type = 3;
				break;
			case STREAM_AC3:
				type = 1;
				break;
			case STREAM_DTS:
				type = 2;
				break;
			  }
		  }
	return type;
} 

 /*return PCM Decode mode*/
unsigned char GetPcmDecodeMode() 
{
	unsigned char rtn_value = 0;
	
	switch(sys_flag.decode_mode.pcm_decode_mode)
		{
		case PCM_MODE_NORMAL:
			rtn_value = 0;
			break;
		case PCM_MODE_PL2:
			rtn_value = 1;
			if(sys_flag.decode_mode.pcm_sur_mode)
				rtn_value ++;
			break;
		case PCM_MODE_PL2X:
			rtn_value = 3;
			if(sys_flag.decode_mode.pcm_sur_mode)
				rtn_value ++;
			break;
		case PCM_MODE_NEO6:
			rtn_value = 5;
			if(sys_flag.decode_mode.pcm_sur_mode)
				rtn_value ++;
			break;
		case PCM_MODE_CES:
			switch(sys_flag.decode_mode.pcm_sur_mode)
				{
				case 0: //CES7.1
					rtn_value = 7;
					break;
				case 1: //CES 7.1 ALL Stereo
					rtn_value = 8;
					break;
				case 2: //CES 7.1 All Mono
					rtn_value = 9;
					break;
				}
			break;
		}
	
	return rtn_value;	
}

/*Process zone1 command(Volume\mute\decode status\stereo effect)*/
BOOL ProcCmdZ1_(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1?,%d\n",size);
#endif
	
	if(size == 3)
		{
		switch(buffer[2])
			{
			case '?':			
				rs232_printf("@Z1SV%dM%dD%dE%d\n",sys_flag.zone1_volume ,sys_flag.zone1_mute_flag,GetCurrentDecodeStatus(),GetPcmDecodeMode());
			break;
			}
			
		return TRUE;
		}
	return FALSE;
}

unsigned char GetZone1Channel()
{
	if(GetCurFocusTid() == RADIO_ID)
		{
		switch(current_radio_mode)
			{
			case _AM_MODE :
				return 9;			
			case _FM_MODE:
				return 10;
			case _HD_AM_MODE:
				return 11;
			case _HD_FM_MODE:
				return 12;
			}
		}
	else
		{
		switch(sys_flag.zone1_channel)
			{
			case INPUT_CH71:
				return 0;			
			case INPUT_1:
			case INPUT_2:
			case INPUT_3:
			case INPUT_4:
			case INPUT_5:
			case INPUT_6:
			case INPUT_7:
			case INPUT_8:
				return sys_flag.zone1_channel - INPUT_1 + 1;			
			case INPUT_HDMI1:
				return 13;
			case INPUT_HDMI2:
				return 14;
			case INPUT_TUNER:
				switch(current_radio_mode)
					{
					case _AM_MODE :
						return 9;			
					case _FM_MODE:
						return 10;
					case _HD_AM_MODE:
						return 11;
					case _HD_FM_MODE:
						return 12;					
					}
				break;
			}
		}
	return 0;	
}

/*Process zone1 channel select command*/
BOOL ProcCmdZ1S(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1S,%d\n",size); 
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':
				MsgSetInputSource(ZONE1_ID,INPUT_CH71);
				break;
			case '1':	//input 1
			case '2':	//input 2
			case '3':	//input 3
			case '4':	//input 4
			case '5':	//input 5
			case '6':	//input 6
			case '7':	//input 7
			case '8':	//input 8
				if(GetCurFocusTid()==RADIO_ID)
					RadioExit();
				MsgSetInputSource(ZONE1_ID,buffer[3] - '1');
				break;		
			case '9': //AM
				RadioInit(_AM_MODE);
				break;
			case '?':
				rs232_printf("@Z1S%d\n",GetZone1Channel());
				break;
			//case 'S':
			//	if(GetCurFocusTid() == ZONE1_ID)
			//		MsgSetKey(ZONE1_ID,MSG_KeyPress,_REMOTE_SEEK_KEY);
			//	break;
			default:
				return FALSE;
			}
		return TRUE;
		}
	if(size == 5)
		{
		if((buffer[3] == '1') && (buffer[4] == '0')) //FM
			{
			RadioInit(_FM_MODE);
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '1')) //HDAM
			{
			RadioInit(_HD_AM_MODE);
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '2')) //HDFM
			{
			RadioInit(_HD_FM_MODE);
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '3')) //HDMI 1
			{
			if(GetCurFocusTid()==RADIO_ID)
				RadioExit();			
			MsgSetInputSource(ZONE1_ID,10);
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '4')) //HDMI 2
			{
			if(GetCurFocusTid()==RADIO_ID)
				RadioExit();			
			MsgSetInputSource(ZONE1_ID,11); 
			return TRUE;
			}		
		if((buffer[3] == '1') && (buffer[4] == '5')) //HDMI 3
			{
			if(GetCurFocusTid()==RADIO_ID)
				RadioExit();			
			MsgSetInputSource(ZONE1_ID,12); 
			return TRUE;
			}		
		if((buffer[3] == '1') && (buffer[4] == '6')) //HDMI 4
			{
			if(GetCurFocusTid()==RADIO_ID)
				RadioExit();			
			MsgSetInputSource(ZONE1_ID,13); 
			return TRUE;
			}		
		if((buffer[3] == '1') && (buffer[4] == '7')) //TV SOUND
			{
			if(GetCurFocusTid()==RADIO_ID)
				RadioExit();			
			MsgSetInputSource(ZONE1_ID,14); 
			return TRUE;
			}		
		}		
	return FALSE;
}

/*Process zone1 power command*/
BOOL ProcCmdZ1P(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1P,%d\n",size); 
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':	//"Z1P0"
			case '1':	//"Z1P1"
				MsgSetPower(ZONE1_ID,buffer[3] - '0');
				break;
			case '?':	//"Z1P?"
				if(IsZone1PowerOn() == TRUE)
					{
					rs232_printf("@Z1P1\n");
					}
				else
					{
					rs232_printf("@Z1P0\n");
					}
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}
	return FALSE;
}

/*Process zone1 mute command*/
BOOL ProcCmdZ1M(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1M,%d\n",size); 
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':	//Z1M0
			case '1':	//Z1M1
				MsgSetMute(ZONE1_ID,MODE_FIXED,buffer[3] - '0');
				return TRUE;  
			}
		}
	return FALSE;
}

/*process zone1 AC3 status command*/
BOOL ProcCmdZ1A_(unsigned char *buffer,unsigned char size) 
{
	unsigned char type = 0; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1A_,%d\n",size);
#endif

	if(size == 4)
		{
		switch(sys_flag.stream_type)
			{
			case STREAM_AC3:
				if(sys_flag.decode_mode.dolby_type == _DOLBY_2CH)
					type = 1; //AC3 2 CHANNEL
				else
					type = 2; //multichannel AC3
				break;
			default:
				break; //not AC3 stream
			}
		rs232_printf("@Z1A%d\n",type);
		return TRUE;
		}	
	return FALSE;
}

/*Process zone1 DOLBY Digital dynamic range compression */
extern unsigned char DolbyType;
BOOL ProcCmdZ1C(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1C,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':
			case '1':
			case '2':
				compression_mode = buffer[3] - '0';
				SetLate(DolbyType,compression_mode);
				break;
			case '?':
				rs232_printf("@Z1C%d\n",compression_mode);
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}	
	return FALSE;
}

/*Process zone1 decode status Command*/
BOOL ProcCmdZ1D(unsigned char *buffer,unsigned char size) 
{
	unsigned char type = 0; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1D,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '?':
				if(sys_flag.zone1_channel == INPUT_CH71)
					{
					type = 4;
					}
				else if(sys_flag.src_mode == 2)
					{
					type = 0;
					}
				else
					{
					switch(sys_flag.stream_type)
						{
						case STREAM_NONE:
							type = 6;
							break;
						case STREAM_PCM:
							type = 3;
							break;
						case STREAM_AC3:
							type = 1;
							break;
						case STREAM_DTS:
							type = 2;
							break;
						}
					}
				rs232_printf("@Z1D%d\n",type);
				break;
			}
		}
	return TRUE;	
}

/*DTS-5.1 input effect status*/
BOOL ProcCmdZ1ED(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ED,%d\n",size);
#endif

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':
			case '1':
				if(sys_flag.stream_type == STREAM_DTS)
					{
					if((sys_flag.decode_mode.dts_type == _DTS_96_24)||(sys_flag.decode_mode.dts_type == _DTS))
						{
						if(sys_flag.decode_mode.dts_decode_mode != DTS_MODE_CES_ON)
							{
							sys_flag.decode_mode.dts_decode_mode = DTS_MODE_CES_ON;
							}
						else
							{
							sys_flag.decode_mode.dts_decode_mode = DTS_MODE_NORMAL;
							}
						DspDecodeDTS(sys_flag.decode_mode.dts_decode_mode);
						}
					}
				return TRUE;
			case '?':
				if(sys_flag.decode_mode.dts_decode_mode == DTS_MODE_CES_ON)
					rs232_printf("@Z1ED1\n");
				else
					rs232_printf("@Z1ED0\n");
				return TRUE;
			}
		}
	return FALSE;	
}

/*return DOLBY 2 channel Decode mode*/
unsigned char GetDolby2ChDecodeMode() 
{
	unsigned char rtn_value = 0;
	switch(sys_flag.decode_mode.dolby_decode_mode)
		{
		case AC3_MODE_NORMAL:
			break;
		case AC3_MODE_PL2:	
			rtn_value = 1;
			if(sys_flag.decode_mode.dolby_sur_mode)
				rtn_value ++;
			break;
		case AC3_MODE_PL2X:
			rtn_value = 3;
			if(sys_flag.decode_mode.dolby_sur_mode)
				rtn_value ++;
			break;
		case AC3_MODE_CES_ON:
			switch(sys_flag.decode_mode.dolby_sur_mode)
				{
				case 0: //CES7.1 ON
					rtn_value =5;
					break;
				case 1: //CES7.1 ON All stereo
					rtn_value =6;
					break;
				case 2: //CES7.1 ON All mono
					rtn_value =7;
					break;
				}
			break;
		}
	return rtn_value;
}

/*Dolby Digital 2.0 surround encoded input surround effect*/
BOOL ProcCmdZ1EF(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EF,%d\n",size);
#endif

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':	//Dolby 2ch pl2 normal
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL;
				break;
			case '1':	//Dolby 2ch pl2 music
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_PL2;
				sys_flag.decode_mode.dolby_sur_mode = 0;
				break;
			case '2':	//Dolby 2ch pl2 movie
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_PL2;
				sys_flag.decode_mode.dolby_sur_mode = 1;
				break;
			case '3':	//Dolby 2ch pl2x music
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_PL2X;
				sys_flag.decode_mode.dolby_sur_mode = 0;
				break;
			case '4':	//Dolby 2ch pl2x movie
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_PL2X;
				sys_flag.decode_mode.dolby_sur_mode = 1;
				break;
			case '5': //Dolby 2ch CES7.1
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_CES_ON;
				sys_flag.decode_mode.dolby_sur_mode = 0;
				break;
			case '6':	//Dolby 2ch ces all streo
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_CES_ON;
				sys_flag.decode_mode.dolby_sur_mode = 1;
				break;
			case '7': //Dolby 2ch ces all mono
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_CES_ON;
				sys_flag.decode_mode.dolby_sur_mode = 2;
				break;
			case '?':
				rs232_printf("@Z1EF%d\n",GetDolby2ChDecodeMode());
				return TRUE;
			default:
				return FALSE;
			}

		if((sys_flag.stream_type == STREAM_AC3) && (sys_flag.decode_mode.dolby_type == _DOLBY_2CH))
			{
			DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
			DelayMs(300);
			if(sys_flag.decode_mode.dolby_decode_mode == AC3_MODE_CES_ON)
				DspSetCrossbarEffect(sys_flag.decode_mode.dolby_sur_mode); 
			MsgSetVfdShow(ZONE1_ID,0);
			}
		return TRUE;
		}	
	return FALSE;
}

/*Dolby Digital EX encoded input effect status*/
BOOL ProcCmdZ1EE(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EE,%d\n",size);
#endif

	if(size==5)
		{
		switch(buffer[4])
			{
			case '?':
				if(sys_flag.decode_mode.dolby_decode_mode== AC3_MODE_CES_ON)
					rs232_printf("@Z1EE1\n");
				else
					rs232_printf("@Z1EE0\n");
				return TRUE;
			}
		}	
	return FALSE;	
}

/* DTS ES encoded input effect status*/
BOOL ProcCmdZ1ES(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ES,%d\n",size);
#endif

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '?':
				if(sys_flag.decode_mode.dts_decode_mode == DTS_MODE_CES_ON)
					rs232_printf("@Z1ES1\n");
				else
					rs232_printf("@Z1ES0\n");
				return TRUE;
			}
		}	
	return FALSE;	
}

/*return Dolby 5 channel Decode mode*/
unsigned char GetDolby5ChDecodeMode() 
{
	unsigned char rtn_value = 0;
	switch(sys_flag.decode_mode.dolby_decode_mode)
		{
		case AC3_MODE_NORMAL:
			break;
		case AC3_MODE_PL2X:
			rtn_value = 1;
			if(sys_flag.decode_mode.dolby_sur_mode)
				rtn_value ++;
			break;
		case AC3_MODE_CES_ON:
			rtn_value = 3;
			break;
		}
	return rtn_value;
}

/*Dolby Digital 5.1 input effect status*/
BOOL ProcCmdZ1EX(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EX,%d\n",size);
#endif

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL;
				break;
			case '1':	//Pl2x Music
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_PL2X;
				sys_flag.decode_mode.dolby_sur_mode=0; 
				break;
			case '2':	//Pl2x Movie
				sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_PL2X;
				sys_flag.decode_mode.dolby_sur_mode=1; 
				break;
			case '?':
				rs232_printf("@Z1EX%d\n",GetDolby5ChDecodeMode());
				return TRUE;
			default:
				return FALSE;
			}

		if((sys_flag.stream_type == STREAM_AC3) && (sys_flag.decode_mode.dolby_type != _DOLBY_2CH))
			{
			DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
			MsgSetVfdShow(ZONE1_ID,0);
			}

		return TRUE;
		}	
	return FALSE;	
}

/*PCM sstereo input surround effect*/
BOOL ProcCmdZ1E(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1E,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0': //off-normal
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NORMAL;
				break;
			case '1':	//Pl2 Music
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_PL2;
				sys_flag.decode_mode.pcm_sur_mode = 0;
				break;
			case '2':	//Pl2 Movie
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_PL2;
				sys_flag.decode_mode.pcm_sur_mode = 1;
				break;
			case '3':	//Pl2x Music
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_PL2X;
				sys_flag.decode_mode.pcm_sur_mode = 0;
				break;
			case '4':	//Pl2x Movie
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_PL2X;
				sys_flag.decode_mode.pcm_sur_mode = 1;
				break;
			case '5':	//Neo6 Music
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NEO6;
				sys_flag.decode_mode.pcm_sur_mode = 0;
				break;
			case '6':	//Neo6 Cinema
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_NEO6;
				sys_flag.decode_mode.pcm_sur_mode = 1;
				break;
			case '7':	//CES7.1
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_CES;
				sys_flag.decode_mode.pcm_sur_mode = 0;
				break;
			case '8':	 //all stereo
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_CES;
				sys_flag.decode_mode.pcm_sur_mode = 1;
				break;
			case '9': //all mono
				sys_flag.decode_mode.pcm_decode_mode = PCM_MODE_CES;
				sys_flag.decode_mode.pcm_sur_mode = 2;
				break;
			case '?':
				rs232_printf("@Z1E%d\n",GetPcmDecodeMode());
				return TRUE;
			default:
				return FALSE;
			}
		if(sys_flag.stream_type == STREAM_PCM)
			{
			DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
			DelayMs(300);
			if(sys_flag.decode_mode.pcm_decode_mode == PCM_MODE_CES)
				DspSetCrossbarEffect(sys_flag.decode_mode.pcm_sur_mode);
			MsgSetVfdShow(ZONE1_ID,0);
			}
		return TRUE;
		}	
	return FALSE;
}

/* LPCM 2.0 surround encoded*/
BOOL ProcCmdZ1EU(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EU,%d\n",size);
#endif

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '?':
				rs232_printf("@Z1E%d\n",GetPcmDecodeMode());
				return TRUE;
			}
		}
	return FALSE;
}

/*Process volume command*/
BOOL ProcCmdZ1VM(unsigned char *buffer,unsigned char size) 
{
	float volume = 0;  
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VM,%d\n",size); 
#endif

	if(size == 5)
		{
		switch(buffer[4])
			{
			case 'U':
				MsgSetVolume(ZONE1_ID,MODE_PLUS,0);
				break;
			case 'D':
				MsgSetVolume(ZONE1_ID,MODE_MINUS,0);
				break;
			case '?':
				rs232_printf("@Z1VMs%d\n",sys_flag.zone1_volume);
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}
	if(buffer[4] == 's') /*如果输入的是数字，则进行音量设置*/
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsZone1VolumeValid((short)volume) == TRUE)
				{
				MsgSetVolume(ZONE1_ID,MODE_FIXED,(short) volume);
#ifdef _DEBUG_RS232
				rs232_printf("volume:%d\n",(short)volume);
#endif
				return TRUE;
				}
#ifdef _DEBUG_RS232
			rs232_printf("volume:%d,error\n",(short)volume);
#endif					
			}
		}	
	return FALSE;
}

extern unsigned char stream_type;
extern unsigned char PcmType;
extern unsigned char DolbyType;
BOOL ProcCmdZ1DF_(unsigned char *buffer,unsigned char size)
{
	unsigned char type=0;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1DF_,%d\n",size); 
#endif

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '?':
				if(sys_flag.zone1_channel == INPUT_CH71)
					{
					type=16;
					}
				else
					{	
					switch(stream_type)
						{
						case STREAM_NONE:
							type=0;
							break;
						case STREAM_AC3:
							switch(DolbyType)
								{
								case _DOLBY_2CH:
									type=3;
									break;
								case _DOLBY_5CH:
									type=4;
									break;
								case _DOLBY_EX:
									type=5;
									break;
								case _DOLBY_PLUS:
									type=6;
									break;
								case _DOLBY_TRUE_HD_2ch:
								case _DOLBY_TRUE_HD_5ch:
								case _DOLBY_TRUE_HD_6ch:
								case _DOLBY_TRUE_HD_7ch:
									type=7;
									break;
								}
							break;
						case STREAM_DTS:
							switch(sys_flag.decode_mode.dts_type)
								{
								case _DTS:
								case _DTS_96_24:
									type=8;
									break;
								case _DTS_ES_MATRIX:
									type=9;
									break;
								case _DTS_ES_DISCRETE:
									type=10;
									break;
								case _DTS_HD_MA_48K:
								case _DTS_HD_MA_96k_192K:
									type=11;
									break;
								case _DTS_HD_HRA_48K:
								case _DTS_HD_HRA_96K:
									type=12;
									break;
								}
							break;
						case STREAM_PCM:
							switch(PcmType)
								{
								case _PCM_2_1:
									type=13;
									break;
								case _PCM_5_1:
									type=14;
									break;
								case _PCM_7_1:
									type=15;
									break;
								}					
							break;
						}
					}
				rs232_printf("@Z1DF %d\n",type);
				break; 
			}
		}
	return TRUE;  
}
//

void ProtocolSetEq(unsigned char channel,unsigned char band,float eq_value) 
{
	dsp_eq_table[channel][band] = eq_value;
	EqModuleconfig(channel,band);
	if(Movie_Music_Speaker_Flag==0)
		EepromWriteByte(EQ_FL_POSITION + (channel * EQ_POSITION_LENGTH) + band, FindBalancePosition(eq_value));
	else
		EepromWriteByte(EQ_FL_POSITION_MUSIC + (channel * EQ_POSITION_LENGTH) + band, FindBalancePosition(eq_value));
}

BOOL IsProtocolEqBandValid(unsigned char *band,unsigned char string) 
{ 
	switch(string)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			*band = string - '0';
			return TRUE;
		}
	return FALSE;	
}

BOOL ProcCmdZ1EQL(unsigned char *buffer,unsigned char size) 
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQL,%d\n",size);
#endif

	if(size > 8)
		{
		if(buffer[5] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[6]))
				{
				if(GetProtocolFloatValue(buffer+7,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=0; //Movie mode
						ProtocolSetEq(CH_L,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}		
 	return FALSE;
}
//
BOOL ProcCmdZ1EQC(unsigned char *buffer,unsigned char size) 
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQC,%d\n",size);
#endif

	if(size > 8)
		{
		if(buffer[5] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[6]))
				{
				if(GetProtocolFloatValue(buffer+7,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=0; //Movie mode
						ProtocolSetEq(CH_C,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}		
 	return FALSE;	
}

BOOL ProcCmdZ1EQR(unsigned char *buffer,unsigned char size) 
{
	unsigned char band = 0;
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQR,%d\n",size);
#endif

	if(size > 8)
		{
		if(buffer[5] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[6]))
				{
				if(GetProtocolFloatValue(buffer+7,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=0; //Movie mode
						ProtocolSetEq(CH_R,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}	
 	return FALSE;
}

BOOL ProcCmdZ1EQSR(unsigned char *buffer,unsigned char size) 
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQSR,%d\n",size);
#endif

	if(size > 9)
		{
		if(buffer[6] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[7]))
				{
				if(GetProtocolFloatValue(buffer+8,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=0; //Movie mode
						ProtocolSetEq(CH_RS,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}	
 	return FALSE;
}

BOOL ProcCmdZ1EQBSR(unsigned char *buffer,unsigned char size) 
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQBSR,%d\n",size);
#endif

	if(size > 10)
		{
		if(buffer[7] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[8]))
				{
				if(GetProtocolFloatValue(buffer+9,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=0; //Movie mode
						ProtocolSetEq(CH_RB,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}
 	return FALSE;	
}

BOOL ProcCmdZ1EQBSL(unsigned char *buffer,unsigned char size) 
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQBSL,%d\n",size);
#endif

	if(size > 10)
		{
		if(buffer[7] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[8]))
				{
				if(GetProtocolFloatValue(buffer+9,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=0; //Movie mode
						ProtocolSetEq(CH_LB,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}		
 	return FALSE;	
}

BOOL ProcCmdZ1EQSL(unsigned char *buffer,unsigned char size) 
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQSL,%d\n",size);
#endif

	if(size > 9)
		{
		if(buffer[6] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[7]))
				{
				if(GetProtocolFloatValue(buffer+8,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=0; //Movie mode						
						ProtocolSetEq(CH_LS,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}	
 	return FALSE;	
}

BOOL ProcCmdZ1EQSW(unsigned char *buffer,unsigned char size) 
{
	unsigned char band = 0;
	float eq_value = 0;
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQSW,%d\n",size);
#endif

	if(size > 9)
		{
		if(buffer[6] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[7]))
				{
				if(GetProtocolFloatValue(buffer+8,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=0; //Movie mode								
						ProtocolSetEq(CH_SW,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}	
 	return FALSE;
}

BOOL ProcCmdZ1EQ(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQ,%d\n",size);
#endif

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':
				enable_eq = 0;
				VfdTempDispBlank(VFD_LINE2,VFD_UPDATE_NONE);
				if(enable_eq)
					VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,30,"EQ ON");				
				else
					VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,30,"EQ OFF");		
				EqOn(enable_eq);
				return TRUE;
			case '1':
				enable_eq = 1;
				VfdTempDispBlank(VFD_LINE2,VFD_UPDATE_NONE);
				if(enable_eq)
					VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,30,"EQ ON");
				else
					VfdTempDisp(VFD_LINE2_START_POSITION,VFD_UPDATE_LINE2,30,"EQ OFF");
				EqOn(enable_eq); 
				return TRUE;
			}
		}		
	return FALSE;	
}

/*Process zone2 power command*/
BOOL ProcCmdZ2P(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ2P,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':	//"Z2P0"
			case '1':	//"Z2P1"
				MsgSetPower(ZONE2_ID,buffer[3] - '0');
				break;
			case '?':	//"Z2P?"
				if(IsZone2PowerOn() == TRUE)
					{
					rs232_printf("@Z2P1\n");
					}
				else
					{
					rs232_printf("@Z2P0\n");
					}
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}	
	return FALSE;
}

unsigned char GetZone2Channel() 
{ 
	switch(sys_flag.zone2_channel)
		{
		case INPUT_CH71:
			return 0;		
		case INPUT_1:
		case INPUT_2:
		case INPUT_3:
		case INPUT_4:
		case INPUT_5:
		case INPUT_6:
		case INPUT_7:
		case INPUT_8:
			return sys_flag.zone2_channel - INPUT_1 + 1;
		case INPUT_TUNER:
			switch(current_radio_mode)
				{
				case _AM_MODE:
					return 9;
				case _FM_MODE:
					return 10;
				case _HD_AM_MODE:
					return 11;
				case _HD_FM_MODE:
					return 12;	
				}	
		case INPUT_HDMI1:
			return 13;
		case INPUT_HDMI2:
			return 14;
		}
	return 0;	
}

extern char * Zone2SrcName;
extern const unsigned int zone2_analog_in_table[];
BOOL ProcCmdZ2S(unsigned char *buffer,unsigned char size) 
{
	unsigned char value;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ2S,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0'://input 7.1
				MsgSetInputSource(ZONE2_ID, INPUT_CH71);
				MsgSetVfdShow(ZONE2_ID,0);
				break;
			case '1':	//input 1
			case '2':	//input 2
			case '3':	//input 3
			case '4':	//input 4
			case '5':	//input 5
			case '6':	//input 6
			case '7':	//input 7
			case '8':	//input 8
				MsgSetInputSource(ZONE2_ID,buffer[3] - '1');
				MsgSetVfdShow(ZONE2_ID,0);
				break;
			case '9': //Tuner-AM
				sys_flag.zone2_channel=INPUT_TUNER;
				if(!IsCurFocusTid(RADIO_ID))
					{
					Zone2RadioMode(_AM_MODE);
					}
				Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
				SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
				UpdateMax335();	/*Update MAX335*/
				DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/
				MsgSetVfdShow(ZONE2_ID,0);	
				break;
			case '?':
				rs232_printf("@Z2S%d\n",GetZone2Channel());
				return TRUE;
			default:
				return FALSE;
			}
		return TRUE;
		}
	if(size == 5)
		{
		if((buffer[3] == '1') && (buffer[4] == '0')) //Tuner-FM
			{
			sys_flag.zone2_channel=INPUT_TUNER;
			if(!IsCurFocusTid(RADIO_ID))
				{
				Zone2RadioMode(_FM_MODE);    
				}
			Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
			SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
			UpdateMax335();	/*Update MAX335*/
			DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/
			MsgSetVfdShow(ZONE2_ID,0);
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '1')) //Tuner-HDAM
			{
			sys_flag.zone2_channel=INPUT_TUNER;
			if(!IsCurFocusTid(RADIO_ID))
				{
				Zone2RadioMode(_HD_AM_MODE);  
				}
			Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
			SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
			UpdateMax335();	/*Update MAX335*/
			DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/				
			MsgSetVfdShow(ZONE2_ID,0);
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '2')) //Tuner-HMFM
			{
			sys_flag.zone2_channel=INPUT_TUNER;
			if(!IsCurFocusTid(RADIO_ID)) 
				{
				Zone2RadioMode(_HD_FM_MODE);  
				}
			Zone2AnalogInOff(); /*关闭所有Zone 2 Analog Input*/
			SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog 开关*/
			UpdateMax335();	/*Update MAX335*/
			DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/				
			MsgSetVfdShow(ZONE2_ID,0);
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '3')) //HDMI 1
			{
			if(sys_flag.zone1_channel!=INPUT_HDMI2)
				{
				value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
				if(value==6) /*当为6表示Zone1的HDMI 流已经稳定*/
					{
					sys_flag.zone2_channel=INPUT_HDMI1;
					DigiSendByte(ADDR_ZONE2,0X00);/*通道切到HDMI口*/
					Zone2SrcName = GetSrcName(INPUT_HDMI1);
					MsgSetVfdShow(ZONE2_ID,0);
					}
				}
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '4')) //HDMI 2
			{
			if(sys_flag.zone1_channel!=INPUT_HDMI1)
				{
				value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
				if(value==6) /*当为6表示Zone1的HDMI 流已经稳定*/
					{
					sys_flag.zone2_channel=INPUT_HDMI2; 
					DigiSendByte(ADDR_ZONE2,0X00);/*通道切到HDMI口*/
					Zone2SrcName = GetSrcName(INPUT_HDMI2);
					MsgSetVfdShow(ZONE2_ID,0);	  				
					}
				}
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '5')) //HDMI 3
			{
			if(sys_flag.zone1_channel!=INPUT_HDMI3)
				{
				value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
				if(value==6) /*当为6表示Zone1的HDMI 流已经稳定*/
					{
					sys_flag.zone2_channel=INPUT_HDMI3; 
					DigiSendByte(ADDR_ZONE2,0X00);/*通道切到HDMI口*/
					Zone2SrcName = GetSrcName(INPUT_HDMI3);
					MsgSetVfdShow(ZONE2_ID,0);	  				
					}
				}
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '6')) //HDMI 4
			{
			if(sys_flag.zone1_channel!=INPUT_HDMI4)
				{
				value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
				if(value==6) /*当为6表示Zone1的HDMI 流已经稳定*/
					{
					sys_flag.zone2_channel=INPUT_HDMI4; 
					DigiSendByte(ADDR_ZONE2,0X00);/*通道切到HDMI口*/
					Zone2SrcName = GetSrcName(INPUT_HDMI4);
					MsgSetVfdShow(ZONE2_ID,0);	  				
					}
				}
			return TRUE;
			}
		if((buffer[3] == '1') && (buffer[4] == '7')) //TV SOUND
			{
			if(sys_flag.zone1_channel!=ARC_CHANNEL)
				{
				value=HdmiReadByte(4); /*读HDMI板状态寄存器*/
				if(value==6) /*当为6表示Zone1的HDMI 流已经稳定*/
					{
					sys_flag.zone2_channel=ARC_CHANNEL; 
					DigiSendByte(ADDR_ZONE2,0X00);/*通道切到HDMI口*/
					Zone2SrcName = GetSrcName(ARC_CHANNEL);
					MsgSetVfdShow(ZONE2_ID,0);	  				
					}
				}
			return TRUE;
			}
		}	
	return FALSE;
}

BOOL ProcCmdZ2V(unsigned char *buffer,unsigned char size) 
{
	float volume; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ2V,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case 'U':
				MsgSetVolume(ZONE2_ID,MODE_PLUS,0);
				break;
			case 'D':
				MsgSetVolume(ZONE2_ID,MODE_MINUS,0);
				break;
			case '?':
				rs232_printf("@Z2Vs%d\n",sys_flag.zone2_volume);
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}

	if(buffer[3] == 's')
		{
		if(GetProtocolFloatValue(buffer+4, & volume) == TRUE)
			{
			if(IsZone1VolumeValid((short)volume) == TRUE)
				{
				MsgSetVolume(ZONE2_ID,MODE_FIXED,(short) volume);
#ifdef _DEBUG_RS232
				rs232_printf("volume:%d\n",(short)volume);
#endif
				return TRUE;
				}
#ifdef _DEBUG_RS232
			rs232_printf("volume:%d,error\n",(short)volume);
#endif
			}
		}
	return FALSE;	
}

BOOL ProcCmdZ2M(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ2M,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':	//Z1M0
			case '1':	//Z1M1
				MsgSetMute(ZONE2_ID,MODE_FIXED,buffer[3] - '0');
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ2_(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ2?,%d\n",size);
#endif

	if(size==3)
		{
		switch(buffer[2])
			{
			case '?':
				rs232_printf("@Z2SV%dM%d\n",sys_flag.zone2_volume ,sys_flag.zone2_mute_flag);
				break;
			}
		return TRUE;
		}	
	return FALSE;
}

BOOL ProCmdZ2VFD_(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProCmdZ2VFD_?,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case '?': 
				if(sys_flag.system_zone2_status == POWER_ON)
					rs232_printf("@Z2:%s%d ",Zone2SrcName,sys_flag.zone2_volume); 
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdRSN(unsigned char *buffer,unsigned char size) 
{
	char *name; 
	uchar name_position; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdRSN,%d\n",size);
#endif

	switch(buffer[3])
		{
		case '0':
			name = SrcName[8];
			name_position = INPUT7CH1_NAME_POSITION;
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			name = SrcName[buffer[3] - '1'];
			name_position = (buffer[3] - '1')*INPUT_NAME_LENGTH + INPUT1_NAME_POSITION;
			break;
		default:
			return FALSE;
		}

	if(size > 4)
		{
		if(IsProtocolStringValid(buffer+4,INPUT_NAME_LENGTH) == TRUE)
			{
#ifdef _DEBUG_RS232
			rs232_printf("%s\n",buffer+4);
#endif
			memcpy(name,buffer+4,INPUT_NAME_LENGTH);
#ifdef _DEBUG_RS232
			rs232_printf("%s\n",name);
#endif
			WriteName(name_position, INPUT_NAME_LENGTH, name);
			}
		return TRUE;
		}
	return FALSE;	
}

BOOL IsBalanceValid(float volume)
{
	if((volume >= -15.0) && (volume <= 15.0))
		return TRUE;
	return FALSE;
}

BOOL ProcCmdSLL(unsigned char *buffer,unsigned char size) 
{
	float volume; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSLL,%d\n",size);
#endif

	if(buffer[3] == 's')
		{
		if(GetProtocolFloatValue(buffer+4, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_L,volume);
				EepromWriteByte(BALANCE_FL_POSITION,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSLC(unsigned char *buffer,unsigned char size) 
{
	float volume; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSLC,%d\n",size);
#endif

	if(buffer[3] == 's')
		{
		if(GetProtocolFloatValue(buffer+4, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_C,volume);
				EepromWriteByte(BALANCE_CEN_POSITION,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSLR(unsigned char *buffer,unsigned char size)
{
	float volume; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSLR,%d\n",size);
#endif

	if(buffer[3] == 's')
		{
		if(GetProtocolFloatValue(buffer+4, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_R,volume);
				EepromWriteByte(BALANCE_FR_POSITION,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSLSR(unsigned char *buffer,unsigned char size)
{
	float volume; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSLSR,%d\n",size);
#endif

	if(buffer[4] == 's')
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_RS,volume);
				EepromWriteByte(BALANCE_SR_POSITION,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSLSL(unsigned char *buffer,unsigned char size) 
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSLSL,%d\n",size);
#endif

	if(buffer[4] == 's')
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_LS,volume);
				EepromWriteByte(BALANCE_SL_POSITION,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSLBR(unsigned char *buffer,unsigned char size)
{
	float volume; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSLBR,%d\n",size);
#endif

	if(buffer[4] == 's')
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_RB,volume);
				EepromWriteByte(BLANCE_SBR_POSITION,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSLBL(unsigned char *buffer,unsigned char size) 
{
	float volume; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSLBL,%d\n",size);
#endif

	if(buffer[4] == 's')
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_LB,volume);
				EepromWriteByte(BLANCE_SBL_POSITION,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSLSW(unsigned char *buffer,unsigned char size) 
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSLSW,%d\n",size);
#endif

	if(buffer[4] == 's')
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_SW,volume);
				EepromWriteByte(BALANCE_SW_POSITION,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}		
	return FALSE;	
}

BOOL IsDistanceValid(float position,unsigned char mode)
{
	if(mode)
		{
		if((position >= 0) && (position <= 100.0))
			return TRUE;
		}
	else
		{
		if((position >= 0) && (position <= (100/METER2FEET)))
			return TRUE;
		}
	return FALSE;
}

BOOL ProcCmdSPL(unsigned char *buffer,unsigned char size) 
{
	float position; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSPL,%d\n",size);
#endif

	if(size > 3)
		{
		if(GetProtocolFloatValue(buffer+3, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_L);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_L);
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSPC(unsigned char *buffer,unsigned char size) 
{
	float position; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSPC,%d\n",size);
#endif	

	if(size > 3)
		{
		if(GetProtocolFloatValue(buffer+3, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode				
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_C);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_C);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSPR(unsigned char *buffer,unsigned char size) 
{
	float position;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSPR,%d\n",size);
#endif	

	if(size > 3)
		{
		if(GetProtocolFloatValue(buffer+3, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode				
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_R);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_R);
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSPSR(unsigned char *buffer,unsigned char size)
{
	float position; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSPSR,%d\n",size);
#endif	

	if(size > 4)
		{
		if(GetProtocolFloatValue(buffer+4, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode				
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_RS);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_RS);
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSPSL(unsigned char *buffer,unsigned char size) 
{
	float position; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSPSL,%d\n",size);
#endif	

	if(size > 4)
		{
		if(GetProtocolFloatValue(buffer+4, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode						
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_LS);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_LS);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSPBR(unsigned char *buffer,unsigned char size) 
{
	float position; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSPBR,%d\n",size);
#endif	

	if(size > 4)
		{
		if(GetProtocolFloatValue(buffer+4, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode						
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_RB);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_RB);
				return TRUE;
				}
			}
		}		
	return FALSE;
}

BOOL ProcCmdSPBL(unsigned char *buffer,unsigned char size) 
{
	float position; 
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSPBL,%d\n",size);
#endif	

	if(size > 4)
		{
		if(GetProtocolFloatValue(buffer+4, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode							
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_LB);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_LB);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSPs(unsigned char *buffer,unsigned char size) 
{
	float position; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSPs,%d\n",size);
#endif	

	if(size > 3)
		{
		if(GetProtocolFloatValue(buffer+3, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode							
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_SW);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_SW);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSZL(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZFL,%d\n",size);
#endif	

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':
			case '1':
				Movie_Music_Speaker_Flag=0; //Movie mode
				SetDspSpkMode(CH_L,buffer[3] - '0');
				SpkConfigSetup(WRITE_VALUE_MODE,buffer[3] - '0',CH_L);  
				return TRUE;
			}
		}		
	return FALSE;	
}

BOOL ProcCmdSZR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZFR,%d\n",size);
#endif	
	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':
			case '1':
				Movie_Music_Speaker_Flag=0; //Movie mode
				SetDspSpkMode(CH_R,buffer[3] - '0');
				SpkConfigSetup(WRITE_VALUE_MODE,buffer[3] - '0',CH_R);
				return TRUE;
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSZSL(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZSL,%d\n",size);
#endif	
	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':
			case '1':
				Movie_Music_Speaker_Flag=0; //Movie mode
				SetDspSpkMode(CH_LS,buffer[4] - '0');
				SpkConfigSetup(WRITE_VALUE_MODE,buffer[4] - '0',CH_LS);
				return TRUE;
			}
		}		
	return FALSE;
}

BOOL ProcCmdSZSR(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZSR,%d\n",size);
#endif	
	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':
			case '1':
				Movie_Music_Speaker_Flag=0; //Movie mode
				SetDspSpkMode(CH_RS,buffer[4] - '0');
				SpkConfigSetup(WRITE_VALUE_MODE,buffer[4] - '0',CH_RS);
				return TRUE;
			}
		}
	return FALSE;	
}

BOOL ProcCmdSZSBL(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZBL,%d\n",size);
#endif	
	if(size == 6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
				Movie_Music_Speaker_Flag=0; //Movie mode
				SetDspSpkMode(CH_LB,buffer[5] - '0');
				SpkConfigSetup(WRITE_VALUE_MODE,buffer[5] - '0',CH_LB);
				return TRUE;
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSZSBR(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZBR,%d\n",size);
#endif	
	if(size == 6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
				Movie_Music_Speaker_Flag=0; //Movie mode
				SetDspSpkMode(CH_RB,buffer[5] - '0');
				SpkConfigSetup(WRITE_VALUE_MODE,buffer[5] - '0',CH_RB);
				return TRUE;
			}
		}
	return FALSE;	
}

BOOL ProcCmdSZC(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZC,%d\n",size);
#endif	
	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':
			case '1':
				Movie_Music_Speaker_Flag=0; //Movie mode
				SetDspSpkMode(CH_C,buffer[3] - '0');
				SpkConfigSetup(WRITE_VALUE_MODE,buffer[3] - '0',CH_C);
				return TRUE;
			}
		}
	return FALSE;	
}

BOOL ProcCmdSZS(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZSW,%d\n",size);
#endif	

	if(size == 4)
		{
		switch(buffer[3])
			{
			case '0':
			case '1':
				Movie_Music_Speaker_Flag=0; //Movie mode
				SetDspSpkMode(CH_SW,buffer[3] - '0');
				SpkConfigSetup(WRITE_VALUE_MODE,buffer[3] - '0',CH_SW);
				return TRUE;
			}
		}	
	return FALSE;
}

BOOL IsCrossoverValid(float crossover)
{
	unsigned char point;
	point = (unsigned char)crossover;
	switch(point)
		{
		case 0:
		case 40:
		case 50:
		case 60:
		case 70:
		case 80:
		case 90:
		case 100:
		case 110:
		case 120:
		case 130:
		case 150:
			return TRUE;
		}
	return FALSE;
}

uchar GetCrossoverPoint(uchar crossover)
{
	switch(crossover)
		{
		case 0:			return 0;
		case 40:			return 1;
		case 50:			return 2;
		case 60:			return 3;
		case 70:			return 4;
		case 80:			return 5;
		case 90:			return 6;
		case 100:		return 7;
		case 110:		return 8;
		case 120:		return 9;
		case 130:		return 10;
		case 150:		return 11;
		}
	return 0;
}

BOOL ProcCmdSZXFL(unsigned char *buffer,unsigned char size) 
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZXFL,%d\n",size);
#endif		

	if(size > 5)
		{
		if(GetProtocolFloatValue(buffer+5, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_L);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_L);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSZXFC(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZXFC,%d\n",size);
#endif	

	if(size > 5)
		{
		if(GetProtocolFloatValue(buffer+5, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode				
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_C);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_C);
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSZXFR(unsigned char *buffer,unsigned char size) 
{
	float crossover;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZXFR,%d\n",size);
#endif	

	if(size > 5)
		{
		if(GetProtocolFloatValue(buffer+5, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode							
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_R);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_R);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSZXSR(unsigned char *buffer,unsigned char size) 
{
	float crossover;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZXSR,%d\n",size);
#endif	

	if(size > 5)
		{
		if(GetProtocolFloatValue(buffer+5, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode						
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_RS);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_RS);
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSZXSL(unsigned char *buffer,unsigned char size) 
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZXSL,%d\n",size);
#endif	

	if(size > 5)
		{
		if(GetProtocolFloatValue(buffer+5, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode							
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_LS);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_LS);
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdSZXSBR(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZXSBR,%d\n",size);
#endif	

	if(size > 6)
		{
		if(GetProtocolFloatValue(buffer+6, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode							
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_RB);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_RB);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSZXSBL(unsigned char *buffer,unsigned char size) 
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZXSBL,%d\n",size);
#endif	

	if(size > 6)
		{
		if(GetProtocolFloatValue(buffer+6, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=0; //Movie mode							
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_LB);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_LB);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdSZX(unsigned char *buffer,unsigned char size) 
{
	float crossover; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSZX,%d\n",size);
#endif	

	if(size > 3)
		{
		if(GetProtocolFloatValue(buffer+3, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE) 
				{
				Movie_Music_Speaker_Flag=0; //Movie mode						
				crossover = GetCrossoverPoint((unsigned char)crossover);				
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_SW);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_SW);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdZ1LPS(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1LPS,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0'://Movie mode
				Movie_Music_Speaker_Flag=0; //Movie mode
				ChangeListenProfileMode();
				break;
			case '1'://Music mode
				Movie_Music_Speaker_Flag=1; //Music mode
				ChangeListenProfileMode();				
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}
	return FALSE;
}

BOOL ProcCmdSSPL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSSPL,%d\n",size);
#endif	

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':
				SetSpkPhase(CH_L,0);
				EepromWriteByte(SPK_PHASE_POSITION+CH_L, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_L,1);
				EepromWriteByte(SPK_PHASE_POSITION+CH_L, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdSSPC(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSSPC,%d\n",size);
#endif	

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':
				SetSpkPhase(CH_C,0);
				EepromWriteByte(SPK_PHASE_POSITION+CH_C, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_C,1);
				EepromWriteByte(SPK_PHASE_POSITION+CH_C, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdSSPR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSSPR,%d\n",size);
#endif	

	if(size == 5)
		{
		switch(buffer[4])
			{
			case '0':
				SetSpkPhase(CH_R,0);
				EepromWriteByte(SPK_PHASE_POSITION+CH_R, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_R,1);
				EepromWriteByte(SPK_PHASE_POSITION+CH_R, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdSSPSR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSSPSR,%d\n",size);
#endif	

	if(size == 6)
		{
		switch(buffer[5])
			{
			case '0':
				SetSpkPhase(CH_RS,0);
				EepromWriteByte(SPK_PHASE_POSITION+CH_RS, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_RS,1);
				EepromWriteByte(SPK_PHASE_POSITION+CH_RS, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdSSPSL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSSPSL,%d\n",size);
#endif	

	if(size == 6)
		{
		switch(buffer[5])
			{
			case '0':
				SetSpkPhase(CH_LS,0);
				EepromWriteByte(SPK_PHASE_POSITION+CH_LS, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_LS,1);
				EepromWriteByte(SPK_PHASE_POSITION+CH_LS, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdSSPSBR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSSPSBR,%d\n",size);
#endif	

	if(size == 7)
		{
		switch(buffer[6])
			{
			case '0':
				SetSpkPhase(CH_RB,0);
				EepromWriteByte(SPK_PHASE_POSITION+CH_RB, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_RB,1);
				EepromWriteByte(SPK_PHASE_POSITION+CH_RB, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdSSPSBL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSSPSBL,%d\n",size);
#endif	

	if(size == 7)
		{
		switch(buffer[6])
			{
			case '0':
				SetSpkPhase(CH_LB,0);
				EepromWriteByte(SPK_PHASE_POSITION+CH_LB, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_LB,1);
				EepromWriteByte(SPK_PHASE_POSITION+CH_LB, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdSSPSW(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdSSPSW,%d\n",size);
#endif	

	if(size == 6)
		{
		switch(buffer[5])
			{
			case '0':
				SetSpkPhase(CH_SW,0);
				EepromWriteByte(SPK_PHASE_POSITION+CH_SW, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_SW,1);
				EepromWriteByte(SPK_PHASE_POSITION+CH_SW, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdMSZL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZL,%d\n",size);
#endif	

	if(size==5)
		{
		switch(buffer[4])
			{
			case '0':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_L,0);
				SpkConfigSetup(WRITE_VALUE_MODE,0,CH_L);				
				return TRUE;
			case '1':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_L,1);
				SpkConfigSetup(WRITE_VALUE_MODE,1,CH_L);	
				return TRUE;	
			}
		}
	return FALSE;
}

BOOL ProcCmdMSZR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZR,%d\n",size);
#endif	

	if(size==5)
		{
		switch(buffer[4])
			{
			case '0':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_R,0);
				SpkConfigSetup(WRITE_VALUE_MODE,0,CH_R);	
				return TRUE;
			case '1':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_R,1);
				SpkConfigSetup(WRITE_VALUE_MODE,1,CH_R);	
				return TRUE;	
			}
		}
	return FALSE;
}

BOOL ProcCmdMSZSL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZSL,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_LS,0);
				SpkConfigSetup(WRITE_VALUE_MODE,0,CH_LS);	
				return TRUE;
			case '1':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_LS,1);
				SpkConfigSetup(WRITE_VALUE_MODE,1,CH_LS);	
				return TRUE;	
			}
		}
	return FALSE;
}

BOOL ProcCmdMSZSR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZSR,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_RS,0);
				SpkConfigSetup(WRITE_VALUE_MODE,0,CH_RS);	
				return TRUE;
			case '1':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_RS,1);
				SpkConfigSetup(WRITE_VALUE_MODE,1,CH_RS);	
				return TRUE;	
			}
		}
	return FALSE;
}

BOOL ProcCmdMSZSBL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZSBL,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_LB,0);
				SpkConfigSetup(WRITE_VALUE_MODE,0,CH_LB);	
				return TRUE;
			case '1':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_LB,1);
				SpkConfigSetup(WRITE_VALUE_MODE,1,CH_LB);	
				return TRUE;	
			}
		}
	return FALSE;
}

BOOL ProcCmdMSZSBR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZSBR,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_RB,0);
				SpkConfigSetup(WRITE_VALUE_MODE,0,CH_RB);	
				return TRUE;
			case '1':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_RB,1);
				SpkConfigSetup(WRITE_VALUE_MODE,1,CH_RB);	
				return TRUE;	
			}
		}
	return FALSE;
}

BOOL ProcCmdMSZC(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZC,%d\n",size);
#endif	

	if(size==5)
		{
		switch(buffer[4])
			{
			case '0':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_C,0);
				SpkConfigSetup(WRITE_VALUE_MODE,0,CH_C);	
				return TRUE;
			case '1':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_C,1);
				SpkConfigSetup(WRITE_VALUE_MODE,1,CH_C);	
				return TRUE;	
			}
		}
	return FALSE;
}

BOOL ProcCmdMSZS(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZS,%d\n",size);
#endif	

	if(size==5)
		{
		switch(buffer[4])
			{
			case '0':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_SW,0);
				SpkConfigSetup(WRITE_VALUE_MODE,0,CH_SW);	
				return TRUE;
			case '1':
				Movie_Music_Speaker_Flag=1; //Music mode
				SetDspSpkMode(CH_SW,1);
				SpkConfigSetup(WRITE_VALUE_MODE,1,CH_SW);	
				return TRUE;	
			}
		}
	return FALSE;
}

BOOL ProcCmdMSPL(unsigned char *buffer,unsigned char size)
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSPL,%d\n",size);
#endif

	if(size>4)
		{
		if(GetProtocolFloatValue(buffer+4, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_L);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_L);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdMSPC(unsigned char *buffer,unsigned char size)
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSPC,%d\n",size);
#endif

	if(size>4)
		{
		if(GetProtocolFloatValue(buffer+4, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_C);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_C);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdMSPR(unsigned char *buffer,unsigned char size)
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSPR,%d\n",size);
#endif

	if(size>4)
		{
		if(GetProtocolFloatValue(buffer+4, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_R);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_R);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdMSPSR(unsigned char *buffer,unsigned char size)
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSPSR,%d\n",size);
#endif

	if(size>5)
		{
		if(GetProtocolFloatValue(buffer+5, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_RS);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_RS);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdMSPSL(unsigned char *buffer,unsigned char size)
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSPSL,%d\n",size);
#endif

	if(size>5)
		{
		if(GetProtocolFloatValue(buffer+5, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_LS);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_LS);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdMSPBR(unsigned char *buffer,unsigned char size)
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSPBR,%d\n",size);
#endif

	if(size>5)
		{
		if(GetProtocolFloatValue(buffer+5, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_RB);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_RB);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdMSPBL(unsigned char *buffer,unsigned char size)
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSPBL,%d\n",size);
#endif

	if(size>5)
		{
		if(GetProtocolFloatValue(buffer+5, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_LB);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_LB);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdMSPs(unsigned char *buffer,unsigned char size)
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSPs,%d\n",size);
#endif

	if(size>4)
		{
		if(GetProtocolFloatValue(buffer+4, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				SpkDistanceSetup(EXECUTE_MODE,(unsigned char)position,CH_SW);
				SpkDistanceSetup(WRITE_VALUE_MODE,(unsigned char)position,CH_SW);
				return TRUE;
				}
			}
		}  
	return FALSE;
}

BOOL ProcCmdMSLL(unsigned char *buffer,unsigned char size)
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSLL,%d\n",size);
#endif

	if(buffer[4] == 's')
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_L,volume);
				EepromWriteByte(BALANCE_FL_POSITION_MUSIC,FindBalancePosition(volume));
				return TRUE;
				}
			} 
		}
	return FALSE;	
}

BOOL ProcCmdMSLC(unsigned char *buffer,unsigned char size)
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSLC,%d\n",size);
#endif

	if(buffer[4] == 's')
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_C,volume);
				EepromWriteByte(BALANCE_CEN_POSITION_MUSIC,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdMSLR(unsigned char *buffer,unsigned char size)
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSLR,%d\n",size);
#endif

	if(buffer[4] == 's')
		{
		if(GetProtocolFloatValue(buffer+5, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_R,volume);
				EepromWriteByte(BALANCE_FR_POSITION_MUSIC,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdMSLSR(unsigned char *buffer,unsigned char size)
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSLSR,%d\n",size);
#endif

	if(buffer[5] == 's')
		{
		if(GetProtocolFloatValue(buffer+6, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_RS,volume);
				EepromWriteByte(BALANCE_SR_POSITION_MUSIC,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdMSLSL(unsigned char *buffer,unsigned char size)
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSLSL,%d\n",size);
#endif

	if(buffer[5] == 's')
		{
		if(GetProtocolFloatValue(buffer+6, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_LS,volume);
				EepromWriteByte(BALANCE_SL_POSITION_MUSIC,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdMSLBR(unsigned char *buffer,unsigned char size)
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSLBR,%d\n",size);
#endif

	if(buffer[5] == 's')
		{
		if(GetProtocolFloatValue(buffer+6, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_RB,volume);
				EepromWriteByte(BLANCE_SBR_POSITION_MUSIC,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdMSLBL(unsigned char *buffer,unsigned char size)
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSLBL,%d\n",size);
#endif

	if(buffer[5] == 's')
		{
		if(GetProtocolFloatValue(buffer+6, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_LB,volume);
				EepromWriteByte(BLANCE_SBL_POSITION_MUSIC,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdMSLSW(unsigned char *buffer,unsigned char size)
{
	float volume;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSLSW,%d\n",size);
#endif

	if(buffer[5] == 's')
		{
		if(GetProtocolFloatValue(buffer+6, & volume) == TRUE)
			{
			if(IsBalanceValid(volume) == TRUE)
				{
				SetBalanceVolume(CH_SW,volume);
				EepromWriteByte(BALANCE_SW_POSITION_MUSIC,FindBalancePosition(volume));
				return TRUE;
				}
			}
		}
	return FALSE;	
}

BOOL ProcCmdMSZXFL(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZXFL,%d\n",size);
#endif		

	if(size > 6)
		{
		if(GetProtocolFloatValue(buffer+6, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_L);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_L);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdMSZXFC(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZXFC,%d\n",size);
#endif		

	if(size > 6)
		{
		if(GetProtocolFloatValue(buffer+6, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_C);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_C);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdMSZXFR(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZXFR,%d\n",size);
#endif		

	if(size > 6)
		{
		if(GetProtocolFloatValue(buffer+6, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_R);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_R);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdMSZXSR(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZXSR,%d\n",size);
#endif		

	if(size > 6)
		{
		if(GetProtocolFloatValue(buffer+6, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_RS);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_RS);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdMSZXSBR(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZXSBR,%d\n",size);
#endif		

	if(size > 7)
		{
		if(GetProtocolFloatValue(buffer+7, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_RB);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_RB);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdMSZXSBL(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZXSBL,%d\n",size);
#endif		

	if(size > 7)
		{
		if(GetProtocolFloatValue(buffer+7, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_LB);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_LB);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdMSZXSL(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZXSL,%d\n",size);
#endif		

	if(size > 6)
		{
		if(GetProtocolFloatValue(buffer+6, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_LS);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_LS);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdMSZX(unsigned char *buffer,unsigned char size)
{
	float crossover; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSZX,%d\n",size);
#endif		

	if(size > 4)
		{
		if(GetProtocolFloatValue(buffer+4, & crossover) == TRUE)
			{
			if(IsCrossoverValid(crossover) == TRUE)
				{
				Movie_Music_Speaker_Flag=1; //Music mode
				crossover = GetCrossoverPoint((unsigned char)crossover);
				SpkCrossoverSetup(EXECUTE_MODE,(unsigned char)crossover,CH_SW);
				SpkCrossoverSetup(WRITE_VALUE_MODE,(unsigned char)crossover,CH_SW);
				return TRUE;
				}
			}
		}	
	return FALSE;	
}

BOOL ProcCmdMZ1EQL(unsigned char *buffer,unsigned char size)
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMZ1EQL,%d\n",size);
#endif

	if(size > 9)
		{
		if(buffer[6] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[7]))
				{
				if(GetProtocolFloatValue(buffer+8,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=1; //Music mode
						ProtocolSetEq(CH_L,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}		
 	return FALSE;
}

BOOL ProcCmdMZ1EQC(unsigned char *buffer,unsigned char size)
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQC,%d\n",size);
#endif

	if(size > 9)
		{
		if(buffer[6] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[7]))
				{
				if(GetProtocolFloatValue(buffer+8,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=1; //Music mode
						ProtocolSetEq(CH_C,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}		
 	return FALSE;	
}

BOOL ProcCmdMZ1EQR(unsigned char *buffer,unsigned char size)
{
	unsigned char band = 0;
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMZ1EQR,%d\n",size);
#endif

	if(size > 9)
		{
		if(buffer[6] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[7]))
				{
				if(GetProtocolFloatValue(buffer+8,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=1; //Music mode
						ProtocolSetEq(CH_R,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}	
 	return FALSE;
}

BOOL ProcCmdMZ1EQSR(unsigned char *buffer,unsigned char size)
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMZ1EQSR,%d\n",size);
#endif

	if(size > 10)
		{
		if(buffer[7] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[8]))
				{
				if(GetProtocolFloatValue(buffer+9,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=1; //Music mode
						ProtocolSetEq(CH_RS,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}	
 	return FALSE;
}

BOOL ProcCmdMZ1EQBSR(unsigned char *buffer,unsigned char size)
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMZ1EQBSR,%d\n",size);
#endif

	if(size > 11)
		{
		if(buffer[8] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[9]))
				{
				if(GetProtocolFloatValue(buffer+10,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=1; //Music mode
						ProtocolSetEq(CH_RB,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}
 	return FALSE;	
}

BOOL ProcCmdMZ1EQBSL(unsigned char *buffer,unsigned char size)
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMZ1EQBSL,%d\n",size);
#endif

	if(size > 11)
		{
		if(buffer[8] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[9]))
				{
				if(GetProtocolFloatValue(buffer+10,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=1; //Music mode
						ProtocolSetEq(CH_LB,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}		
 	return FALSE;	
}

BOOL ProcCmdMZ1EQSL(unsigned char *buffer,unsigned char size)
{
	unsigned char band = 0; 
	float eq_value = 0; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMZ1EQSL,%d\n",size);
#endif

	if(size > 10)
		{
		if(buffer[7] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[8]))
				{
				if(GetProtocolFloatValue(buffer+9,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=1; //Music mode						
						ProtocolSetEq(CH_LS,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}	
 	return FALSE;	
}

BOOL ProcCmdMZ1EQSW(unsigned char *buffer,unsigned char size)
{
	unsigned char band = 0;
	float eq_value = 0;
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EQSW,%d\n",size);
#endif

	if(size > 10)
		{
		if(buffer[7] == 's')
			{
			if(TRUE == IsProtocolEqBandValid(&band,buffer[8]))
				{
				if(GetProtocolFloatValue(buffer+9,&eq_value) == TRUE)
					{
					if(IsProtocolEqValid(eq_value) == TRUE)
						{
						Movie_Music_Speaker_Flag=1; //Music mode							
						ProtocolSetEq(CH_SW,band,eq_value);
						return TRUE;
						}
					}
				}
			}
		}	
 	return FALSE;
}

BOOL ProcCmdMSSPL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSSPL,%d\n",size);
#endif	

	if(size == 6)
		{
		switch(buffer[5])
			{
			case '0':
				SetSpkPhase(CH_L,0);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_L, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_L,1);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_L, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdMSSPC(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSSPC,%d\n",size);
#endif	

	if(size == 6)
		{
		switch(buffer[5])
			{
			case '0':
				SetSpkPhase(CH_C,0);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_C, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_C,1);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_C, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdMSSPR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSSPR,%d\n",size);
#endif	

	if(size == 6)
		{
		switch(buffer[5])
			{
			case '0':
				SetSpkPhase(CH_R,0);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_R, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_R,1);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_R, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdMSSPSR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSSPSR,%d\n",size);
#endif	

	if(size == 7)
		{
		switch(buffer[6])
			{
			case '0':
				SetSpkPhase(CH_RS,0);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_RS, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_RS,1);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_RS, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdMSSPSL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSSPSL,%d\n",size);
#endif	

	if(size == 7)
		{
		switch(buffer[6])
			{
			case '0':
				SetSpkPhase(CH_LS,0);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_LS, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_LS,1);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_LS, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdMSSPSBR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSSPSL,%d\n",size);
#endif	

	if(size == 8)
		{
		switch(buffer[7])
			{
			case '0':
				SetSpkPhase(CH_RB,0);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_RB, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_RB,1);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_RB, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdMSSPSBL(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSSPSBL,%d\n",size);
#endif	

	if(size == 8)
		{
		switch(buffer[7])
			{
			case '0':
				SetSpkPhase(CH_LB,0);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_LB, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_LB,1);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_LB, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdMSSPSW(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdMSSPSW,%d\n",size);
#endif	

	if(size == 7)
		{
		switch(buffer[6])
			{
			case '0':
				SetSpkPhase(CH_SW,0);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_SW, 0);
				return TRUE;
			case '1':
				SetSpkPhase(CH_SW,1);
				EepromWriteByte(SPK_PHASE_POSITION_MUSIC+CH_SW, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1EMDIM(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EMDIM,%d\n",size);
#endif

	if(size==8)
		{
		switch(buffer[7])
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':	
				Set_Prologic2Dimension(buffer[7] - '0'); 
				EepromWriteByte(SPK_PANORAMA_DIMENSION_MUSIC, buffer[7] - '0');
				return TRUE;
			case '?':
				bValue = EepromReadByte(SPK_PANORAMA_DIMENSION_MUSIC);
				rs232_printf("@Z1EMDIM%d\n",bValue);
				return TRUE;
			}
		}	
	return FALSE;	
}

BOOL ProcCmdZ1EMP(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EMP,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				Set_Prologic2Panorama_On_Or_Off(0); 
				EepromWriteByte(SPK_PANORAMA_PANORAMA_MUSIC , 0);
				return TRUE;
			case '1':
				Set_Prologic2Panorama_On_Or_Off(1); 
				EepromWriteByte(SPK_PANORAMA_PANORAMA_MUSIC , 1);
				return TRUE;	
			case '?':
				bValue = EepromReadByte(SPK_PANORAMA_PANORAMA_MUSIC);
				rs232_printf("@Z1EMP%d\n",bValue);
				return TRUE;
			}
		}	
	return FALSE;
}

BOOL ProcCmdZ1EMC(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EMC,%d\n",size);
#endif	
	
	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':	
				Set_Prologic2Width(buffer[5] - '0');
				EepromWriteByte(SPK_PANORAMA_WIDTH_MUSIC , buffer[5] - '0');
				return TRUE;		
			case '?':
				bValue = EepromReadByte(SPK_PANORAMA_WIDTH_MUSIC);
				rs232_printf("@Z1EMC%d\n",bValue);
				return TRUE;
			}
		}	
	return FALSE;	
}

BOOL ProcCmdZ1EMDEL(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1EMDEL,%d\n",size);
#endif	

	if(size==8)
		{
		switch(buffer[7])
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':	
			case '8':
			case '9':
				DSP_AV_Delay(buffer[7] - '0');
				EepromWriteByte(SPK_PANORAMA_DELAY_MUSIC, buffer[7] - '0');
				return TRUE;			
			case '?':
				bValue = EepromReadByte(SPK_PANORAMA_DELAY_MUSIC);
				rs232_printf("@Z1EMDEL%d\n",bValue);
				return TRUE;
			}
		}	
	if(size==9)
		{
		if((buffer[7] == '1') && (buffer[8] == '0')) 
			{
			DSP_AV_Delay(10);
			EepromWriteByte(SPK_PANORAMA_DELAY_MUSIC, 10);	
			return TRUE;
			}
		if((buffer[7] == '1') && (buffer[8] == '1')) 
			{
			DSP_AV_Delay(11);
			EepromWriteByte(SPK_PANORAMA_DELAY_MUSIC, 11);
			return TRUE;
			}
		if((buffer[7] == '1') && (buffer[8] == '2')) 
			{
			DSP_AV_Delay(12);
			EepromWriteByte(SPK_PANORAMA_DELAY_MUSIC, 12);
			return TRUE;
			}
		if((buffer[7] == '1') && (buffer[8] == '3')) 
			{
			DSP_AV_Delay(13);
			EepromWriteByte(SPK_PANORAMA_DELAY_MUSIC, 13);
			return TRUE;
			}
		if((buffer[7] == '1') && (buffer[8] == '4')) 
			{
			DSP_AV_Delay(14);
			EepromWriteByte(SPK_PANORAMA_DELAY_MUSIC, 14);
			return TRUE;
			}	
		if((buffer[7] == '1') && (buffer[8] == '5')) 
			{
			DSP_AV_Delay(15);
			EepromWriteByte(SPK_PANORAMA_DELAY_MUSIC, 15);
			return TRUE;
			}
		}
	return FALSE;	
}

BOOL ProcCmdZ1VFL_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VFL_,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '?':
				if(Movie_Music_Speaker_Flag==0) 
					{
					bValue = EepromReadByte(BALANCE_FL_POSITION); //Movie mode
					}
				else
					{
					bValue = EepromReadByte(BALANCE_FL_POSITION_MUSIC); //Music mode
					}
				
				rs232_printf("@Z1VFL:%03f DB\n",balance_table[bValue]);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1VFR_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VFR_,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '?':
				if(Movie_Music_Speaker_Flag==0) 
					{
					bValue = EepromReadByte(BALANCE_FR_POSITION); //Movie mode
					}
				else
					{
					bValue = EepromReadByte(BALANCE_FR_POSITION_MUSIC); //Music mode
					}
				
				rs232_printf("@Z1VFR:%03f DB\n",balance_table[bValue]);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1VC_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VC_,%d\n",size);
#endif	

	if(size==5)
		{
		switch(buffer[4])
			{
			case '?':
				if(Movie_Music_Speaker_Flag==0) 
					{
					bValue = EepromReadByte(BALANCE_CEN_POSITION); //Movie mode
					}
				else
					{
					bValue = EepromReadByte(BALANCE_CEN_POSITION_MUSIC); //Music mode
					}
				
				rs232_printf("@Z1VC:%03f DB\n",balance_table[bValue]);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1VSL_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VSL_,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '?':
				if(Movie_Music_Speaker_Flag==0) 
					{
					bValue = EepromReadByte(BALANCE_SL_POSITION); //Movie mode
					}
				else
					{
					bValue = EepromReadByte(BALANCE_SL_POSITION_MUSIC); //Music mode
					}			
				rs232_printf("@Z1VSL:%03f DB\n",balance_table[bValue]);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1VSR_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VSR_,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '?':
				if(Movie_Music_Speaker_Flag==0) 
					{
					bValue = EepromReadByte(BALANCE_SR_POSITION); //Movie mode
					}
				else
					{
					bValue = EepromReadByte(BALANCE_SR_POSITION_MUSIC); //Music mode
					}
				
				rs232_printf("@Z1VSR:%03f DB\n",balance_table[bValue]);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1VBL_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VBL_,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '?':
				if(Movie_Music_Speaker_Flag==0) 
					{
					bValue = EepromReadByte(BLANCE_SBL_POSITION); //Movie mode
					}
				else
					{
					bValue = EepromReadByte(BLANCE_SBL_POSITION_MUSIC); //Music mode
					}
				
				rs232_printf("@Z1VFBL:%03f DB\n",balance_table[bValue]);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1VBR_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VBR_,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '?':
				if(Movie_Music_Speaker_Flag==0) 
					{
					bValue = EepromReadByte(BLANCE_SBR_POSITION); //Movie mode
					}
				else
					{
					bValue = EepromReadByte(BLANCE_SBR_POSITION_MUSIC); //Music mode
					}
				
				rs232_printf("@Z1VBR:%03f DB\n",balance_table[bValue]);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1VS_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1VS_,%d\n",size);
#endif	

	if(size==5)
		{
		switch(buffer[4])
			{
			case '?':
				if(Movie_Music_Speaker_Flag==0) 
					{
					bValue = EepromReadByte(BALANCE_SW_POSITION); //Movie mode
					}
				else
					{
					bValue = EepromReadByte(BALANCE_SW_POSITION_MUSIC); //Music mode
					}
				
				rs232_printf("@Z1VS:%03f DB\n",balance_table[bValue]);
				return TRUE;
			}
		}
	return FALSE;
}

extern unsigned char DSDType;
void Rs232UpdateStreamType(unsigned char type,unsigned char sub_type)
{
	uchar decode_mode;
	
	//if(sys_flag.zone1_mute_flag == ON)
	//	{
	//	rs232_printf("Mute");
	//	return;
	//	}
	if(sys_flag.src_mode == 2)
		{
		if(sys_flag.zone1_channel == INPUT_CH71)
			rs232_printf("Bypass");
		else
			rs232_printf("Stereo");
		return;
		}
	if(sys_flag.zone1_channel == INPUT_CH71) /*7.1 Analog 输入*/
		{
		rs232_printf("PCM        48kHz");
		return;    
		}
	
	switch(type)
		{
		case STREAM_NONE:
			rs232_printf("NO SIGNAL");
			break;
		case STREAM_DSD:
			if(DSDType==_DSD_2CH) 
				rs232_printf("DSD 2.0             ");
			else
				rs232_printf("DSD 5.1             ");
			break;
		case STREAM_PCM:
		if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))				
			{
				decode_mode = sys_flag.decode_mode.pcm_decode_mode;
				switch(PcmType)    
					{
					case _PCM_2_1:
						switch(decode_mode)
							{
							case PCM_MODE_NORMAL:
								rs232_printf("LPCM 2.0            ");
								break;
							case PCM_MODE_PL2:
								if(sys_flag.decode_mode.pcm_sur_mode)
									rs232_printf("DOLBY PL II  Movie");
								else
									rs232_printf("DOLBY PL II  Music");
								break;
							case PCM_MODE_PL2X:
								if(sys_flag.decode_mode.pcm_sur_mode)
									rs232_printf("DOLBY PL IIx  Movie");
								else
									rs232_printf("DOLBY PL IIx  Music");
								break;
							case PCM_MODE_NEO6:
								if(sys_flag.decode_mode.pcm_sur_mode)
									rs232_printf("DTS NEO:6    Cinema");
								else
									rs232_printf("DTS NEO:6    Music");
								break;
							case PCM_MODE_CES:
								switch(sys_flag.decode_mode.pcm_sur_mode)    
									{  
									case 0:
										rs232_printf("CES 7.1 ON");
										break;
									case 1:
										rs232_printf("CES 7.1 All Stereo");
										break;
									case 2:
										rs232_printf("CES 7.1 All Mono");
										break;
									default:
										rs232_printf("CES 7.1 ON");
										break;
									}
								break;
							}			
						break;
					case _PCM_5_1:
						switch(decode_mode)
							{
							case PCM_MODE_NORMAL:
								rs232_printf("LPCM 5.1            ");
								break;
							case PCM_MODE_PL2X:
								if(sys_flag.decode_mode.pcm_sur_mode)
									rs232_printf("DOLBY PL IIx  Movie");
								else
									rs232_printf("DOLBY PL IIx  Music");
								break;
							case PCM_MODE_NEO6:
								if(sys_flag.decode_mode.pcm_sur_mode)
									rs232_printf("DTS NEO:6    Cinema");
								else
									rs232_printf("DTS NEO:6    Music");
								break;
							case PCM_MODE_CES:
								switch(sys_flag.decode_mode.pcm_sur_mode)    
									{  
									case 0:
										rs232_printf("CES 7.1 ON");
										break;
									case 1:
										rs232_printf("CES 7.1 All Stereo");
										break;
									case 2:
										rs232_printf("CES 7.1 All Mono");
										break;
									default:
										rs232_printf("CES 7.1 ON");
										break;
									}
								break;
							}
						break;
					case _PCM_7_1:
						rs232_printf("LPCM 7.1            ");
						break;
					default:
						break;	
					}
				}
			else
				{
				decode_mode = sys_flag.decode_mode.pcm_decode_mode;

				if(GetNormalFreq() == FREQ_192KHZ)	//192K不做其他效果
					decode_mode = PCM_MODE_NORMAL;
				//if((GetNormalFreq() == FREQ_96KHZ) && (decode_mode == PCM_MODE_NEO6))
				//	decode_mode = PCM_MODE_NORMAL;

				switch(decode_mode)
					{
					case PCM_MODE_NORMAL:
						if(sys_flag.src_mode == 1)
							{
							rs232_printf("PCM      48kHz");
							}
						else
							{
							switch(GetNormalFreq())
								{
								case FREQ_44_1KHZ:
									rs232_printf("PCM      44.1kHz");
									break;
								case FREQ_48KHZ:
									rs232_printf("PCM      48kHz");
									break;
								case FREQ_96KHZ:
									rs232_printf("PCM      96kHz");
									break;
								case FREQ_192KHZ:
									rs232_printf("PCM      192kHz");
									break;
								}
							}
						break;
					case PCM_MODE_PL2:
						if(sys_flag.decode_mode.pcm_sur_mode)
							rs232_printf("DOLBY PL II  Movie");
						else
							rs232_printf("DOLBY PL II  Music");
						break;
					case PCM_MODE_PL2X: 
						if(sys_flag.decode_mode.pcm_sur_mode)
							rs232_printf("DOLBY PL IIx  Movie");
						else
							rs232_printf("DOLBY PL IIx  Music");
						break;
					case PCM_MODE_NEO6:
						if(sys_flag.decode_mode.pcm_sur_mode)
							rs232_printf("DTS NEO:6    Cinema");
						else
							rs232_printf("DTS NEO:6    Music");
						break;  
					case PCM_MODE_CES:
						switch(sys_flag.decode_mode.pcm_sur_mode)    
							{  
							case 0:
								rs232_printf("CES 7.1 ON");
								break;
							case 1:
								rs232_printf("CES 7.1 All Stereo");
								break;
							case 2:
								rs232_printf("CES 7.1 All Mono");
								break;
							default:
								rs232_printf("CES 7.1 ON");
								break;
							}
						break;
					}
				}
			break;
			
		case STREAM_AC3:
			decode_mode = sys_flag.decode_mode.dolby_decode_mode;
			if(sys_flag.headphone_flag)
				{
				switch(sys_flag.decode_mode.dolby_type)
					{
					case _DOLBY_2CH:
					case _DOLBY_5CH:
					case _DOLBY_EX:
						rs232_printf("DOLBY Headphone     ");
						break;
					case _DOLBY_PLUS: /*高清不支持Headphone*/
						rs232_printf("DOLBY DIGITAL PLUS");
						break;
					case _DOLBY_TRUE_HD_2ch:
						rs232_printf("DOLBY TRUEHD 2.0");
						break;
					case _DOLBY_TRUE_HD_5ch:
						rs232_printf("DOLBY TRUEHD 3/2.1");
						break;
					case _DOLBY_TRUE_HD_6ch:
						rs232_printf("DOLBY TRUEHD 3/3.1");
						break;
					case  _DOLBY_TRUE_HD_7ch: /*高清不支持Headphone*/
						rs232_printf("DOLBY TRUEHD 3/4.1");
						break;
					default :
						break;
					}
				}
			else
				{
				switch(sys_flag.decode_mode.dolby_type)
					{
					case _DOLBY_2CH:
						switch(decode_mode)
							{
							case AC3_MODE_CES_ON:  
								switch(sys_flag.decode_mode.dolby_sur_mode)
									{
									case 0:
										rs232_printf("CES 7.1 ON");
										break;
									case 1:
										rs232_printf("CES 7.1 All Stereo");
										break;
									case 2:
										rs232_printf("CES 7.1 All Mono");
										break;
									default:
										break;      
									}
								break;
							case AC3_MODE_PL2:
								if(sys_flag.decode_mode.dolby_sur_mode)
									rs232_printf("DOLBY PL II  Movie");
								else
									rs232_printf("DOLBY PL II  Music");
								break;
							case AC3_MODE_PL2X:
								if(sys_flag.decode_mode.dolby_sur_mode)
									rs232_printf("DOLBY PL IIx  Movie");
								else
									rs232_printf("DOLBY PL IIx  Music");
								break;
							case AC3_MODE_NORMAL:
							default:
								rs232_printf("DOLBY DIGITAL 2.0");
								break;
							}
						break;
					case _DOLBY_5CH:
						switch(decode_mode)  
							{
							case AC3_MODE_CES_ON:
								rs232_printf("CES 7.1 ON");			
							break;
							case AC3_MODE_PL2X:
								if(sys_flag.decode_mode.dolby_sur_mode)
									rs232_printf("DOLBY PL IIx  Movie");
								else
									rs232_printf("DOLBY PL IIx  Music");
								break;
							case AC3_MODE_EX_ON:
								rs232_printf("DOLBY DIGITAL EX ON");
								break;
							case AC3_MODE_EX_OFF:
								//rs232_printf("DOLBY DIGITAL 3/2.1");
								//rs232_printf(VFD_UPDATE_LINE2,50,"DOLBY DIGITAL EX OFF");
								sys_flag.decode_mode.dolby_decode_mode = AC3_MODE_NORMAL;
								break;
							case AC3_MODE_NORMAL:
							default:
								rs232_printf("DOLBY DIGITAL 3/2.1");
								break;
							}
						break;
					case _DOLBY_EX:
						switch(decode_mode)
							{
							case AC3_MODE_CES_ON:
								rs232_printf("CES 7.1 ON");					
								break;
							default:
								rs232_printf("DOLBY DIGITAL EX");
								break;
							}
						break;
					case _DOLBY_PLUS:
						//switch(decode_mode)  
							//{
							//case AC3_MODE_NORMAL:
								rs232_printf("DOLBY DIGITAL PLUS");
							//	break;
							//}
						break;
					case _DOLBY_TRUE_HD_2ch:
						switch(decode_mode)
							{
							case AC3_MODE_PL2X:
								rs232_printf("DOLBY TRUEHD + PLIIx"); 
								break;
							case AC3_MODE_NORMAL:
								rs232_printf("DOLBY TRUEHD 2.0");
								break;
							}
						break;
					case _DOLBY_TRUE_HD_5ch:
						switch(decode_mode)
							{
							case AC3_MODE_PL2X:
								rs232_printf("DOLBY TRUEHD + PLIIx"); 
								break;
							case AC3_MODE_NORMAL:
								rs232_printf("DOLBY TRUEHD 3/2.1");
								break;
							}	
						break;
					case _DOLBY_TRUE_HD_6ch:
						switch(decode_mode)
							{
							case AC3_MODE_PL2X:
								rs232_printf("DOLBY TRUEHD + PLIIx"); 
								break;
							case AC3_MODE_NORMAL:
								rs232_printf("DOLBY TRUEHD 3/3.1");
								break;
							}	
						break;

					 case _DOLBY_TRUE_HD_7ch:
					 	rs232_printf("DOLBY TRUEHD 3/4.1");
					 	break;
					}
				}
			break;
		case STREAM_DTS:
			decode_mode = sys_flag.decode_mode.dts_decode_mode;			
			if(sys_flag.headphone_flag)
				decode_mode = DTS_MODE_NORMAL; //DTS 无Headphone 效果				
			switch(sub_type)
				{
				case _DTS:
					if(decode_mode == DTS_MODE_CES_ON)
						rs232_printf("CES 7.1 ON");
					else
						rs232_printf("dts");
					break;
				case _DTS_ES_DISCRETE:
					rs232_printf("dts-ES Discrete");
					break;
				case _DTS_ES_MATRIX:  
					rs232_printf("dts-ES Matrix");
					break;
				case _DTS_96_24:
					if(decode_mode == DTS_MODE_CES_ON)
						rs232_printf("CES 7.1 ON");
					else
						rs232_printf("dts 96/24");		
					break;
				case _DTS_HD_MA_48K:
					switch(sys_flag.decode_mode.dts_type)
						{
						case _DTS_HD_MA_48K_2ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD MA + PL IIx");
							else
								rs232_printf("dts-HD MA 2.0 48K");
							break;
						case _DTS_HD_MA_48K_5ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD MA + PL IIx");
							else
								rs232_printf("dts-HD MA 3/2.1 48K");
							break;
						case _DTS_HD_MA_48K_6ch: // jian add in 11-10-10
						//	if(decode_mode ==DTS_MODE_PLIIX)
						//		rs232_printf("dts-HD MA + PL IIx");
						//	else
								rs232_printf("dts-HD MA 3/3.1 48K");
							break;
						
						case _DTS_HD_MA_48K_7ch:
							rs232_printf("dts-HD MA 3/4.1 48K");
							break;
						default:
							break;
						}
					break;
				case _DTS_HD_MA_96k_192K:
					switch(sys_flag.decode_mode.dts_type)
						{
						case _DTS_HD_MA_96K_2ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD MA + PL IIx");
							else
								rs232_printf("dts-HD MA 2.0 96K");
							break;
						case _DTS_HD_MA_96K_5ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD MA + PL IIx");
							else
								rs232_printf("dts-HD MA 3/2.1 96K");
							break;
						case _DTS_HD_MA_96K_7ch:
							rs232_printf("dts-HD MA 3/4.1 96K");
							break;
						default:
							break;
						}
					break;
				case _DTS_HD_HRA_48K:
					switch(sys_flag.decode_mode.dts_type)
					{
						case _DTS_HD_HRA_48K_2ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD HRA + PLIIX");
							else
								rs232_printf("dts-HD HRA 2.0 48K");
							break;
						case _DTS_HD_HRA_48K_6ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD HRA + PLIIX");
							else
								rs232_printf("dts-HD HRA 3/2.1 48K");
							break;
						case _DTS_HD_HRA_48K_7ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD HRA + PLIIX");
							else
								rs232_printf("dts-HD HRA 3/3.1 48K");
							break;
						case _DTS_HD_HRA_48K_8ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD HRA + PLIIX");
							else
								rs232_printf("dts-HD HRA 3/4.1 48K");
							break;
						default:
							break;
					}
					break;
				case _DTS_HD_HRA_96K:
					switch(sys_flag.decode_mode.dts_type)
					{
						case _DTS_HD_HRA_96K_2ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD HRA + PLIIX");
							else
								rs232_printf("dts-HD HRA 2.0 96K");
							break;
//							break;
						case _DTS_HD_HRA_96K_6ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD HRA + PLIIX");
							else
								rs232_printf("dts-HD HRA 3/2.1 96K");
							break;
							break;
						case _DTS_HD_HRA_96K_7ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD HRA + PLIIX");
							else
								rs232_printf("dts-HD HRA 3/3.1 96K");
							break;
						case _DTS_HD_HRA_96K_8ch:
							if(decode_mode ==DTS_MODE_PLIIX)
								rs232_printf("dts-HD HRA + PLIIX");
							else
								rs232_printf("dts-HD HRA 3/4.1 96K");
							break;
						default:
							break;
					}
					break;
				}
			break;
		}
}

extern char * Zone1SrcName;
BOOL ProCmdZ1VFD_(unsigned char *buffer,unsigned char size)
{
	unsigned char bValue; 

#ifdef _DEBUG_RS232
	rs232_printf("ProCmdZ1VFD_,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '?':
				rs232_printf("@Z1:%s%d ",Zone1SrcName,sys_flag.zone1_volume);
				rs232_printf("\n ");  
				Rs232UpdateStreamType(sys_flag.stream_type,sys_flag.stream_subtype);
				return TRUE;
			}
		}

	return FALSE;
}

BOOL ProcCmdZ1IN1(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1IN1,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
				EepromWriteByte(ASSIGNED_INPUT1, buffer[5]-'0');
				if(sys_flag.zone1_channel == INPUT_1)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1IN2(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1IN2,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
			case '2':
				EepromWriteByte(ASSIGNED_INPUT2, buffer[5]-'0');
				if(sys_flag.zone1_channel == INPUT_2)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1IN3(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1IN3,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
			case '2':
				EepromWriteByte(ASSIGNED_INPUT3, buffer[5]-'0');
				if(sys_flag.zone1_channel == INPUT_3)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1IN4(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1IN4,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
			case '2':
				EepromWriteByte(ASSIGNED_INPUT4, buffer[5]-'0');
				if(sys_flag.zone1_channel == INPUT_4)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1IN5(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1IN5,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
			case '2':
				EepromWriteByte(ASSIGNED_INPUT5, buffer[5]-'0');
				if(sys_flag.zone1_channel == INPUT_5)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1IN6(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1IN6,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
			case '2':
				EepromWriteByte(ASSIGNED_INPUT6, buffer[5]-'0');
				if(sys_flag.zone1_channel == INPUT_6)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1IN7(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1IN7,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
			case '2':
				EepromWriteByte(ASSIGNED_INPUT7, buffer[5]-'0');
				if(sys_flag.zone1_channel == INPUT_7)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1IN8(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1IN8,%d\n",size);
#endif	

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
			case '1':
			case '2':
				EepromWriteByte(ASSIGNED_INPUT8, buffer[5]-'0');
				if(sys_flag.zone1_channel == INPUT_8)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL IsAnalogLevelValid(float level)
{
	if((level >= -18.0) && (level <= 12.0))
		return TRUE;
	return FALSE;
}

unsigned char FindLevelPosition(float level_value)
{
	unsigned char i;

	level_value *= 2;
	level_value = (short)level_value;
	level_value /= 2;

	for(i=0;i<31;i++)
		{
		if(level_value == AnalogInLevelTable[i])
			{
			return i; 
			} 
		}
	if(level_value < AnalogInLevelTable[0])
		return 0;
	if(level_value > AnalogInLevelTable[30])
		return 30;
	
	return 18; //default value
}

BOOL ProcCmdZ1SAL1(unsigned char *buffer,unsigned char size)
{
	float level; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1SAL1,%d\n",size);
#endif	

	if(GetProtocolFloatValue(buffer+6, & level) == TRUE)
		{
		if(IsAnalogLevelValid(level) == TRUE)
			{
			AnalogInLevel[INPUT_1] = FindLevelPosition(level);
			Set2ChVolume(level);  
			EepromWriteByte(ANALOG1_LEVEL_POSITION, FindLevelPosition(level));
			return TRUE;
			}
		}
	return FALSE;	
}

BOOL ProcCmdZ1SAL2(unsigned char *buffer,unsigned char size)
{
	float level; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1SAL2,%d\n",size);
#endif	

	if(GetProtocolFloatValue(buffer+6, & level) == TRUE)
		{
		if(IsAnalogLevelValid(level) == TRUE)
			{
			AnalogInLevel[INPUT_2] = FindLevelPosition(level);
			Set2ChVolume(level);  
			EepromWriteByte(ANALOG2_LEVEL_POSITION, FindLevelPosition(level));
			return TRUE;
			}
		}
	return FALSE;	
}

BOOL ProcCmdZ1SAL3(unsigned char *buffer,unsigned char size)
{
	float level; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1SAL3,%d\n",size);
#endif	

	if(GetProtocolFloatValue(buffer+6, & level) == TRUE)
		{
		if(IsAnalogLevelValid(level) == TRUE)
			{
			AnalogInLevel[INPUT_3] = FindLevelPosition(level);
			Set2ChVolume(level);  
			EepromWriteByte(ANALOG3_LEVEL_POSITION, FindLevelPosition(level));
			return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1SAL4(unsigned char *buffer,unsigned char size)
{
	float level; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1SAL4,%d\n",size);
#endif	

	if(GetProtocolFloatValue(buffer+6, & level) == TRUE)
		{
		if(IsAnalogLevelValid(level) == TRUE)
			{
			AnalogInLevel[INPUT_4] = FindLevelPosition(level);
			Set2ChVolume(level);  
			EepromWriteByte(ANALOG4_LEVEL_POSITION, FindLevelPosition(level));
			return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1SAL5(unsigned char *buffer,unsigned char size)
{
	float level; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1SAL5,%d\n",size);
#endif	

	if(GetProtocolFloatValue(buffer+6, & level) == TRUE)
		{
		if(IsAnalogLevelValid(level) == TRUE)
			{
			AnalogInLevel[INPUT_5] = FindLevelPosition(level);
			Set2ChVolume(level);  
			EepromWriteByte(ANALOG5_LEVEL_POSITION, FindLevelPosition(level));
			return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1SAL6(unsigned char *buffer,unsigned char size)
{
	float level; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1SAL6,%d\n",size);
#endif	

	if(GetProtocolFloatValue(buffer+6, & level) == TRUE)
		{
		if(IsAnalogLevelValid(level) == TRUE)
			{
			AnalogInLevel[INPUT_6] = FindLevelPosition(level);
			Set2ChVolume(level);  
			EepromWriteByte(ANALOG6_LEVEL_POSITION, FindLevelPosition(level));
			return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1SAL7(unsigned char *buffer,unsigned char size)
{
	float level; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1SAL7,%d\n",size);
#endif	

	if(GetProtocolFloatValue(buffer+6, & level) == TRUE)
		{
		if(IsAnalogLevelValid(level) == TRUE)
			{
			AnalogInLevel[INPUT_7] = FindLevelPosition(level);
			Set2ChVolume(level);  
			EepromWriteByte(ANALOG7_LEVEL_POSITION, FindLevelPosition(level));
			return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1SAL8(unsigned char *buffer,unsigned char size)
{
	float level; 

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1SAL8,%d\n",size);
#endif	

	if(GetProtocolFloatValue(buffer+6, & level) == TRUE)
		{
		if(IsAnalogLevelValid(level) == TRUE)
			{
			AnalogInLevel[INPUT_8] = FindLevelPosition(level);
			Set2ChVolume(level);  
			EepromWriteByte(ANALOG8_LEVEL_POSITION, FindLevelPosition(level));
			return TRUE;
			}
		}
	return FALSE;
}


BOOL ProcCmdZ1ACN1(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN1,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_1] = buffer[6]-'0';
				if(sys_flag.zone1_channel == INPUT_1)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);				
				EepromWriteByte(ANALOG1_MODE_POSITION, buffer[6]-'0');
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACN2(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN2,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_2] = buffer[6]-'0';
				if(sys_flag.zone1_channel == INPUT_2)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);				
				EepromWriteByte(ANALOG2_MODE_POSITION, buffer[6]-'0');				
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACN3(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN3,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_3] = buffer[6]-'0';
				if(sys_flag.zone1_channel == INPUT_3)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);				
				EepromWriteByte(ANALOG3_MODE_POSITION, buffer[6]-'0');	
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACN4(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN4,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_4] = buffer[6]-'0';
				if(sys_flag.zone1_channel == INPUT_4)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);				
				EepromWriteByte(ANALOG4_MODE_POSITION, buffer[6]-'0');	
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACN5(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN5,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_5] = buffer[6]-'0';
				if(sys_flag.zone1_channel == INPUT_5)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);				
				EepromWriteByte(ANALOG5_MODE_POSITION, buffer[6]-'0');	
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACN6(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN6,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_6] = buffer[6]-'0';
				if(sys_flag.zone1_channel == INPUT_6)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);				
				EepromWriteByte(ANALOG6_MODE_POSITION, buffer[6]-'0');	
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACN7(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN7,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_7] = buffer[6]-'0';
				if(sys_flag.zone1_channel == INPUT_7)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);				
				EepromWriteByte(ANALOG7_MODE_POSITION, buffer[6]-'0');	
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACN8(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN8,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_8] = buffer[6]-'0';
				if(sys_flag.zone1_channel == INPUT_8)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);				
				EepromWriteByte(ANALOG8_MODE_POSITION, buffer[6]-'0');	
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACN7_1(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACN7_1,%d\n",size);
#endif	

	if(size==9)
		{
		switch(buffer[8])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_CH71] = buffer[8]-'0';
				if(sys_flag.zone1_channel == INPUT_CH71)
					MsgSetInputSource(ZONE1_ID,sys_flag.zone1_channel);
				EepromWriteByte(ANALOG7CH1_MODE_POSITION, buffer[8]-'0');	
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdZ1ACNR(unsigned char *buffer,unsigned char size)
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdZ1ACNR,%d\n",size);
#endif	

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
			case '1':
				AnalogInMode[INPUT_TUNER] = buffer[6]-'0';
				EepromWriteByte(ANALOG_TUNER_MODE_POSITION, buffer[6]-'0');	
				return TRUE;
			}
		}
	return FALSE;
}

/*命令只在当前Band 有效*/
BOOL ProcCmdTAT(unsigned char *buffer,unsigned char size) 
{
	float freq;  
	FREQT freq_value; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTAT,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case 'U':
				if(current_radio_mode == _AM_MODE)
					MsgSetRadioSearch(RADIO_SEEK_UP,0);
				else
					return FALSE;
				break;
			case 'D':
				if(current_radio_mode == _AM_MODE)
					MsgSetRadioSearch(RADIO_SEEK_DN,0);
				else
					return FALSE;
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}

	if(size > 4)
		{
		if(current_radio_mode == _AM_MODE)
			{
			if(GetProtocolFloatValue(buffer+3, & freq) == TRUE)
				{
				freq_value = (FREQT)freq;
				if(IsFreqValid(freq_value,_AM_MODE) == TRUE)
					{
					MsgSetRadioFreq(_AM_MODE,freq_value);
					return TRUE;
					}
				}
			}
		else
			{
			return FALSE;
			}
		}	
	return FALSE;	
}

/*命令只在当前Band 有效*/
BOOL ProcCmdTAHDT(unsigned char *buffer,unsigned char size) 
{
	float freq;  
	FREQT freq_value; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTAHDT,%d\n",size);
#endif

	if(size == 6)
		{
		switch(buffer[5])
			{
			case 'U':
				if(current_radio_mode == _HD_AM_MODE)
					MsgSetRadioSearch(RADIO_SEEK_UP,0);
				else
					return FALSE;
				break;
			case 'D':
				if(current_radio_mode == _HD_AM_MODE)
					MsgSetRadioSearch(RADIO_SEEK_DN,0);
				else
					return FALSE;
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}

	if(size>6)
		{
		if(current_radio_mode == _HD_AM_MODE)
			{
			if(GetProtocolFloatValue(buffer+5, & freq) == TRUE)
				{
				freq_value = (FREQT)freq;
				if(IsFreqValid(freq_value,_HD_AM_MODE) == TRUE)
					{
					MsgSetRadioFreq(_HD_AM_MODE,freq_value);
					return TRUE;				
					}
				}
			}
		else
			{
			return FALSE;
			}
		}
	return FALSE;
}

/*命令只在当前Band 有效*/
//
BOOL ProcCmdTFT(unsigned char *buffer,unsigned char size)
{
	float freq; 
	FREQT freq_value;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTFT,%d\n",size);
#endif

	if(size == 4)
		{
		switch(buffer[3])
			{
			case 'U':
				if(current_radio_mode == _FM_MODE)
					MsgSetRadioSearch(RADIO_SEEK_UP,0);
				else
					return FALSE;
				break;
			case 'D':
				if(current_radio_mode == _FM_MODE)
					MsgSetRadioSearch(RADIO_SEEK_DN,0);
				else
					return FALSE;
				break;
			default:
				return FALSE;
			}
		return TRUE;
		}

	if(size > 4)
		{
		if(current_radio_mode == _FM_MODE)
			{
			if(GetProtocolFloatValue(buffer+3, & freq) == TRUE)
				{
				freq_value = freq * 1000;
				if(IsFreqValid(freq_value,_FM_MODE) == TRUE)
					{
					MsgSetRadioFreq(_FM_MODE,freq_value);
					return TRUE;
					}
				}
			}
		else
			{
			return FALSE;
			}
		}	
	return FALSE;	
}

/*命令只在当前Band 有效*/
BOOL ProcCmdTFHDT(unsigned char *buffer,unsigned char size)
{
	float freq; 
	FREQT freq_value;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTFHDT,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case 'U':
				if(current_radio_mode == _HD_FM_MODE)
					MsgSetRadioSearch(RADIO_SEEK_UP,0);
				else
					return FALSE;
				break;
			case 'D':
				if(current_radio_mode == _HD_FM_MODE)
					MsgSetRadioSearch(RADIO_SEEK_DN,0);
				else
					return FALSE;
				break;
			default:
				return FALSE;
			}
		return TRUE;		
		}

	if(size>6)
		{
		if(current_radio_mode == _HD_FM_MODE)
			{
			if(GetProtocolFloatValue(buffer+5, & freq) == TRUE)
				{
				freq_value = freq * 1000;
				if(IsFreqValid(freq_value,_HD_FM_MODE) == TRUE)
					{
					MsgSetRadioFreq(_HD_FM_MODE,freq_value);
					return TRUE;
					}
				}
			}
		else
			{
			return FALSE; 
			}
		}
	return FALSE;	
}

/*命令只在当前Band 有效*/
BOOL ProcCmdTAP(unsigned char *buffer,unsigned char size) 
{
	float freq; 
	unsigned char position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTAP,%d\n",size);
#endif

	if(size >= 4)
		{
		if(GetProtocolFloatValue(buffer+3, & freq) == TRUE)
			{
			position = (unsigned char) freq;
			if(position > 100)
				return FALSE;
			//if(GetCurFocusTid() == RADIO_ID) //Dele by cjm 2009-03-30
			//	{
				if(current_radio_mode == _AM_MODE)
					{
					MsgSetRadioLoadPreset(position);
					MsgSetVfdShow(RADIO_ID,0);
					return TRUE;
					}
				else
					{
					return FALSE;
					}
			//	}
			}
		}
	return FALSE;	
}

/*命令只在当前Band 有效*/
BOOL ProcCmdTAHDP(unsigned char *buffer,unsigned char size) 
{
	float freq; 
	unsigned char position;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTAHDP,%d\n",size);
#endif

	if(size>=6)
		{
		if(GetProtocolFloatValue(buffer+5, & freq) == TRUE)
			{
			position = (unsigned char) freq;
			if(position > 100)
				return FALSE;
			//if(GetCurFocusTid() == RADIO_ID) //Dele by cjm 2009-03-30
			//	{
				if(current_radio_mode == _HD_AM_MODE)
					{
					MsgSetRadioLoadPreset(position);
					MsgSetVfdShow(RADIO_ID,0);
					return TRUE;					
					}
				else
					{
					return FALSE;
					}					
			//	}
			}
		}
	return FALSE;
}

/*命令只在当前Band 有效*/
BOOL ProcCmdTFP(unsigned char *buffer,unsigned char size) 
 {
	float freq; 
	unsigned char position; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTFP,%d\n",size);
#endif

	if(size >= 4)
		{
		if(GetProtocolFloatValue(buffer+3, & freq) == TRUE)
			{
			position = (unsigned char) freq;
			if(position > 100)
				return FALSE;
			//if(GetCurFocusTid() == RADIO_ID) //Dele by cjm 2009-03-30
			//	{
				if(current_radio_mode == _FM_MODE)
					{
					MsgSetRadioLoadPreset(position);
					MsgSetVfdShow(RADIO_ID,0);
					return TRUE;
					}
				else
					{
					return FALSE;
					}
			//	}
			}
		}
	return FALSE;
}

/*命令只在当前Band 有效*/
BOOL ProcCmdTFHDP(unsigned char *buffer,unsigned char size) 
{
	float freq; 
	unsigned char position; 
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTFHDP,%d\n",size);
#endif

	if(size>=6)
		{
		if(GetProtocolFloatValue(buffer+5, & freq) == TRUE)
			{
			position = (unsigned char) freq;
			if(position > 100)
				return FALSE;
			//if(GetCurFocusTid() == RADIO_ID) //Dele by cjm 2009-03-30
			//	{
				if(current_radio_mode == _HD_FM_MODE)
					{
					MsgSetRadioLoadPreset(position);
					MsgSetVfdShow(RADIO_ID,0);
					return TRUE;					
					}
				else
					{
					return FALSE;
					}
			//	}
			}
		}
	return FALSE;
}

BOOL ProcCmdT(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdT,%d\n",size);
#endif

	if(size == 2)
		{
		if(buffer[1] == '+') // T+ 
			{
			MsgSetRadioSearch(RADIO_SEARCH_UP,1);
			return TRUE;
			}
		else if(buffer[1] == '-') // T- 
			{
			MsgSetRadioSearch(RADIO_SEARCH_DN,1);
			return TRUE;
			}
		}	
	return FALSE;	
}

unsigned char GetTunerPosition(unsigned char *buffer)
{
	unsigned char Postion;

	Postion=(buffer[0]-'0')*10+(buffer[1]-'0');

#ifdef _DEBUG_RS232
	rs232_printf("Postion=%d\n",Postion);
#endif	

	return Postion;
}

BOOL ProcCmdTAS(unsigned char *buffer,unsigned char size) 
{
	float freq;  
	FREQT freq_value; 
	unsigned int temp_freq;	
	unsigned char Postion;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTAS,%d\n",size);
#endif

	if(buffer[3]=='?')
		{
		if(GetProtocolFloatValue(buffer+4, & freq) == TRUE)
			{
			Postion = (unsigned char) freq;
			if(Postion > 100)
				return FALSE;
			
			LoadRadioPreset(Postion,&temp_freq,_AM_MODE);
			rs232_printf("TASy=%4dKHz",temp_freq);
			return TRUE;
			}
		return FALSE;
		}

	if(size>7)
		{
		if(buffer[5]!='=') /*不符合命令规则*/
			return FALSE;

		if(GetProtocolFloatValue(buffer+6, & freq) == TRUE)
			{
			freq_value = (FREQT)freq;
			if(IsFreqValid(freq_value,_AM_MODE) == TRUE)
				{
				Postion=GetTunerPosition(buffer+3);
				if(Postion>=100)
					return FALSE;	
				SaveFmPreset(Postion,freq_value,_AM_MODE);
				return TRUE;
				}
			else
				{
				return FALSE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdTAHDS(unsigned char *buffer,unsigned char size) 
{
	float freq;  
	FREQT freq_value; 
	unsigned int temp_freq;	
	unsigned char Postion;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTAHDS,%d\n",size);
#endif	

	if(buffer[5]=='?')
		{
		if(GetProtocolFloatValue(buffer+6, & freq) == TRUE)
			{
			Postion = (unsigned char) freq;
			if(Postion > 100)
				return FALSE;

			LoadRadioPreset(Postion,&temp_freq,_HD_AM_MODE);
			rs232_printf("TAHDSy=%4dKHz",temp_freq);
			return TRUE;
			}
		return FALSE;
		}

	if(size>9)
		{
		if(buffer[7]!='=') /*不符合命令*/
			return FALSE;

		if(GetProtocolFloatValue(buffer+8, & freq) == TRUE)
			{
			freq_value = (FREQT)freq;
			if(IsFreqValid(freq_value,_HD_AM_MODE) == TRUE)
				{
				Postion=GetTunerPosition(buffer+5);
				if(Postion>=100)
					return FALSE;	
				SaveFmPreset(Postion,freq_value,_HD_AM_MODE);
				return TRUE;
				}
			else
				{
				return FALSE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdTAF(unsigned char *buffer,unsigned char size) 
{
	float freq;  
	FREQT freq_value; 
	unsigned int temp_freq;	
	unsigned char Postion;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTAF,%d\n",size);
#endif	

	if(buffer[3]=='?')
		{
		if(GetProtocolFloatValue(buffer+4, & freq) == TRUE)
			{
			Postion = (unsigned char) freq;
			if(Postion > 100)
				return FALSE;
			
			LoadRadioPreset(Postion,&temp_freq,_FM_MODE);
			rs232_printf("TAFy=%3d.%01dMHz",((temp_freq/100) / 10),((temp_freq/100) % 10));
			return TRUE;
			}
		return FALSE;
		}

	if(size>7)
		{
		if(buffer[5]!='=') /*不符合命令规则*/
			return FALSE;
		
		if(GetProtocolFloatValue(buffer+6, & freq) == TRUE)
			{
			freq_value = freq * 1000;
			if(IsFreqValid(freq_value,_FM_MODE) == TRUE)
				{
				Postion=GetTunerPosition(buffer+3);
				if(Postion>=100)
					return FALSE;	
				SaveFmPreset(Postion,freq_value,_FM_MODE);
				return TRUE;
				}
			else
				{
				return FALSE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdTAHDF(unsigned char *buffer,unsigned char size) 
{
	float freq;  
	FREQT freq_value; 
	unsigned int temp_freq;	
	unsigned char Postion;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTAHDF,%d\n",size);
#endif	

	if(buffer[5]=='?')
		{
		if(GetProtocolFloatValue(buffer+6, & freq) == TRUE)
			{
			Postion = (unsigned char) freq;
			if(Postion > 100)
				return FALSE;

			LoadRadioPreset(Postion,&temp_freq,_HD_FM_MODE);
			rs232_printf("TAHDFy=%3d.%01dMHz",((temp_freq/100) / 10),((temp_freq/100) % 10));
			return TRUE;
			}
		}

	if(size>9)
		{
		if(buffer[7]!='=') /*不符合命令规则*/
			return FALSE;

		if(GetProtocolFloatValue(buffer+8, & freq) == TRUE)
			{
			freq_value = freq * 1000;
			if(IsFreqValid(freq_value,_HD_FM_MODE) == TRUE)
				{
				Postion=GetTunerPosition(buffer+5);
				if(Postion>=100)
					return FALSE;	
				SaveFmPreset(Postion,freq_value,_HD_FM_MODE);
				return TRUE;
				}
			else
				{
				return FALSE;
				}
			}
		}
	return FALSE;
} 

BOOL ProcCmdTT_(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdTT?,%d\n",size);
#endif

	if(size == 3)
		{
		switch(current_radio_mode)
			{
			case _AM_MODE:
				rs232_printf("@TAT %d\n",system_freq);
				return TRUE;
			case _FM_MODE:
				rs232_printf("@TFT %d.%1d\n",(system_freq / 1000),(system_freq % 1000) / 100);
				return TRUE;
			case  _HD_AM_MODE:
				rs232_printf("@THDAT %d\n",system_freq);
				return TRUE;
			case _HD_FM_MODE:
				rs232_printf("@THDFT %d.%1d\n",(system_freq / 1000),(system_freq % 1000) / 100);
				return TRUE;
			default :
				return FALSE;
			}
		}	
	return FALSE;
}

BOOL ProcCmdASSP(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASSP,%d\n",size);
#endif

	if(size==5)
		{
		switch(buffer[4])
			{
			case '0':
				pwd_mode = 0;
				EepromWriteByte(PASSWORD_MODE_POSITION, 0);
				return TRUE;
			case '1':
				return FALSE;
			}
		}
	return FALSE;
}

extern void SetHdmiAudioOut(uchar mode);
BOOL ProcCmdASHDMIAO(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASHDMIAO,%d\n",size);
#endif

	if(size==9)
		{
		switch(buffer[8])
			{
			case '0':
				HDMI_Audio_Out=0;
				SetHdmiAudioOut(0);
				EepromWriteByte(HDMI_AUDIO_OUT , 0);
				return TRUE;
			case '1':
				HDMI_Audio_Out=1;
				SetHdmiAudioOut(1);
				EepromWriteByte(HDMI_AUDIO_OUT , 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASAVD(unsigned char *buffer,unsigned char size) 
{
	float position;
	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASAVD,%d\n",size);
#endif

	if(size>5)
		{
		if(GetProtocolFloatValue(buffer+5, & position) == TRUE)
			{
			if(IsDistanceValid(position,cur_dis_unit) == TRUE)
				{
				DSP_AV_Delay((unsigned char)position);
				EepromWriteByte(AV_SYNC_POSITION, (unsigned char)position);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL ProcCmdASFP(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASFP,%d\n",size);
#endif

	if(size==5)
		{
		switch(buffer[4])
			{
			case '0':
				sys_flag.system_dimer = 0;
				MsgSetDimer(0,MODE_FIXED);
				EepromWriteByte(DIMER_POSITION, 0);
				return TRUE;
			case '1':
				sys_flag.system_dimer = 1;
				MsgSetDimer(1,MODE_FIXED);
				EepromWriteByte(DIMER_POSITION, 1);
				return TRUE;
			case '2':
				sys_flag.system_dimer = 2;
				MsgSetDimer(2,MODE_FIXED);
				EepromWriteByte(DIMER_POSITION, 2);
				return TRUE;
			case '3':
				sys_flag.system_dimer = 3;
				MsgSetDimer(3,MODE_FIXED);
				EepromWriteByte(DIMER_POSITION, 3);
				return TRUE;
			}
		}
	return FALSE;
}

extern char C11Logo[PRODUCT_NAME_LENGTH+1];
extern void ReadName(unsigned char addr,unsigned char len,char *src);
BOOL ProcCmdASRSN(unsigned char *buffer,unsigned char size) 
{	
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASRSN,%d\n",size);
#endif

	if(size>5)
		{
		if(IsProtocolStringValid(buffer+5,PRODUCT_NAME_LENGTH) == TRUE)
			{
			WriteName(PRODUCT_NAME_POSITION,PRODUCT_NAME_LENGTH,buffer+5);
			ReadName(PRODUCT_NAME_POSITION, PRODUCT_NAME_LENGTH, C11Logo); //把NAME 存到C11LOGO
			return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASIRC(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASIRC,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				Zone1SelectIr(0);
				EepromWriteByte(IR_CONTROL_POSITION, 0);
				return TRUE;
			case '1':
				Zone1SelectIr(1);
				EepromWriteByte(IR_CONTROL_POSITION, 1);
				return TRUE;
			case '2':
				Zone1SelectIr(2);
				EepromWriteByte(IR_CONTROL_POSITION, 2);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASIR2C(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASIR2C,%d\n",size);
#endif

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
				Zone2SelectIr(0);
				EepromWriteByte(ZONE2_IR_CONTROL_POSITION, 0);
				return TRUE;
			case '1':
				Zone2SelectIr(1);
				EepromWriteByte(ZONE2_IR_CONTROL_POSITION, 1);				
				return TRUE;
			case '2':
				Zone2SelectIr(2);
				EepromWriteByte(ZONE2_IR_CONTROL_POSITION, 2);				
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASIRM(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASIRM,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				rear_ir_mode = 0;
				EepromWriteByte(REAR_IR_MODE_POSITION, 0);
				return TRUE;
			case '1':
				rear_ir_mode = 1;
				EepromWriteByte(REAR_IR_MODE_POSITION, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASIR2M(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASIR2M,%d\n",size);
#endif

	if(size==7)
		{
		switch(buffer[6])
			{
			case '0':
				zone2_ir_mode = 0;
				EepromWriteByte(ZONE2_IR_MODE_POSITION, 0);
				return TRUE;
			case '1':
				zone2_ir_mode = 1;
				EepromWriteByte(ZONE2_IR_MODE_POSITION, 1);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASTO1(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASTO1,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				SystemTriggerMode = 0;
				EepromWriteByte(TRIGGER_POSITION, 0);
				return TRUE;
			case '1':
				SystemTriggerMode = 1;
				EepromWriteByte(TRIGGER_POSITION, 1);
				return TRUE;
			case '2':
				SystemTriggerMode = 2;
				EepromWriteByte(TRIGGER_POSITION, 2);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASTO2(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASTO2,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				SystemTriggerMode1 = 0;
				EepromWriteByte(TRIGGER_POSITION1, 0);
				return TRUE;
			case '1':
				SystemTriggerMode1 = 1;
				EepromWriteByte(TRIGGER_POSITION1, 1);
				return TRUE;
			case '2':
				SystemTriggerMode1 = 2;
				EepromWriteByte(TRIGGER_POSITION1, 2);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASTO3(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASTO3,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				SystemTriggerMode2 = 0;
				EepromWriteByte(TRIGGER_POSITION2, 0);
				return TRUE;
			case '1':
				SystemTriggerMode2 = 1;
				EepromWriteByte(TRIGGER_POSITION2, 1);
				return TRUE;
			case '2':
				SystemTriggerMode2 = 2;
				EepromWriteByte(TRIGGER_POSITION2, 2);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASAIS(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASAIS,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case'0':
				EepromWriteByte(AUTO_DETECT_POSITION, 1);
				return TRUE;
			case '1':
				EepromWriteByte(AUTO_DETECT_POSITION, 0); 
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASDD(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASDD,%d\n",size);
#endif

	if(size==5)
		{
		switch(buffer[4])
			{
			case '0':
				SetLate(DolbyType,0);
				EepromWriteByte(LATE_POSITION, 0);
				return TRUE;
			case '1':
				SetLate(DolbyType,1);
				EepromWriteByte(LATE_POSITION, 1);
				return TRUE;
			case '2':
				SetLate(DolbyType,2);
				EepromWriteByte(LATE_POSITION, 2);
				return TRUE;
			}
		}
	return FALSE;
}

BOOL ProcCmdASRD(unsigned char *buffer,unsigned char size) 
{
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASRD,%d\n",size);
#endif

	if(size==5)
		{
		switch(buffer[4])
			{
			case '0':
				return TRUE;
			case '1':
				key_enable = 0;
				DacMute(1);
				SetupLoadDefault(1);
				DelayMs(500);
				rs232_printf("\nRestore Default OK\n");
				DacMute(0);
				key_enable = 1;
				return TRUE;
			}
		}
	return FALSE;
}


BOOL ProcCmdASPAO(unsigned char *buffer,unsigned char size) 
{

extern unsigned char CD_Pure_Audio;
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASPAO,%d\n",size);
#endif

	if(size==6)
		{
		switch(buffer[5])
			{
			case '0':
				CD_Pure_Audio = 0;
				return TRUE;
			case '1':
				CD_Pure_Audio = 1;
				return TRUE;
			}
		}
	return FALSE;
}


/*JAIN ADD IN 10-11-18*/
BOOL IsVolumeValid(float position)
{
	if((position >= -90.0) && (position <= 15.0))
		return TRUE;

	return FALSE;
}


BOOL IsMAXVolumeValid(float position)
{
	if((position >= 0.0) && (position <= 15.0))
		return TRUE;

	return FALSE;
}


BOOL ProcCmdASMVO(unsigned char *buffer,unsigned char size) 
{

float volume;  
#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdASMVO,%d\n",size);
#endif

	if(size>5)
		{
		if(GetProtocolFloatValue(buffer+5, &volume) == TRUE)
			{
			if(IsMAXVolumeValid(volume) == TRUE)
				{
				volume = volume + 90; 
				MaxVolumeLevel((uchar)volume);
				EepromWriteByte(Set_Max_Volume_Level,volume);   
				return TRUE;
				}
			}
		}
	return FALSE;
}

  uchar GetMasterVolumePoint(float master_volume)
{
	float temp_value = master_volume;

	temp_value = temp_value+90;

	return (uchar)temp_value;
}


BOOL ProcCmdAZ1SMV(unsigned char *buffer,unsigned char size) 
{

float volume;  
short max_vol_Eeprom;
short max_vol_value;
short temp_volume;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdAZ1SMV,%d\n",size);
#endif

	if(size>6)
		{
		if(GetProtocolFloatValue(buffer+6, &volume) == TRUE)
			{
			if(IsVolumeValid(volume) == TRUE)
				{
				SetMasterVolumeLevel(GetMasterVolumePoint( volume));
				max_vol_Eeprom=EepromReadByte(Set_Max_Volume_Level); 
				max_vol_value=volume_table[max_vol_Eeprom];
				temp_volume=volume_table[GetMasterVolumePoint( volume)];
				if(temp_volume<=max_vol_value)
				{
					EepromWriteByte(Master_Volume_Level,GetMasterVolumePoint( volume));
				}
				return TRUE;
				}
			}
		}
	return FALSE;
}


BOOL ProcCmdAZ2SMV(unsigned char *buffer,unsigned char size) 
{

float volume;  
short max_vol_Eeprom;
short max_vol_value;
short temp_volume;

#ifdef _DEBUG_RS232
	rs232_printf("ProcCmdAZ2SMV,%d\n",size);
#endif

	if(size>6)
		{
		if(GetProtocolFloatValue(buffer+6, &volume) == TRUE)
			{
			if(IsVolumeValid(volume) == TRUE)
				{
				SetZ2MasterVolumeLevel(GetMasterVolumePoint( volume)); 
				EepromWriteByte(Z2_Master_Volume_Level,GetMasterVolumePoint( volume));   
				return TRUE;
				}
			}
		}
	return FALSE;
}

const MSG_T cmd_SPU = {"SPU",ProcCmdSPU};
const MSG_T cmd_RSN = {"RSN",ProcCmdRSN};

const MSG_T cmd_TAT = {"TAT",ProcCmdTAT};
const MSG_T cmd_TAHDT={"TAHDT",ProcCmdTAHDT};
const MSG_T cmd_TFT = {"TFT",ProcCmdTFT};
const MSG_T cmd_TFHDT={"TFHDT",ProcCmdTFHDT};
const MSG_T cmd_TAP = {"TAP",ProcCmdTAP};
const MSG_T cmd_TAHDP={"TAHDP",ProcCmdTAHDP};
const MSG_T cmd_TFP = {"TFP",ProcCmdTFP};
const MSG_T cmd_TFHDP={"TFHDP",ProcCmdTFHDP};
const MSG_T cmd_T = {"T",ProcCmdT};
const MSG_T cmd_TAS = {"TAS",ProcCmdTAS};
const MSG_T cmd_TAHDS={"TAHDS",ProcCmdTAHDS};
const MSG_T cmd_TAF={"TAF",ProcCmdTAF};
const MSG_T cmd_TAHDF={"TAHDF",ProcCmdTAHDF};
const MSG_T cmd_TT_ = {"TT?",ProcCmdTT_};

const MSG_T cmd_Z1LPS={"Z1LPS",ProcCmdZ1LPS};
const MSG_T cmd_Z1S = {"Z1S",ProcCmdZ1S};
const MSG_T cmd_Z1P = {"Z1P",ProcCmdZ1P};
const MSG_T cmd_Z1M = {"Z1M",ProcCmdZ1M};
const MSG_T cmd_Z1E = {"Z1E",ProcCmdZ1E};
const MSG_T cmd_Z1EX = {"Z1EX",ProcCmdZ1EX};
const MSG_T cmd_Z1EF = {"Z1EF",ProcCmdZ1EF};
const MSG_T cmd_Z1EE={"Z1EE",ProcCmdZ1EE}; 
const MSG_T cmd_Z1ES={"Z1ES",ProcCmdZ1ES}; 
const MSG_T cmd_Z1EU={"Z1EU",ProcCmdZ1EU};
const MSG_T cmd_Z1ED = {"Z1ED",ProcCmdZ1ED};
const MSG_T cmd_Z1D = {"Z1D",ProcCmdZ1D};
const MSG_T cmd_Z1C = {"Z1C",ProcCmdZ1C};
const MSG_T cmd_Z1A_ = {"Z1A?",ProcCmdZ1A_};
const MSG_T cmd_Z1_ = {"Z1?",ProcCmdZ1_};
const MSG_T cmd_Z1VM = {"Z1VM",ProcCmdZ1VM};
const MSG_T cmd_Z1DF_={"Z1DF?",ProcCmdZ1DF_};

const MSG_T cmd_Z1EQL = {"Z1EQL",ProcCmdZ1EQL};
const MSG_T cmd_Z1EQC = {"Z1EQC",ProcCmdZ1EQC};
const MSG_T cmd_Z1EQR = {"Z1EQR",ProcCmdZ1EQR};
const MSG_T cmd_Z1EQSR = {"Z1EQSR",ProcCmdZ1EQSR};
const MSG_T cmd_Z1EQBSR = {"Z1EQBSR",ProcCmdZ1EQBSR};
const MSG_T cmd_Z1EQBSL = {"Z1EQBSL",ProcCmdZ1EQBSL};
const MSG_T cmd_Z1EQSL = {"Z1EQSL",ProcCmdZ1EQSL};
const MSG_T cmd_Z1EQSW = {"Z1EQSW",ProcCmdZ1EQSW};
const MSG_T cmd_Z1EQ = {"Z1EQ",ProcCmdZ1EQ};

const MSG_T cmd_Z2P = {"Z2P",ProcCmdZ2P};
const MSG_T cmd_Z2S = {"Z2S",ProcCmdZ2S};
const MSG_T cmd_Z2V = {"Z2V",ProcCmdZ2V};
const MSG_T cmd_Z2M = {"Z2M",ProcCmdZ2M};
const MSG_T cmd_Z2_ = {"Z2?",ProcCmdZ2_};
const MSG_T cmd_Z2VFD_={"Z2VFD?",ProCmdZ2VFD_};

const MSG_T cmd_SLL = {"SLL",ProcCmdSLL};
const MSG_T cmd_SLC = {"SLC",ProcCmdSLC};
const MSG_T cmd_SLR = {"SLR",ProcCmdSLR};
const MSG_T cmd_SLSR = {"SLSR",ProcCmdSLSR};
const MSG_T cmd_SLSL = {"SLSL",ProcCmdSLSL};
const MSG_T cmd_SLBR = {"SLBR",ProcCmdSLBR};
const MSG_T cmd_SLBL = {"SLBL",ProcCmdSLBL};
const MSG_T cmd_SLSW = {"SLSW",ProcCmdSLSW};

const MSG_T cmd_SPL = {"SPL",ProcCmdSPL};
const MSG_T cmd_SPC = {"SPC",ProcCmdSPC};
const MSG_T cmd_SPR = {"SPR",ProcCmdSPR};
const MSG_T cmd_SPSR = {"SPSR",ProcCmdSPSR};
const MSG_T cmd_SPSL = {"SPSL",ProcCmdSPSL};
const MSG_T cmd_SPBR = {"SPBR",ProcCmdSPBR};
const MSG_T cmd_SPBL = {"SPBL",ProcCmdSPBL};
const MSG_T cmd_SPs = {"SPs",ProcCmdSPs};

const MSG_T cmd_SZL = {"SZL",ProcCmdSZL};
const MSG_T cmd_SZR = {"SZR",ProcCmdSZR};
const MSG_T cmd_SZSL = {"SZSL",ProcCmdSZSL};
const MSG_T cmd_SZSR = {"SZSR",ProcCmdSZSR};
const MSG_T cmd_SZSBL = {"SZSBL",ProcCmdSZSBL};
const MSG_T cmd_SZSBR = {"SZSBR",ProcCmdSZSBR};
const MSG_T cmd_SZC = {"SZC",ProcCmdSZC};
const MSG_T cmd_SZS = {"SZS",ProcCmdSZS};

const MSG_T cmd_SZXFL = {"SZXFL",ProcCmdSZXFL};
const MSG_T cmd_SZXFC = {"SZXFC",ProcCmdSZXFC};
const MSG_T cmd_SZXFR = {"SZXFR",ProcCmdSZXFR};
const MSG_T cmd_SZXSR = {"SZXSR",ProcCmdSZXSR};
const MSG_T cmd_SZXSL = {"SZXSL",ProcCmdSZXSL};
const MSG_T cmd_SZXSBR = {"SZXSBR",ProcCmdSZXSBR};
const MSG_T cmd_SZXSBL = {"SZXSBL",ProcCmdSZXSBL};
const MSG_T cmd_SZX = {"SZX",ProcCmdSZX};

const MSG_T cmd_SSPL={"SSPL",ProcCmdSSPL};
const MSG_T cmd_SSPC={"SSPC",ProcCmdSSPC};
const MSG_T cmd_SSPR={"SSPR",ProcCmdSSPR};
const MSG_T cmd_SSPSR={"SSPSR",ProcCmdSSPSR};
const MSG_T cmd_SSPSL={"SSPSL",ProcCmdSSPSL};
const MSG_T cmd_SSPSBR={"SSPSBR",ProcCmdSSPSBR};
const MSG_T cmd_SSPSBL={"SSPSBL",ProcCmdSSPSBL};
const MSG_T cmd_SSPSW={"SSPSW",ProcCmdSSPSW};

const MSG_T cmd_MSZL={"MSZL",ProcCmdMSZL};
const MSG_T cmd_MSZR={"MSZR",ProcCmdMSZR};
const MSG_T cmd_MSZSL={"MSZSL",ProcCmdMSZSL};
const MSG_T cmd_MSZSR={"MSZSR",ProcCmdMSZSR};
const MSG_T cmd_MSZSBL={"MSZSBL",ProcCmdMSZSBL};
const MSG_T cmd_MSZSBR={"MSZSBR",ProcCmdMSZSBR};
const MSG_T cmd_MSZC={"MSZC",ProcCmdMSZC};
const MSG_T cmd_MSZS={"MSZS",ProcCmdMSZS};

const MSG_T cmd_MSPL={"MSPL",ProcCmdMSPL};
const MSG_T cmd_MSPC={"MSPC",ProcCmdMSPC};
const MSG_T cmd_MSPR={"MSPR",ProcCmdMSPR};
const MSG_T cmd_MSPSR={"MSPSR",ProcCmdMSPSR};
const MSG_T cmd_MSPSL={"MSPSL",ProcCmdMSPSL};
const MSG_T cmd_MSPBR={"MSPBR",ProcCmdMSPBR};
const MSG_T cmd_MSPBL={"MSPBL",ProcCmdMSPBL};
const MSG_T cmd_MSPs={"MSPs",ProcCmdMSPs};

const MSG_T cmd_MSLL={"MSLL",ProcCmdMSLL};
const MSG_T cmd_MSLC={"MSLC",ProcCmdMSLC};
const MSG_T cmd_MSLR={"MSLR",ProcCmdMSLR};
const MSG_T cmd_MSLSR={"MSLSR",ProcCmdMSLSR};
const MSG_T cmd_MSLSL={"MSLSL",ProcCmdMSLSL};
const MSG_T cmd_MSLBR={"MSLBR",ProcCmdMSLBR};
const MSG_T cmd_MSLBL={"MSLBL",ProcCmdMSLBL};
const MSG_T cmd_MSLSW={"MSLSW",ProcCmdMSLSW};

const MSG_T cmd_MSZXFL={"MSZXFL",ProcCmdMSZXFL};
const MSG_T cmd_MSZXFC={"MSZXFC",ProcCmdMSZXFC};
const MSG_T cmd_MSZXFR={"MSZXFR",ProcCmdMSZXFR};
const MSG_T cmd_MSZXSR={"MSZXSR",ProcCmdMSZXSR};
const MSG_T cmd_MSZXSBR={"MSZXSBR",ProcCmdMSZXSBR};
const MSG_T cmd_MSZXSBL={"MSZXSBL",ProcCmdMSZXSBL};
const MSG_T cmd_MSZXSL={"MSZXSL",ProcCmdMSZXSL};
const MSG_T cmd_MSZX={"MSZX",ProcCmdMSZX};

const MSG_T cmd_MZ1EQL={"MZ1EQL",ProcCmdMZ1EQL};  
const MSG_T cmd_MZ1EQC={"MZ1EQC",ProcCmdMZ1EQC};
const MSG_T cmd_MZ1EQR={"MZ1EQR",ProcCmdMZ1EQR};
const MSG_T cmd_MZ1EQSR={"MZ1EQSR",ProcCmdMZ1EQSR};
const MSG_T cmd_MZ1EQBSR={"MZ1EQBSR",ProcCmdMZ1EQBSR};
const MSG_T cmd_MZ1EQBSL={"MZ1EQBSL",ProcCmdMZ1EQBSL};
const MSG_T cmd_MZ1EQSL={"MZ1EQSL",ProcCmdMZ1EQSL};
const MSG_T cmd_MZ1EQSW={"MZ1EQSW",ProcCmdMZ1EQSW};

const MSG_T cmd_MSSPL={"MSSPL",ProcCmdMSSPL};
const MSG_T cmd_MSSPC={"MSSPC",ProcCmdMSSPC};
const MSG_T cmd_MSSPR={"MSSPR",ProcCmdMSSPR};
const MSG_T cmd_MSSPSR={"MSSPSR",ProcCmdMSSPSR};
const MSG_T cmd_MSSPSL={"MSSPSL",ProcCmdMSSPSL};
const MSG_T cmd_MSSPSBR={"MSSPSBR",ProcCmdMSSPSBR};
const MSG_T cmd_MSSPSBL={"MSSPSBL",ProcCmdMSSPSBL};
const MSG_T cmd_MSSPSW={"MSSPSW",ProcCmdMSSPSW};

const MSG_T cmd_Z1EMDIM={"Z1EMDIM",ProcCmdZ1EMDIM};
const MSG_T cmd_Z1EMP={"Z1EMP",ProcCmdZ1EMP};
const MSG_T cmd_Z1EMC={"Z1EMC",ProcCmdZ1EMC};
const MSG_T cmd_Z1EMDEL={"Z1EMDEL",ProcCmdZ1EMDEL};

const MSG_T cmd_Z1VFL_={"Z1VFL?",ProcCmdZ1VFL_};
const MSG_T cmd_Z1VFR_={"Z1VFR?",ProcCmdZ1VFR_};
const MSG_T cmd_Z1VC_={"Z1VC?",ProcCmdZ1VC_};
const MSG_T cmd_Z1VSL_={"Z1VSL?",ProcCmdZ1VSL_};
const MSG_T cmd_Z1VSR_={"Z1VSR?",ProcCmdZ1VSR_};
const MSG_T cmd_Z1VBL_={"Z1VBL?",ProcCmdZ1VBL_};
const MSG_T cmd_Z1VBR_={"Z1VBR?",ProcCmdZ1VBR_};
const MSG_T cmd_Z1VS_={"Z1VS?",ProcCmdZ1VS_};
const MSG_T cmd_Z1VFD_={"Z1VFD?",ProCmdZ1VFD_};

const MSG_T cmd_Z1IN1={"Z1IN1",ProcCmdZ1IN1};
const MSG_T cmd_Z1IN2={"Z1IN2",ProcCmdZ1IN2};
const MSG_T cmd_Z1IN3={"Z1IN3",ProcCmdZ1IN3};
const MSG_T cmd_Z1IN4={"Z1IN4",ProcCmdZ1IN4};
const MSG_T cmd_Z1IN5={"Z1IN5",ProcCmdZ1IN5};
const MSG_T cmd_Z1IN6={"Z1IN6",ProcCmdZ1IN6};
const MSG_T cmd_Z1IN7={"Z1IN7",ProcCmdZ1IN7};
const MSG_T cmd_Z1IN8={"Z1IN8",ProcCmdZ1IN8};

const MSG_T cmd_Z1SAL1={"Z1SAL1",ProcCmdZ1SAL1};
const MSG_T cmd_Z1SAL2={"Z1SAL2",ProcCmdZ1SAL2};
const MSG_T cmd_Z1SAL3={"Z1SAL3",ProcCmdZ1SAL3};
const MSG_T cmd_Z1SAL4={"Z1SAL4",ProcCmdZ1SAL4};
const MSG_T cmd_Z1SAL5={"Z1SAL5",ProcCmdZ1SAL5};
const MSG_T cmd_Z1SAL6={"Z1SAL6",ProcCmdZ1SAL6};
const MSG_T cmd_Z1SAL7={"Z1SAL7",ProcCmdZ1SAL7};
const MSG_T cmd_Z1SAL8={"Z1SAL8",ProcCmdZ1SAL8};

const MSG_T cmd_Z1ACN1={"Z1ACN1",ProcCmdZ1ACN1};
const MSG_T cmd_Z1ACN2={"Z1ACN2",ProcCmdZ1ACN2};
const MSG_T cmd_Z1ACN3={"Z1ACN3",ProcCmdZ1ACN3};
const MSG_T cmd_Z1ACN4={"Z1ACN4",ProcCmdZ1ACN4};
const MSG_T cmd_Z1ACN5={"Z1ACN5",ProcCmdZ1ACN5};
const MSG_T cmd_Z1ACN6={"Z1ACN6",ProcCmdZ1ACN6};
const MSG_T cmd_Z1ACN7={"Z1ACN7",ProcCmdZ1ACN7};
const MSG_T cmd_Z1ACN8={"Z1ACN8",ProcCmdZ1ACN8};
const MSG_T cmd_Z1ACN7_1={"Z1ACN7.1",ProcCmdZ1ACN7_1};
const MSG_T cmd_Z1ACNR={"Z1ACNR",ProcCmdZ1ACNR};

const MSG_T cmd_ASSP={"ASSP",ProcCmdASSP};
const MSG_T cmd_ASHDMIAO={"ASHDMIAO",ProcCmdASHDMIAO};
const MSG_T cmd_ASAVD={"ASAVD",ProcCmdASAVD};
const MSG_T cmd_ASFP={"ASFP",ProcCmdASFP};
const MSG_T cmd_ASRSN={"ASRSN",ProcCmdASRSN};
const MSG_T cmd_ASIRC={"ASIRC",ProcCmdASIRC};
const MSG_T cmd_ASIR2C={"ASIR2C",ProcCmdASIR2C};
const MSG_T cmd_ASIRM={"ASIRM",ProcCmdASIRM};
const MSG_T cmd_ASIR2M={"ASIR2M",ProcCmdASIR2M};
const MSG_T cmd_ASTO1={"ASTO1",ProcCmdASTO1};
const MSG_T cmd_ASTO2={"ASTO2",ProcCmdASTO2};
const MSG_T cmd_ASTO3={"ASTO3",ProcCmdASTO3};

const MSG_T cmd_ASAIS={"ASAIS",ProcCmdASAIS};
const MSG_T cmd_ASDD={"ASDD",ProcCmdASDD};
const MSG_T cmd_ASRD={"ASRD",ProcCmdASRD};


const MSG_T cmd_ASPAO={"ASPAO",ProcCmdASPAO};
const MSG_T cmd_ASMVO={"ASMVO",ProcCmdASMVO};
const MSG_T cmd_AZ1SMV={"AZ1SMV",ProcCmdAZ1SMV};
const MSG_T cmd_AZ2SMV={"AZ2SMV",ProcCmdAZ2SMV};

/*
 由于比较的时候是从数组的第一个元素开始比较
 因此比较字符有重复的情况下，字符多的必须排在数组前
 如包含Z1,Z1EQ,Z1E 时，必须安装此顺序排列 Z1EQ,Z1E,Z1
 */
const MSG_T *msgs[] = 
{
&cmd_Z1EQL, //
&cmd_Z1EQC, //
&cmd_Z1EQR, //
&cmd_Z1EQSR, //
&cmd_Z1EQBSR, //
&cmd_Z1EQBSL, //
&cmd_Z1EQSL, //
&cmd_Z1EQSW, //
&cmd_Z1EQ, //

&cmd_Z1EMDIM, // 
&cmd_Z1EMP, //
&cmd_Z1EMC, //
&cmd_Z1EMDEL, //

&cmd_Z1EX, //
&cmd_Z1EF, //
&cmd_Z1EE, //
&cmd_Z1ES, //
&cmd_Z1EU, //
&cmd_Z1ED, //
&cmd_Z1E, //

&cmd_Z1VM, //
&cmd_Z1VFL_, //
&cmd_Z1VFR_, //
&cmd_Z1VC_, //
&cmd_Z1VSL_, //
&cmd_Z1VSR_, //
&cmd_Z1VBL_, //
&cmd_Z1VBR_, //
&cmd_Z1VS_, //
&cmd_Z1VFD_,

&cmd_Z1SAL1, //
&cmd_Z1SAL2, //
&cmd_Z1SAL3, //
&cmd_Z1SAL4, //
&cmd_Z1SAL5, //
&cmd_Z1SAL6, //
&cmd_Z1SAL7, //
&cmd_Z1SAL8, //

&cmd_Z1DF_, //
&cmd_Z1LPS, //
&cmd_Z1S, //	
&cmd_Z1P, //	
&cmd_Z1M, //	
&cmd_Z1D, //	
&cmd_Z1C, //	
&cmd_Z1A_,	//
&cmd_Z1_, //	

&cmd_Z1IN1, //
&cmd_Z1IN2, //
&cmd_Z1IN3, //
&cmd_Z1IN4, //
&cmd_Z1IN5, //
&cmd_Z1IN6, //
&cmd_Z1IN7, //
&cmd_Z1IN8, //

&cmd_Z1ACN7_1, //
&cmd_Z1ACN1, //
&cmd_Z1ACN2, //
&cmd_Z1ACN3, //
&cmd_Z1ACN4, //
&cmd_Z1ACN5, //
&cmd_Z1ACN6, //
&cmd_Z1ACN7, //
&cmd_Z1ACN8, //
&cmd_Z1ACNR, //

&cmd_Z2VFD_,
&cmd_Z2P, //	
&cmd_Z2S, //	 
&cmd_Z2V, //	
&cmd_Z2M, //	
&cmd_Z2_, //	

&cmd_TAT, //	
&cmd_TAHDT, //
&cmd_TFT, //	
&cmd_TFHDT, //	
&cmd_TAP, //	
&cmd_TAHDP, //
&cmd_TFP, //	
&cmd_TFHDP, // 
&cmd_TAS, // 
&cmd_TAHDS, //
&cmd_TAF, //
&cmd_TAHDF, //
&cmd_TT_, //	
&cmd_T,	 //	

&cmd_SLL, //
&cmd_SLC, //
&cmd_SLR, //
&cmd_SLSR, //
&cmd_SLSL, //
&cmd_SLBR, //
&cmd_SLBL, //
&cmd_SLSW, //

&cmd_SPL, //
&cmd_SPC, //
&cmd_SPR, //
&cmd_SPSR, //
&cmd_SPSL, //
&cmd_SPBR, //
&cmd_SPBL, //
&cmd_SPs, //

&cmd_SZXFL, //
&cmd_SZXFC, //
&cmd_SZXFR, //
&cmd_SZXSR, //
&cmd_SZXSL, //
&cmd_SZXSBR, //
&cmd_SZXSBL, //
&cmd_SZX, //

&cmd_SZL, //
&cmd_SZR, //
&cmd_SZSL, //
&cmd_SZSR, //
&cmd_SZSBL, //
&cmd_SZSBR, //
&cmd_SZC, //
&cmd_SZS, //

&cmd_SSPL, //
&cmd_SSPC, //
&cmd_SSPR, //
&cmd_SSPSR, //
&cmd_SSPSL, //
&cmd_SSPSBR, //
&cmd_SSPSBL, //
&cmd_SSPSW, //

&cmd_MSZL, //
&cmd_MSZR, //
&cmd_MSZSL, //
&cmd_MSZSR, //
&cmd_MSZSBL, //
&cmd_MSZSBR, //
&cmd_MSZC, //
&cmd_MSZS, //

&cmd_MSPL, //
&cmd_MSPC, //
&cmd_MSPR, //
&cmd_MSPSR, //
&cmd_MSPSL, //
&cmd_MSPBR, //
&cmd_MSPBL, //
&cmd_MSPs, //

&cmd_MSLL, //
&cmd_MSLC, //
&cmd_MSLR, //
&cmd_MSLSR, //
&cmd_MSLSL, //
&cmd_MSLBR, //
&cmd_MSLBL, //
&cmd_MSLSW, //

&cmd_MSZXFL, //
&cmd_MSZXFC, //
&cmd_MSZXFR, //
&cmd_MSZXSR, //
&cmd_MSZXSBR, //
&cmd_MSZXSBL, //
&cmd_MSZXSL, //
&cmd_MSZX, //

&cmd_MZ1EQL, //
&cmd_MZ1EQC, //
&cmd_MZ1EQR, //
&cmd_MZ1EQSR, //
&cmd_MZ1EQBSR, //
&cmd_MZ1EQBSL, //
&cmd_MZ1EQSL, //
&cmd_MZ1EQSW, //

&cmd_MSSPL, //
&cmd_MSSPC, //
&cmd_MSSPR, //
&cmd_MSSPSR, //
&cmd_MSSPSL, //
&cmd_MSSPSBR, //
&cmd_MSSPSBL, //
&cmd_MSSPSW, //

&cmd_RSN, //

&cmd_ASSP,
&cmd_ASHDMIAO,
&cmd_ASAVD,
&cmd_ASFP,
&cmd_ASRSN,
&cmd_ASIRC,
&cmd_ASIR2C,
&cmd_ASIRM,
&cmd_ASIR2M,
&cmd_ASTO1,
&cmd_ASTO2,
&cmd_ASTO3,
&cmd_ASAIS,
&cmd_ASDD,
&cmd_ASRD,
&cmd_ASPAO,
&cmd_ASMVO,
&cmd_AZ1SMV,
&cmd_AZ2SMV,
&cmd_SPU, 
NULL, /*最后字节必须是NULL*/
};

/*
 比较dis中是否包含src
 dis包含的字符数大于或者等于src，并且dis从头开始的字符依次和src一摸一样，则返回0
 否则返回1
*/
BOOL StrCmp(unsigned char *src,unsigned char *dis)
{
//	unsigned char i = 0;

#ifdef _DEBUG_RS232
	//rs232_printf("\n%s,%s\n",src,dis);
#endif

	do{
		if(*src != *dis)
			{
#ifdef _DEBUG_RS232
			//rs232_printf("err:%02X,%02X,%d\n",*src,*dis,i);
#endif
			return FALSE;
			}
		src++;
		dis++;
		//i++;
		}while(*src);
	return TRUE;
}

BOOL ProcRs232Msg(unsigned char * buffer, unsigned char size)
{
	unsigned char i=0;
	buffer[size] = 0;
	
#ifdef _DEBUG_RS232
	rs232_printf("\nRecv(%d):%s\n",size,buffer);
#endif

	for(i=0;msgs[i];i++)
		{
		if(StrCmp(msgs[i]->cmd,buffer) == TRUE)
			{
			return msgs[i]->run(buffer,size);
			}
		}
	return FALSE;
}

void C11Rs232(void)
{
	static unsigned char msg_status = MSG_IDLE, byte_count;

	unsigned char comm_data;

	if(IsQueueEmpty(protocol_queue_pointer) == QUEUE_NORMAL)	/*队列有数据*/
		{
		DeQueue(protocol_queue_pointer,&comm_data); /*出队列*/	
		echo(comm_data); /*发接收到的数据到串口打印出来*/

		if(comm_data == STX)
			{
			msg_status = MSG_IDLE;
			}

		switch(msg_status)
			{
			case MSG_IDLE:
				if(comm_data == STX)	/*在IDLE模式下，只要接收到STX，就表示开始有信息*/
					{
					msg_status = MSG_RECEIVING;
					byte_count = 0;
					}
				break;
			case MSG_RECEIVING:
				if((comm_data >= 0x20) && (comm_data <= 0x7F)) /*0x20-0x7F 之间的字符*/
					{
					recv_buffer[byte_count++] = comm_data;
					if(byte_count < PROTOCOL_BUF_SIZE)
						break;
					}
				if(comm_data == ETX)
					{
					if(TRUE == ProcRs232Msg(recv_buffer,byte_count))
						{
						rs232_printf("\n"); 
						msg_status = MSG_IDLE;
						break;
						}
					}
				rs232_printf("@ERR\n"); /*如果消息不为0x20-0x7F 和0x0D,表示接收到的数据是错的*/
				msg_status = MSG_IDLE;
				break;
			}
		}
}

