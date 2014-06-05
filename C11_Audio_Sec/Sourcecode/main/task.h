#ifndef _TASK_H
#define _TASK_H

#include "macro.h"

#ifdef _USE_ARTX
#include <ARTX.h>
#include <ARTX_Config.h>

extern OS_TID comm_task_id;
extern OS_TID key_task_id;
extern OS_TID dsp_task_id;

extern void key_task (void);// __task;
extern void dsp_task (void);// __task;
extern void comm_task (void);// __task;

#define TASK_TICK_MS_TIME 10

#define KEY_TASK_TICK 1	/*KEY TASK执行的time tick*/
#define DSP_TASK_TICK 1	/*KEY TASK执行的time tick*/
#define COMM_TASK_TICK 1	/*KEY TASK执行的time tick*/
#else
extern void key_task (void);
extern void comm_task(void);
#endif
#if 0
typedef struct {
	unsigned short cmd_header;
	unsigned short cmd_value;
	unsigned char valid_flag;
}_TASK_MSG;
#endif
typedef struct {
	unsigned char key_type;
	KEYT key_value;
}KEY_TASK_MSG;
typedef void (*PFV)();
typedef TASK_RESULT (*TASK_PROC)(unsigned char *);
typedef struct{
	TASK_ID t_id;
	TASK_PROC run;
}TASK_T;

enum{
NULL_ID = 0,
ZONE1_ID,
ZONE2_ID,
SETUP_ID,
RADIO_ID,
AUTO_SOUND_ID,
FLASH_UPDATE_ID,
};

typedef struct{
unsigned char pcm_decode_mode;
unsigned char pcm_sur_mode;
unsigned char dolby_type;
unsigned char dolby_decode_mode;
unsigned char dolby_sur_mode;
unsigned char dolby_hp_mode;
unsigned char dts_type;
unsigned char dts_decode_mode;
unsigned char dts_sur_mode;
}STREAM_DECODE_MODE;

typedef struct{
unsigned char system_status;

unsigned char system_zone1_status;
short zone1_volume;	/*主音量*/
unsigned char zone1_channel;	/*当前系统的输入源*/
unsigned char zone1_sub_channel;	/*当前系统的输入源*/
unsigned char zone1_mute_flag;

unsigned char system_zone2_status;
unsigned char zone2_channel;
short zone2_volume;	/*二区音量*/
unsigned char zone2_mute_flag;

unsigned char system_dimer;	/*系统DIMER*/
#if 0
short balance_fl;	/*front left 平衡*/
short balance_fr;	/*front right 平衡*/
short balance_sl;	/*surround left 平衡*/
short balance_sr;	/*surround right 平衡*/
short balance_cen;	/*center 平衡*/
short balance_sw;	/*subwoofer 平衡*/
short balance_sbl;	/*surround back left 平衡*/
short balance_sbr;	/*surround back right 平衡*/
#endif
unsigned char stream_type;
unsigned char stream_subtype;
unsigned char src_mode;	/*0为digital,1为analog in, dsp out,2为bypass*/
STREAM_DECODE_MODE decode_mode;
unsigned char headphone_flag;
}SYSTEM_FLAG;

extern SYSTEM_FLAG sys_flag;

#define POWER_OFF 	0
#define POWER_ON 	1

#define POWER_ON_DELAY_TIME 	5000  
#define POWER_OFF_DELAY_TIME 	5000

#define POWER_ON_DEFAULT_VOLUME (-30)	/*-30dB*/

extern void VfdUpdateVolumeValue(short volume);
extern void VfdUpdateInputValue(unsigned char input);
extern void VfdUpdateStreamType(unsigned char type,unsigned char sub_type);

extern void DelayMs(int delay_time);
extern void SystemReset(void);

extern void DspLoop(void);
extern void DspProcMsg(unsigned char *buffer);

extern TASK_RESULT Zone1ProcMessage(unsigned char *buffer);
extern TASK_RESULT SetupProcMessage(unsigned char *buffer);
extern TASK_RESULT RadioProcMessage(unsigned char *buffer);
extern TASK_RESULT Zone2ProcMessage(unsigned char *buffer);
extern TASK_RESULT AutoSoundProcMessage(unsigned char *buffer);
extern TASK_RESULT FlashUpdateProcMessage(unsigned char *buffer);

extern BOOL IsCurFocusTid(TASK_ID t_id);
extern BOOL SetCurFocusTid(TASK_ID t_id);
extern TASK_ID GetCurFocusTid(void);

extern BOOL MsgSetDSPHDMISetting(TASK_ID t_id,unsigned char value);
extern BOOL MsgSetDSPSetting(TASK_ID t_id,unsigned char value);
extern BOOL MsgSetRadioFreq(unsigned char mode,FREQT freq);
extern BOOL MsgSetRadioSearch(unsigned char mode,unsigned char init);
extern BOOL MsgSetStream(unsigned char type,uchar sub_type);
extern BOOL MsgSetDimer(unsigned char dimer_value,unsigned char mode);
extern BOOL MsgSetKey(TASK_ID t_id,unsigned char key_mode,KEYT key_value);
extern BOOL MsgSetVfdShow(TASK_ID t_id,unsigned char mode);
extern BOOL MsgSetVolume(TASK_ID t_id,unsigned char mode,short volume);
extern BOOL MsgSetFlash(uchar mode, uchar position, uchar value);
extern BOOL MsgSetPower(TASK_ID t_id, unsigned char mode);
extern BOOL MsgSetInputSource(TASK_ID t_id, unsigned char src);
extern BOOL MsgSetMute(TASK_ID t_id, unsigned char channel, unsigned char mode);

extern void SystemPowerOn(void);
extern void SystemPowerOff(void);

extern char *GetSrcName(unsigned char channel);

extern void LoadSystemStartupValue(void);
extern BOOL IsZone1VolumeValid(short volume);

enum{
RADIO_SEARCH_UP = 0,
RADIO_SEARCH_DN,
RADIO_SEEK_UP,
RADIO_SEEK_DN,
RADIO_SCAN,
RADIO_STOP,
};

enum{
MODE_PLUS = 0,
MODE_MINUS,
MODE_FIXED,
MODE_TOGGLE,
};

struct ID_NAME{
uchar spk_index;
uchar spk_bit;
char *name;
};

typedef struct SpkStatus{
unsigned char valid;
unsigned char phase;
float balance;
float distance;
float eq[10];
unsigned char crossover_point[3];
struct ID_NAME * channel;  
}SPKSTATUS;
extern SPKSTATUS * speaker_table[];

extern const char * DefaultName[];
extern char * SrcName[];
extern char SrcNameCh71[];
extern char SrcNameInput1[];
extern char SrcNameInput2[];
extern char SrcNameInput3[];
extern char SrcNameInput4[];
extern char SrcNameInput5[];
extern char SrcNameInput6[];
extern char SrcNameInput7[];
extern char SrcNameInput8[];
extern char SrcNameInputHDMI1[];
extern char SrcNameInputHDMI2[]; 
extern char SrcNameInputHDMI3[];
extern char SrcNameInputHDMI4[]; 
extern char SrcNameInputARC[]; 
extern char C11Logo[];
extern const char CaryLogo[];

#ifndef C11_VERSION
#define C11_VERSION "V2.03"      
#endif

extern uchar AnalogInLevel[];
extern uchar AnalogInMode[];

extern uchar SystemTriggerMode;
extern uchar SystemTriggerMode1;
extern uchar SystemTriggerMode2;
#define ZONE1_MAX_VOLUME (15)
#define ZONE1_VOLUME_STEP (1)
#define ZONE1_MIN_VOLUME (-90)

#define ZONE2_MAX_VOLUME (15)
#define ZONE2_VOLUME_STEP (1)
#define ZONE2_MIN_VOLUME (-90)

extern BOOL IsZone1PowerOn(void);
extern BOOL IsZone2PowerOn(void);
extern BOOL Zone2RadioMode(unsigned char mode);
extern BOOL Zone1ProcAutoSeek(unsigned char start_channel,unsigned char sub_channel,unsigned char goon_seeking);
extern void SetAutoseekFlag(unsigned char on);
extern unsigned char IsEverAutoseek(void);

extern void C11Rs232(void);
#define default_proc Zone1ProcMessage

extern unsigned char auto_sound_testing;
extern PFV Rs232ProtocolHandler;
#endif

