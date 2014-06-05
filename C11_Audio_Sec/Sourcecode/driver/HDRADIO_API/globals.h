/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                globals.h										  					**
**																			  				**
** DESCRIPTION: 		Global defines														**
**********************************************************************************************/


#if     !defined(globals_H)
#define globals_H

/*!
 *      \file globals.h
 *      \brief Global defines
 */

#ifndef _KSE_
#define _KSE_
#define KSE_LOGO
//#define FM_FREQ_875
#define KSE_FM_MIXER_GAIN  //+6db
#define KSE_AM_PGA_GAIN  // 2.0
#define KSE_FM_PGA_GAIN  // 1.6
//#define USE_COMMENT
//#define USE_TRACK
//#define USE_COMMERCIAL
//#define DRI352
#define OLD_VOLUME
//#define HD4101
#define IDM_SERIAL_COMMUNICATION
#ifdef  IDM_SERIAL_COMMUNICATION
#define SCI2_ENABLE
#endif
#endif

#ifndef FALSE
#define FALSE    0
#endif

#ifndef TRUE
#define TRUE     1
#endif

#ifndef _PC
//typedef unsigned char  BOOL;
//typedef void VOID;
//typedef unsigned long  UINT32;      // Unsigned 32 bit
//typedef signed   long  INT32;      // Signed   32 bit
#endif

typedef unsigned char  BOOLEAN;
typedef unsigned char  UINT8;      // Unsigned  8 bit
typedef signed   char  INT8;       // Signed    8 bit
typedef unsigned short UINT16;      // Unsigned 16 bit
typedef unsigned long  UINT32;      // Unsigned 32 bit
typedef signed   short INT16;      // Signed   16 bit
typedef double         FP32;

typedef enum
{
	CMD_SIS =1,
	CMD_EXTENDED_SIS,
	CMD_ALFN,
	CMD_PSD_TAG
}MONITOR_COMMANDS;

//Commercial info
typedef struct _COMMERCIAL_INFO
{
  	UINT8 Price[16];
  	UINT8 Date[9];
  	UINT8 Contact[40];
  	UINT8 Received;
  	UINT8 SellerName[40];
  	UINT8 Description[64];
}COMMERCIAL_INFO;

typedef enum
{
	TIME_AM=0,
	TIME_PM
}AM_PM;

typedef struct _datetime
{
	UINT8 hr;
	UINT8 min;
	UINT8 am_pm;
	UINT8 mon;
	UINT8 day;
	UINT8 dow;
	UINT16 year;
}DATETIME;

typedef struct _time
{
	UINT8 hr;
	UINT8 min;
	UINT8 am_pm;
}TIME;

//BANDS
typedef enum
{
    BAND_AM,
    BAND_FM,
    BANDS
} BAND;

//Digital Audio Status
typedef enum
{
	NOT_ACQUIRED,
	ACQUIRED
} DIGITAL_AUDIO_STATUS;

//Seek Direction
typedef enum
{
	DOWN=0,
	UP
} DIRECTION;

#ifdef USE_ALARM
//Alarm Modes
typedef enum
{
        ALARM_OFF = 1,
        ALARM_FM,
        ALARM_AM,
		ALARM_BEEP,
        ALARM_MODES
}ALARM_MODE;

//Alarm Type
typedef enum
{
	ALARM_ONCE = 1,
	ALARM_DAILY,
	ALARM_WEEKENDS,
	ALARM_WEEKDAYS,
	ALARM_REPEAT_TYPES
}ALARM_REPEAT_TYPE;
#endif

//Alarm Data
typedef struct _ALARM_DATA
{
	UINT8       mode;
	UINT8       FMIdx;
	UINT8       AMIdx;
	TIME		time;
	UINT8		repeat;
}ALARM_DATA;


typedef enum
{
	AUX_AM_FM = 0,
	AUX_IN,
	AUX_MODES
}AUX_MODE;

typedef enum
{
	DAC1_LEFT_RIGHT = 3,
	DAC2_LEFT_RIGHT = 4,
	DAC1_LEFT = 6,
	DAC1_RIGHT = 7,
	DAC2_LEFT = 8,
	DAC2_RIGHT = 9
}DAC_ID;

typedef enum 
{
	DIS_NONE = 0,
	DIS_CHANNEL,
	DIS_ARTIST,
	DIS_TITLE,
#ifdef USE_COMMENT	
	DIS_COMMENT,
#endif	
#ifdef USE_UFID
	DIS_UFID_DATA,
	DIS_UFID_OWNER,
#endif
#ifdef USE_TRACK
	DIS_TRACK,
#endif
#ifdef USE_COMMERCIAL
	DIS_COMMERCIAL,
#endif
#ifdef USE_PTY
	DIS_PTY,
#endif	
#ifdef USE_AUDIOINFO
	DIS_AUDIO,
#endif
#ifdef _DEBUG
	DIS_SEEK_LEVEL,     
#endif
#ifdef USE_CDNO
	DIS_CDNO,
#endif
#ifdef USE_RADIOID
	DIS_RADIOID,
#endif
	DIS_MAX
} DIS_DATA;

/******************************************************************
 *                                  FM
 *									0x00 - Auto Blend Disabled
 *									0x01 - Mono
 *									0x02 - Partial Stereo
 *									0x03 - Stereo
 *
 *                                  AM
 *									0x00 - Mono
 *									0x01 - Stereo
 ******************************************************************/
typedef enum
{
   ST_DISABLED_MONO = 0,
   ST_MONO_STEREO,
   ST_PARTIAL,
   ST_STEREO
} STEREO_STATUS;

typedef enum
{
   NONE_MODE = 0,
   SRS_CIRCLE,              //0x01, . SRS Circle Surround
   DOLBY_PROLOGIC2,   //0x02, . Dolby ProLogic2 Surround
   NEURAL,                    //0x03, . Neural Surround
   FHG_HEMP3,              //0x04, . FHG HEMP3 Surround
   DTS_MATRIX,             //0x05, . DTS Matrix Surround
   CT                               //0x06, . CT Surround   
} SURROUND_MODE;


typedef enum
{
	BLEND_MUTE = 0,
	BLEND_MONO,
	BLEND_STEREO,
	BLEND_AUTO
}BLEND_MODE;

#define ID3_TAG_LEN			65
#define ID3_COMMENT_LEN 	200
#define ID3_UFID_URL_LEN	128
#define ID3_UFID_DATA_LEN	65
#define ID3_GENRE_LEN		32
#define ID3_TRACK_LEN		10

#define CA_SVC_MSG_LEN	128

#define DEFAULT_EX_MSG_SIZE	1037 //12+1024+1

#if defined(_ENABLE_EXTENDED_IDM_SUPPORT)
	#define CMD_BUFFER_SIZE 256//260	//1037 //13+1024+1
#else
	#define CMD_BUFFER_SIZE 256 //1+9+245+1
#endif

#define SERIAL_BUFFER_SIZE 256

#define FM_MONO			1
#define FM_STEREO		2
#define FM_AUTO		3

#define SetBit(x,y)     (x |= (1<<y))
#define ClrBit(x,y)     (x &= ~(1<<y))
#define ToggleBit(x,y)  (x ^= (1<<y))
#define FlipBit(x,y)    (x ^= (1<<y)) // Same as ToggleBit.
#define TestBit(x,y)    (x &  (1<<y))

typedef struct
{
    UINT16 media_sector_size;   // size in bytes of one sector
    UINT32 media_total_sectors;
    UINT32 media_used_sectors;
    UINT32 media_free_sectors;
    UINT8 is_mounted;
} MEDIA_STATUS_TYPE;


typedef struct
{
    UINT32 total_seconds;
    UINT32 curr_seconds;
    UINT8 codec_mode;
    UINT8 num_streams;
    UINT8 tx_audio_gain;
    UINT8 bit_rate;
    UINT8 sound_processing;
    UINT8 access_rights;
    UINT32 content_id;
    UINT8 badEnhPkts;
    UINT8 badCorePkts;
    UINT16 psd_count;
    UINT8 dig_audio_flg;
    UINT8 blend_flag;
    UINT8 nav_flag;
    UINT8 eof;          // 1 = true, 0 = false
} SP_STATUS_TYPE;


#endif /* End of globals_H */
