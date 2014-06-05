#define _USE_COMM_DSP_WRITE

#define DSP_MAX_MSG_SIZE 			128	/*DSPAB数据包最大值*/
#define Enable 			      			1
#define Disable 			      			0

/*check Message*/
#define BOOT_START 		    			0x00000001
#define BOOT_SUCCESS         			0x00000002
#define APP_START		    			0x00000004
#define SOFTBOOT_ACK	    			0x00000005
#define FLASH_VERIFIED				0xEF00002000000001
#define DSP_NOSIGNEAL				0x8100000000000020

/*DSP Opcode*/
#define OS_A_B_Wop					0x81
#define Dobly_Digital_Wop			0x90
#define PCM_Manager_Wop			0x9B
#define Signal_Generator_Wop		0x9A
#define Audio_Manager_Wop			0x83
#define Bass_Manager_Wop			0xD7
#define Crossbar_Manager_Wop		0xDB
#define Dolby_Pro_LogicII_Wop		0xB2
#define Dolby_Pro_LogicIIx_Wop		0xBF
#define Eq_Manager_Wop				0xD5
#define Delay_Manager_Wop			0xD9
#define IRC_1_Manager_Wop			0x9C
#define IRC_2_Manager_Wop			0x9D
#define DTS_Manager_Wop			0x91
#define DTS_NEO6_Wop				0xB5
#define DTS_ES_Wop					0x91
#define Dolby_Headphone_Wop		0xC0
#define Dolby_Digital_Plus_Wop		0x90
#define Dolby_True_HD_Wop			0x9D
#define Dsp_API_Wop					0xEF

/*ULD in the flash addr-固件版本3*/
#define ADDR_AAC_A					0x000000
#define ADDR_AC3_A					0x000100
#define ADDR_AM_B					0x000200
#define ADDR_APP_B					0x000300
#define ADDR_AUDISTRY1_B			0x001700
#define ADDR_AUDISTRY2_B			0x004200
#define ADDR_AUDISTRY_B			0x005F00
#define ADDR_COMS2_A				0x00A300
#define ADDR_CROSSBAR_A			0x00AF00
#define ADDR_CROSSBAR_B			0x00B000
#define ADDR_DH2_B					0x00B100
#define ADDR_DTS_A					0x00B200
#define ADDR_DTS_ES_A				0x00B500
#define ADDR_DVS2_B				0x00B800
#define ADDR_IRC_B					0x00BA00
#define ADDR_LOGIC7_A				0x00ED00
#define ADDR_NEO6_A				0x011F00
#define ADDR_OS_A					0x013500
#define ADDR_OS_B					0x013700
#define ADDR_OS_PLUS_A				0x013900
#define ADDR_OS_PLUS_B				0x014100
#define ADDR_PL2_A					0x014300
#define ADDR_PL2X_A					0x014500
#define ADDR_SERIAL_FLASH_B		0x014700
#define ADDR_SGEN_A				0x015100
#define ADDR_SPP_B					0x015200
#define ADDR_THX_B					0x015500
#define ADDR_TSXT_B					0x015600
#define ADDR_NEO6_2_A				0x016000
#define ADDR_NEO6_3_A				0x018000
#define ADDR_NEO6_4_A				0x018100
#define ADDR_EC3_HD_B_03			0x018200				
#define ADDR_EC3_HD_B_06			0x018C00		
#define ADDR_EC371_A_02			0x019600		
#define ADDR_EC371_A_03			0x021100		
#define ADDR_OS_HD_A_04			0x028c00	
#define ADDR_OS_HD_A_06			0x029F00
#define ADDR_OS_HD_B_04			0x02A300
#define ADDR_OS_HD_B_06			0x02AD00
#define ADDR_SETUP_CROSSBAR_A_03	0x032D00	
#define ADDR_SETUP_CROSSBAR_A_06	0x032E00	
#define ADDR_TRUEHD_A				0x032f00
#define ADDR_TRUEHD_B				0x039500

#define ADDR_AC3_A_07				0x039A00		
#define ADDR_APP_B_07				0x039B00				
#define ADDR_COMS2_A_07			0x03B000	
#define ADDR_CROSSBAR_A_07_RC7	0x03BC00			
#define ADDR_CROSSBAR_A_07_RC9	0x03BD00		
#define ADDR_CROSSBAR_B_07_RC4	0x03BE00			
#define ADDR_CROSSBAR_B_07_RC5	0x03BF00		
#define ADDR_DH2_B_07				0x03C000				
#define ADDR_DTS_A_07				0x03C100			
#define ADDR_DTS_ES_A_07			0x03C400				
#define ADDR_IRC_B_07				0x03C700				
#define ADDR_NEO6_A_07				0x03FA00			
#define ADDR_OS_HD2_A_07			0x03FB00		
#define ADDR_OS_HD2_AB_07			0x03FF00		
#define ADDR_OS_HD2_B_07			0x040400					
#define ADDR_PL2_A_07				0x040600		
#define ADDR_PL2X_A_07				0x040800
#define ADDR_SERIAL_FLASH_A_07	0x040A00			
#define ADDR_SERIAL_FLASH_B_07	0x041A00		
#define ADDR_SETUP_CROSSBAR_A_07	0x042400	
#define ADDR_SGEN_A_07				0x042500	
#define ADDR_SPP_B_07				0x042600

#define AAC_A						0			
#define AC3_A						1
#define AM_B							2
#define APP_B						3
#define AUDISTRY1_B					4
#define AUDISTRY2_B					5
#define AUDISTRY_B					6
#define COMS2_A						7
#define CROSSBAR_A					8
#define CROSSBAR_B					9
#define DH2_B						10
#define DTS_A						11	
#define DTS_ES_A					12
#define DVS2_B						13
#define IRC_B						14	
#define LOGIC7_A					15
#define NEO6_A						16
#define OS_A							17
#define OS_B							18
#define OS_PLUS_A					19
#define OS_PLUS_B					20	
#define PL2_A						21
#define PL2X_A						22
#define SERIAL_FLASH_B				23
#define SGEN_A						24
#define SPP_B						25
#define THX_B						26
#define TSXT_B						27
#define NEO6_2_A					28
#define NEO6_3_A					29
#define NEO6_4_A					30
#define EC3_HD_B_03					31
#define EC3_HD_B_06					32
#define EC371_A_02					33
#define EC371_A_03					34
#define OS_HD_A_04					35
#define OS_HD_A_06					36
#define OS_HD_B_04					37
#define OS_HD_B_06					38
#define SETUP_CROSSBAR_A_03		39
#define SETUP_CROSSBAR_A_06		40
#define TRUEHD_A					41
#define TRUEHD_B					42

#define AC3_A_07					43
#define APP_B_07					44
#define COMS2_A_07					45
#define CROSSBAR_A_07_RC7			46
#define CROSSBAR_A_07_RC9			47
#define CROSSBAR_B_07_RC4			48
#define CROSSBAR_B_07_RC5			49
#define DH2_B_07					50
#define DTS_A_07					51
#define DTS_ES_A_07					52  
#define IRC_B_07						53
#define NEO6_A_07					54
#define OS_HD2_A_07				55
#define OS_HD2_AB_07				56
#define OS_HD2_B_07					57
#define PL2_A_07					58
#define PL2X_A_07					59
#define SERIAL_FLASH_A_07			60
#define SERIAL_FLASH_B_07			61
#define SETUP_CROSSBAR_A_07		62
#define SGEN_A_07					63
#define SPP_B_07					64

/*Unsolictited Messages(Read-Only)*/
#define UNSOLICITED_MSG_ID			0x81
#define DOLBY_MSG_ID				0x90
#define DTS_MSG_ID					0x91		
#define DOLBY_TURE_HD_ID			0x9D
#define DTS_HD_MA_ID				0xA0
#define DTS_HD_HRA_ID				0x9F
#define AUTODETECT_RESPONSE		0x00
#define PLL_OUT_OF_CLK				0x02

#define STREAM_TYPE_MASK			0xFF
#define DECODEABLE_STRM_FLAG  		0x80
#define NON_IEC_STFM_FLAG			0x20

enum{
SILENT_INPUT_DATA =0x00,			/*0x00*/
SILENT_DATA=0x20,					/*0x20*/
DTS_FMT16_STREAM=0x21,			/*0x21*/
DTS_FMT14_STREAM=0x22,			/*0x22*/
LINEAR_2_1_PCM_STREAM=0x23,		/*0x23*/
LINEAR_5_1_PCM_STREAM=0x623,        /*0x623*/
LINEAR_7_1_PCM_STREAM=0x823,        /*0x823*/
HDCD_PCM_Detect_STREAM=0x24,		/*0x24*/
HDCD_PCM_Lost_STREAM=0x25,		/*0X25*/
};

/*IEC61937 Stream*/
/*IEC61937 Stream*/
#define DTS_HD_MA_Stream_48k			0x0411
#define DTS_HD_MA_Stream_96k_192K	0x4411
#define DTS_HD_HRA_Stream_48K			0X0211
#define DTS_HD_HRA_Stream_96k			0X4211
enum{
NEVER_REPORTED = 0x00,				/*0x00*/
AC_3_DATA,							/*0x01*/

MPEG1_LAYER1_DATA = 0x04,			/*0x04*/
MPEG1_LAYER2_3_DATA,				/*0x05*/
MPEG2_DATA,						/*0x06*/
MPEG2_AAC_ADTS_DATA,				/*0x07*/
MPEG2_LAYER1_DATA = 0x08,			/*0x08*/
MPEG2_LAYER2_3_DATA,				/*0x09*/

DTS_1_DATA = 0x0B,					/*0x0B*/
DTS_2_DATA,						/*0x0C*/
DTS_3_DATA,						/*0x0D*/

MPEG2_AAC_ADTS = 0x1C,			/*0x1C*/

DOLBY_DIGITAL_PLUS=0x15,          	/*0x15*/
True_HD=0x16,						/*0x16*/

DTS_HD_48K=0X0411,					/*0x411*/
DTS_HD_96k=0X0441,					/*0x441*/
DTS_HD_HRA=0x0211,				/*0x211*/
};

#define POW_2_23 					0x7FFFFF	//8388608L
#define POW_2_24 					0xFFFFFF	//16777216L
#define POW_2_26 					0x3FFFFFF	//67108864L
#define POW_2_30 					0x3FFFFFFF	//1073741823L
#define POW_2_31 					0x7FFFFFFF	//2147483648L

#define SPK_FL_BIT 					0x01
#define SPK_CEN_BIT 					0x02
#define SPK_FR_BIT 					0x04
#define SPK_SL_BIT 					0x08
#define SPK_SR_BIT 					0x10
#define SPK_SBL_BIT 					0x20
#define SPK_SBR_BIT 					0x40
#define SPK_SW_BIT 					0x80

enum{
STREAM_NONE = 0,
STREAM_PCM,
STREAM_AC3,
STREAM_DTS,
STREAM_DSD,
};

typedef enum{
CH_L = 0,
CH_C,
CH_R,
CH_LS,
CH_RS,
CH_SW, //LFE
CH_LB,
CH_RB,
CH_ALL,
}Dsp_Channel;

typedef enum{
SUM1 = 0,
SUM2,
SUM3,
SUM4,
SUM5,
}Sum_Channel;

enum{
IRC_TEST_INIT = 0,
IRC_TEST_AND_SETUP_DELAY,
IRC_TEST_AND_SETUP_TRIMS,
IRC_TEST_AND_SETUP_EQ,
IRC_CSS_TEST_D1,
IRC_SPK_CHECK,
IRC_SPK_DISTANCE,
IRC_SPK_LEVEL,
IRC_SPK_SPECTRUM,
};

typedef struct{
unsigned short spk_fl_dtime;
unsigned short spk_c_dtime;
unsigned short spk_fr_dtime;
unsigned short spk_rs_dtime;
unsigned short spk_ls_dtime;
unsigned short spk_sbr_dtime;
unsigned short spk_sbl_dtime;
unsigned short spk_lfe_dtime;
}_DELAY_TIME;

extern _DELAY_TIME dsp_delay_time;
extern _DELAY_TIME dsp_delay_music_time;
#define DELAY_MODULE_MAX_TIME 500

typedef struct{
unsigned char acmode;		/*Current Audio Coding Mode*/
unsigned char dsurmod;		/*Current Dolby Surround Mode*/
unsigned char lfeon;			/*Current lfeon value*/
unsigned char bsid;			/*Current bsid value*/
unsigned char ex_flag;			/*Current Ex flag*/
unsigned short timecod2;
}_AC3_STREAM;

typedef struct{
unsigned char amode;			/*Audio Channel Arrangement*/
unsigned char lfeon;			/*LFF Channel Interpolation factor*/
unsigned char pcmr;			/*Source PCM coding Resolution*/
unsigned char ext_audio_id;	/*Extended Audio Identifier*/
unsigned char ext_audio;		/*Extended Coding Flag*/
unsigned char sfreq;			/*Source Sampling Frequency*/
}_DTS_STREAM;

enum{
_DTS=1,
_DTS_ES_DISCRETE,
_DTS_ES_MATRIX,
_DTS_96_24,
_DTS_HD_MA_48K,         //5
_DTS_HD_MA_48K_2ch,
_DTS_HD_MA_48K_5ch,
_DTS_HD_MA_48K_6ch,
_DTS_HD_MA_48K_7ch,
_DTS_HD_MA_96k_192K,
_DTS_HD_MA_96K_2ch, //10
_DTS_HD_MA_96K_5ch,
_DTS_HD_MA_96K_7ch,
_DTS_HD_MA_192K_2ch, 
_DTS_HD_MA_192K_5ch,
_DTS_HD_MA_192K_7ch, //15
_DTS_HD_HRA_48K,
_DTS_HD_HRA_48K_2ch,
_DTS_HD_HRA_48K_6ch,
_DTS_HD_HRA_48K_7ch,      
_DTS_HD_HRA_48K_8ch,
_DTS_HD_HRA_96K,
_DTS_HD_HRA_96K_2ch,
_DTS_HD_HRA_96K_6ch,
_DTS_HD_HRA_96K_7ch,      
_DTS_HD_HRA_96K_8ch,
};

enum{
_DOLBY_2CH=1,
_DOLBY_5CH,
_DOLBY_EX,
_DOLBY_PLUS,
_DOLBY_TRUE_HD,
_DOLBY_TRUE_HD_2ch,
_DOLBY_TRUE_HD_5ch,
_DOLBY_TRUE_HD_6ch,
_DOLBY_TRUE_HD_7ch,
};

enum{
_PCM_2_1=1,
_PCM_5_1,
_PCM_7_1,
};

enum{
_DSD_2CH=1,
_DSD_Multi_channels,
};

enum{
PCM_MODE_NORMAL = 0,
PCM_MODE_PL2,
PCM_MODE_PL2X,
PCM_MODE_NEO6,
PCM_MODE_CES,
#ifdef _ENABLE_LOGIC7
PCM_MODE_LG7,
#endif
PCM_MODE_2FS,
};

enum{
AC3_MODE_NORMAL = 0,
AC3_MODE_PL2,
AC3_MODE_PL2X,
AC3_MODE_EX_ON,
AC3_MODE_EX_AUTO,
AC3_MODE_EX_OFF,
AC3_MODE_CES_ON,
};

enum{
DTS_MODE_NORMAL = 0,
DTS_MODE_CES_ON,
DTS_MODE_PLIIX, //Only use for DTS-HD
};

extern unsigned char DolbyType;  
extern unsigned char dsp_force_change_code;
extern unsigned char compression_mode;
extern unsigned char enable_eq;
extern char dsp_power_flag;
extern float dsp_eq_table[8][10];
extern float dsp_eq_music_table[8][10];

extern void SetDh2On(unsigned char on);
extern void Set_Prologic2Dimension(unsigned char Dimension);
extern void Set_Prologic2Panorama_On_Or_Off(unsigned char enable);
extern void Set_Prologic2Width(unsigned char width);
extern void DspSetCrossbarEffect(unsigned char mode);
extern void DspDecodeDTS(unsigned char decode_mode);
extern float ReConvert_8_24_Value(long in_value);
extern void EqOn(unsigned char on);
extern void SetLate(unsigned char stream,unsigned char mode);
extern unsigned char GetNormalFreq(void);
extern void DspLoadDefaultSetting(unsigned char mode);
extern void DspProcSpkMode(unsigned char channel,unsigned char mode);
extern void DspProcDelay(unsigned char channel,unsigned short delay_time);
extern void DspProcSignal(unsigned char signal_type, unsigned char channel, unsigned char freq);
extern void DspProcIrc(unsigned char irc_mode,unsigned char test_channel);
extern void DspProcCorner(unsigned char channel,unsigned char corner_value);
extern void DspProcPower(unsigned char mode);
extern void ContextSwitchDSPA(void);
extern void ContextSwitchDSPB(void);
extern void DSP_Unsolicited_Read(void);
extern char DspMasterBoot(void);
extern char HostControlledMasterBoot(unsigned char Decoder_ULD,unsigned char DSP_A_Mid_ULD,unsigned char DSP_B_Mid_ULD,unsigned char Post_Pro_ULD,unsigned char AudioInfo,unsigned char input_channel);
extern void DSPFirmwareWrite(unsigned char wop,unsigned short index,signed long value);
extern void DspDecodePcm(unsigned char decode_mode);
extern void DspDecodeAc3(unsigned char Decode_Mode);
extern void AnalyseAc3Cfg(_AC3_STREAM ac3_stream);
extern void AnalyseDtsCfg(_DTS_STREAM dts_stream);
extern void DspProcUnsolicitedMsg(unsigned char *uCmdBuf);
extern unsigned long ReadDsp(void); 

