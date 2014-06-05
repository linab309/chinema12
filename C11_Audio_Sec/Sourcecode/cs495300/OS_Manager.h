enum{
Stereo,					//0X00
Left_Mono,				//0X01
Right_Mono,				//0X10
Mixed_Mono,			//0X11
};

enum{
Lt_Rt, 					//0x00
C,						//0x01
L_R,						//0x02
L_C_R,					//0x03
L_R_S,					//0x04
L_C_R_S,				//0x05
L_R_Ls_Rs,				//0x06
L_C_R_LS_RS,			//0x07
L_C_R_Ls_Rs_Cs,			//0x08
L_C_R_Ls_Rs_Sbl_Sbr,	//0x09
L_R_Ls_Rs_Cs,			//0x0A
L_R_Ls_Rs_Sbl_Sbr,		//0X0B
};

enum{
_48k,					//0x00
_44_1K,					//0x01
_32k,					//0x02
_Reserved1,				//0x03
_96k,					//0x04
_88_2k,					//0x05
_64k,					//0x06
_Reserved2,				//0x07
_24k,					//0x08
_22_05k,				//0x09
_16k,					//0x0A
_Reserved3,				//0x0B
_192k,					//0x0C
_176_4k,				//0x0D
_128k,					//0x0E
};

enum{
_12_288MHZ,			//0X0000
_24_576MHZ,			//0X0001
};

enum{
_100MHZ,				//0X00
_120MHZ,				//0X01
_151_5750MHZ,			//0X10
};

enum{
Custom,					// 0
Standard,				// 1
};

extern void OS_IO_CONFIG_Control(unsigned char channel);
extern void OS_OUTPUT_MODE_CONTROL(unsigned char Karaoke,unsigned char dual_mode,unsigned char output_mode);
extern void OS_SAMPLE_RATE_CONTROL(unsigned char rate);
extern void OS_AUTODETECT_Config(unsigned char ACCN,unsigned char bypass,unsigned char autodetection);
extern void OS_PCM_AUTOSWITCH_ENABLE(unsigned char enable);
extern void OS_OUTPUT_MODE_CONTROL_FOR_DSPB(unsigned char Karaoke,unsigned char dual_mode,unsigned char output_mode);
extern void OS_DSPB_OUTPUT_MODE_ENABLE(unsigned char enable);
extern void OS_PLL_STANDARD_CONFIG(unsigned char clock_freq,unsigned char Speed_Grade_Select,unsigned char PLL_Config);
extern void DSP_0S_Firmware_Cfg(unsigned char channel_mode);

