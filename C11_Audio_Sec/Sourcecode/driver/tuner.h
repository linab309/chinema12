
#ifndef _TUNER_H
#define _TUNER_H

#define Band_FM 			1
#define Band_AM 			2
#define Band_HD_FM		3
#define Band_HD_AM		4

#define Tune_Up_FALSE	0
#define Tune_Up_True	1

#define Tune_Mute		1
#define Tune_Unmute       	0

#define Seek_Up			1
#define Seek_Down		0

extern void Tuner_Req_Radio_Monitor_Command(unsigned char Radio_Service,unsigned char SIS_Flag,unsigned char PSD_Flag);
extern void Tuner_Band_Command(unsigned char band);
extern void Tuner_Req_Tune_Up(unsigned char mode);
extern void Tuner_Req_Tune_Down(unsigned char mode);
extern void Tuner_Stop_Seek(void);
extern uchar Tuner_Seek_Command(unsigned char direction);
extern uchar Tuner_HDSEEK_Command(unsigned char direction);
extern void Tuner_Mute_Command(unsigned char mute);
extern void Tuner_Volume_Init(void);
extern void Tuner_Ext_Reset(void);
extern void Tuner_Req_Freq(void);
extern unsigned char FreqCheckSum(unsigned char command,unsigned char Freq_H,unsigned char Freq_L);
extern void Tuner_Write_Freq_Command(unsigned char mode,unsigned int Tuner_Freq);
extern void InitTuner(unsigned char mode);
extern void Tune_GetStatus(uint8 type);
extern BOOL get_rbds_group(uint8* buf);
/****************************************************************
 IDM/RX_IDD_2206 opcodes 
****************************************************************/
//Category 1 Commands
#define ACQUIRE_OPCODE          0x01	
#define AM_FM_OPCODE            0x02	

//Category 3 Commands
#define SYS_STATUS_OPCODE       0x20
#define GET_QI_OPCODE           0x21
#define SEL_PROG_OPCODE         0x22
#define ACQ_STATUS_OPCODE       0x23
#define GET_SNR_OPCODE			0x24

//Category 4 Commands
#define GET_PSD_OPCODE          0x43
#define FLUSH_PSD_QUEUE_OPCODE  0x44
#define SET_SIS_CNFG_OPCODE     0x45
#define GET_SIS_CNFG_OPCODE     0x46
#define GET_SIS_DATA_OPCODE     0x47
#define FLUSH_SIS_DATA_OPCODE   0x48
#define GET_SERVICE_INFO_OPCODE 0x4B

/* Analog FM demod function codes */
#define ANALOG_FM_STEREO_CTRL_OPCODE 0x00
#define FM_GET_STATUS           0x0C
#define GET_FM_SEEK_STRENGTH    0x0D

/* Conditional Access Section */
#define CA_OPCODE				0x75

	/** conditional access function codes **/
	#define CA_LOAD_EMM_FC			0x00
	#define CA_DIAGNOSTICS_FC		0x01
	#define CA_RADIO_ID_FC			0x02
	#define CA_INFO_FC				0x03
	#define CA_SVC_MSG_FC			0x04
	/** diagnostic request options **/
	#define CA_DIAG_SVC_TABLE_TYPE	0x00
	#define CA_DIAG_PERSONAL_BITS_TYPE 0x01
	/** function return status **/
	#define CA_FUNC_SUCCESS			0x00

/****************************************************************
 Macro Commads Section 
***************************************************************/
/* System Version Section */   
#define SYS_VERSION_OPCODE		0x80

	/* Sys Version Function codes */
	#define GET_SW_VERSION_FC		0x01
	#define GET_HW_VERSION_FC		0x02
	#define GET_CMD_SET_VERSION_FC	0x03
	#define GET_CNFG_VERSION_FC		0x04

/* System Tune Section */
#define SYS_TUNE_OPCODE			0x82

	/* SysTune functions */
	#define TUNE_SELECT				0x01
	#define TUNE_STEP				0x02
	#define TUNE_GET_STATUS			0x06
		//Tune Get Status Types
		#define GET_STATUS_ALL			0x00
		#define GET_STATUS_ANALOG_ONLY	0x01
		#define GET_STATUS_DIGITAL_ONLY	0x02

/* System Audio Section */
#define SYS_AUDIO_OPCODE		0x8B	

	/* Sys Audio Functions */
	#define MONO_MODE_FC			0x01
		#define MONO_MODE_OFF			0x00	//Default. (=STEREO_MODE)
		#define	MONO_MODE_ON			0x01

	#define VOLUME_CONTROL_FC		0x02
   
	#define DAC_SERVICE_SELECT_FC	0x03
		//DAC service modes
		#define DAC_SILENCE_MODE		0x01
		#define DAC_IBOC_MODE			0x02	
		#define DAC_ANALOG_MODE			0x03	
		#define DAC_HD_MODE				0x04	
		#define DAC_AUX_MODE			0x05

		//DAC Routing
		#define ROUTE_DAC_0		0x01
		#define ROUTE_DAC_1		0x02
		#define ROUTE_DAC_2		0x04
   
/* System Dignostics Section */   
#define SYS_DIAGNOSTICS			0x8D	
	#define SPLIT_AUDIO_MODE_FC	0x01
	#define	DIAGNOSTIC_TONES_FC	0x0A

/* System Analog Section */   
#define AM_DEMOD_CTRL_OPCODE    0xA0	//FIXME Not implemented yet
   /* Analog AM demod function codes */   
	#define AM_GET_STATUS				0x05
	#define GET_AM_SEEK_STRENGTH		 0x22

#define FM_DEMOD_CTRL_OPCODE    0xA1
	/* Analog FM demod function codes */
	#define ANALOG_FM_STEREO_CTRL_OPCODE 0x00
	#define FM_GET_STATUS           0x0C
	#define GET_FM_SEEK_STRENGTH    0x0D
	#define FM_LEFT_RIGHT_SCALING	0x01	//dabb
	#define GET_ANALOG_FM_CNFG		0x7E	//dabb
	
#define RBDS_OPCODE				0xA2	//FIXME Not implemented yet


#define	FM_LOOP_DELAY	60
#define	AM_LOOP_DELAY	100
#define	FM_SEEK_DELAY	160
#define AM_SEEK_DELAY	160


extern void idm_get_FM_status (void);
extern void Radio_GetStatus(void);
extern void EnableRBDS (BOOL enable);
#endif

