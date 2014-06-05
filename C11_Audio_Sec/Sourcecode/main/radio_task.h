#ifndef _RADIO_TASK_H
#define _RADIO_TASK_H

#define Tuner_TEXT_BUF_SIZE		80
#define Tuner_TEXT_DISP_SIZE		20

#define Tuner_MSG_Head1			0x5A
#define Tuner_MSG_Head2			0xA5
#define Tuner_MSG_SIS_Com_Leg_H	0x00
#define Tuner_MSG_SIS_Com_Leg_L	0x10
#define Tuner_MSG_SIS_Com_Byte	0xF1
#define Tuner_MSG_PSD_Com_Leg_H	0x00
#define Tuner_MSG_PSD_Com_Leg_L	0x51
#define Tuner_MSG_PSD_Com_Byte	0xF2

enum{
MSG_Idle,
MSG_Head1,
MSG_Head2,
MSG_Com_Leg_H,
MSG_Com_Leg_L,
MSG_Com_Byte,
MSG_Reply_Pack,
};

#define Text_reply_time				3

#define _AM_MODE 					1
#define _FM_MODE 					2
#define _HD_AM_MODE				3
#define _HD_FM_MODE				4

#define DEFAULT_AM_POSITION 		0
#define DEFAULT_FM_POSITION 		0
#define DEFAULT_HDFM_POSITION		0
#define DEFAULT_HDAM_POSITION		0

#define EUROPE_FM_MAX_FREQ 	108000L		/*108MHz*/
#define EUROPE_FM_MIN_FREQ  	87900L		/*87.9MHz*/
#define HD_FM_MAX_FREQ	  	108000L 		/*108.0MHz*/
#define HD_FM_MIN_FREQ		  	87900L		/*87.9MHZ*/

#define AM_10K_MAX_FREQ 		1700L		/*1700KHz*/
#define AM_10K_MIN_FREQ 		530L		/*530KHz*/
#define HD_AM_MAX_FREQ		1700L		/*1700KHZ*/
#define HD_AM_MIN_FREEQ		530L		/*530KHZ*/

#define FM_STEP 					200			/*200khz*/
#define AM_STEP					10			/*10khz*/
#define HDFM_STEP				200			/*200khz*/
#define HDAM_STEP				10			/*10khz*/

extern FREQT DEFAULT_AM_FREQ;
extern FREQT MAX_AM_FREQ;
extern FREQT MIN_AM_FREQ;

extern FREQT DEFAULT_FM_FREQ;
extern FREQT MAX_FM_FREQ;
extern FREQT MIN_FM_FREQ;

extern uchar am_preset_base;
extern unsigned char current_radio_mode;
extern unsigned char system_position;

extern unsigned char am_position;
extern unsigned char fm_position;
extern unsigned char HDFM_position;     
extern unsigned char HDAM_position;     

extern uchar stereo_mode;
extern FREQT system_freq;
extern unsigned char Tuner_Reply_Flag;
extern unsigned int rec_reply_count;

extern unsigned int fm_freq;		
extern unsigned int am_freq;		
extern unsigned int HDFM_freq;	
extern unsigned int HDAM_freq;	

extern BOOL SaveFmPreset(unsigned char position,unsigned int freq,unsigned char tuner_mode);
extern void Tuner_Req_Radio_Monitor_Command(unsigned char Radio_Service,unsigned char SIS_Flag,unsigned char PSD_Flag);
extern void C11_Tuner_PSD_Change(void);
extern void Radio_Text_Display(void);
extern void RadioDisplay(unsigned int freq,unsigned char tuner_mode,unsigned char position);
extern BOOL LoadRadioPreset(unsigned char position,unsigned int * freq,unsigned char tuner_mode);
extern BOOL IsFreqValid(FREQT freq, unsigned char radio_mode);
extern BOOL RadioInit(unsigned char mode);
extern void TunerResetPreset(void);
extern void RadioLoop(void);
extern void RadioExit(void);
extern BOOL MsgSetRadioLoadPreset(unsigned char position);
extern void AcqStatusSave (uint8 * pAcqData);
extern uint8 GetCurrProgram(void);
#endif

