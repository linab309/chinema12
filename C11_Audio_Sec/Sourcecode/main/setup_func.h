#ifndef _SETUP_FUNC_H
#define _SETUP_FUNC_H

extern SETUP_T AvSyncDelaySetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T LateSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T BrightSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AutoInputSeekSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Zone2InputSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Zone2VolumeSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T SetMaxVolumeLevel(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue);
extern SETUP_T Other_AD_IN_LEVEL_Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T IrControlSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Zone2IrControlSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Zone2IrModeSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T RearIrModeSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T TriggerSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T TriggerSetup1(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T TriggerSetup2(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T RestoreDefaultSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern void SetupLoadEeprom(unsigned char exec);
extern void SetupLoadDefault(unsigned char);
extern void SetSpkPhase(unsigned char channel,unsigned char Phase);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern SETUP_T SpkConfigSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern SETUP_T SpkDistanceSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern SETUP_T SpkBlanceTestMode(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern SETUP_T SpkBlanceSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern SETUP_T  SpkCrossoverSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern SETUP_T SpkPhaseSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern SETUP_T SpkEqSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern SETUP_T AnalogLevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
/*
extern SETUP_T AnalogInput1LevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogInput2LevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogInput3LevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogInput4LevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogInput5LevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogInput6LevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogInput7LevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogInput8LevelSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
*/

extern SETUP_T AnalogInSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
/*
extern SETUP_T AnalogIn1Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogIn2Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogIn3Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogIn4Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogIn5Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogIn6Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogIn7Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogIn8Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogIn7Ch1Setup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AnalogInTunerSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
*/
extern SETUP_T CDPureAudio(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue);
extern SETUP_T MasterVolumeLevel(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue);
extern SETUP_T Z2MasterVolumeLevel(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue); 
extern SETUP_T HDMIAUIDOOUT(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue);
extern SETUP_T HDMIInAudioSetup(SETUP_MODE_T bMode,SETUP_VALUE_T bValue,SETUP_VALUE_T resValue);
extern SETUP_T InputNameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern void WriteName(unsigned char addr,unsigned char len,char *src);
extern SETUP_T LogoSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
/*
extern SETUP_T Input7CH1NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Input1NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Input2NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Input3NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Input4NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Input5NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Input6NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Input7NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T Input8NameSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
*/

extern SETUP_T FmFreqSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AmFreqStepSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

extern SETUP_T PasswordModeSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T PasswordSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

/*add cjm 2007-07-17*/
extern void SetSpkDistance(unsigned char channel,unsigned char distance);
extern SETUP_T SpkDimensionSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T SpkPanoramaSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T SpkWidthSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T SpkDelaySetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern SETUP_T AssignedInputSetup(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
extern void ChangeListenProfileMode(void);
/*add cjm 2007-07-17*/

extern uchar pwd_mode;
extern char strInput7CH1[];
extern char strInput1[];
extern char strInput2[];
extern char strInput3[];
extern char strInput4[];
extern char strInput5[];
extern char strInput6[];
extern char strInput7[];
extern char strInput8[];

#define DEFAULT_INPUT1_LEVEL 18		/*0dB*/
#define DEFAULT_INPUT2_LEVEL 18		/*0dB*/
#define DEFAULT_INPUT3_LEVEL 18		/*0dB*/
#define DEFAULT_INPUT4_LEVEL 18		/*0dB*/
#define DEFAULT_INPUT5_LEVEL 18		/*0dB*/
#define DEFAULT_INPUT6_LEVEL 18		/*0dB*/
#define DEFAULT_INPUT7_LEVEL 18		/*0dB*/
#define DEFAULT_INPUT8_LEVEL 18		/*0dB*/

#define DEFAULT_ANALOG1_MODE 0		/*Bypass*/
#define DEFAULT_ANALOG2_MODE 0		/*Bypass*/ //change by cjm 2009-03-31
#define DEFAULT_ANALOG3_MODE 0		/*Bypass*/ //change by cjm 2009-03-31
#define DEFAULT_ANALOG4_MODE 0		/*Bypass*/ //change by cjm 2009-03-31
#define DEFAULT_ANALOG5_MODE 0		/*Bypass*/ //change by cjm 2009-03-31
#define DEFAULT_ANALOG6_MODE 0		/*Bypass*/ //change by cjm 2009-03-31
#define DEFAULT_ANALOG7_MODE 0		/*Bypass*/ //change by cjm 2009-03-31
#define DEFAULT_ANALOG8_MODE 0		/*Bypass*/ //change by cjm 2009-03-31
#define DEFAULT_ANALOG7CH1_MODE 1	/*Dsp*/
#define DEFAULT_ANALOG_TUNER_MODE 1	/*Dsp*/

#define DEFAULT_INPUT_SEEK_MODE 1		/*Off*/
#define DEFAULT_AVSYNC_MODE 0			/*0ms*/
#define DEFAULT_BRIGHT_MODE 2			/*dimer 2*/
#define DEFAULT_IR_MODE 0				/*front*/
#define DEFAULT_ZONE2_IR_MODE 0		/*front*/
#define DEFAULT_LATE_MODE 0			/*Off*/
#define DEFAULT_PASSWORD_MODE 0		/*Off*/
#define DEFAULT_TRIGGER_MODE 0		/*ZONE 1*/

#define DEFAULT_FM_MODE 0				/*Europe*/
#define DEFAULT_AM_FREQ_STEP 0		/*10KHz*/

#define DEFAULT_ZONE2_INPUT 0			/*INPUT 1*/
#define DEFAULT_ZONE2_VOLUME 60		/*-30dB*/

#define DEFAULT_ZONE1_VOLUME 60		/*-30dB*/
#define DEFAULT_INPUT_CHANNEL INPUT_1
#endif

