/* valid index values for DSP Manager API, indexes 7,8,9, and A, bits [15:0] */
typedef enum uld_id_t {
  ULD_ID_INVALID=0,
  ULD_ID_OS_HD=1,
  ULD_ID_OS_HD2=2,
  ULD_ID_DDPLUS=3,
  ULD_ID_TRUEHD=4,
  ULD_ID_DTS_MA_48=5,
  ULD_ID_DTS_MA_96=6,
  ULD_ID_DTS_MA_192=7,
  ULD_ID_DTS_MA_NBC_48=8,
  ULD_ID_DTS_MA_NBC_96=9,
  ULD_ID_DTS_MA_NBC_192=10,
  ULD_ID_DTS_HRA=11,
  ULD_ID_DTS_LBR=12,
  ULD_ID_AC3=13,
  ULD_ID_DTS_ES=14,
  ULD_ID_SGEN=15,
  ULD_ID_CROSSBAR=16,
  ULD_ID_PL2X=17,
  ULD_ID_PL2X_B=18,
  ULD_ID_NEO6=19,
  ULD_ID_SETUP_XBAR=20,
  ULD_ID_DVS2=21,
  ULD_ID_DH2=22,
  ULD_ID_IRC=23,
  ULD_ID_CROSSBAR_B=24,
  ULD_ID_APPLY_CROSSBAR_B=25,
  ULD_ID_SPP=26,
  ULD_ID_APP=27,
  ULD_ID_AM=28,
  ULD_ID_NUMBER=28,
  ULD_ID_DONTCARE=0xffff
};
/* valid index values for DSP Manager API, indexes 7,8,9, bits [31:16] */
typedef enum mode_id_t {
  MODULE01_MODE_1=1,
  MODULE01_MODE_2=2,
  MODULE01_MODE_3=3,
  MODULE01_MODE_4=4,
  MODULE01_MODE_5=5,
  MODULE01_MODE_6=6,
  MODULE01_MODE_7=7,
  MODULE01_MODE_8=8,
  MODULE01_MODE_9=9,
  MODULE01_MODE_A=10,
  MODULE01_MODE_B=11,
  MODULE01_MODE_C=12,
  MODULE02_MODE_1=1, //ADD from Cirrus logic
  MODULE57_MODE_1=1,
  DDPLUS_MODE_DEFAULT=1,
  DDPLUS_MODE_STEREO=2,
  TRUEHD_MODE_DEFAULT=1,
  TRUEHD_MODE_OUTMODE=2,
  DTS_MA_48_MODE_DEFAULT=1,
  DTS_MA_96_MODE_DEFAULT=1,
  DTS_MA_192_MODE_DEFAULT=1,
  DTS_MA_NBC_48_MODE_DEFAULT=1,
  DTS_MA_NBC_96_MODE_DEFAULT=1,
  DTS_MA_NBC_192_MODE_DEFAULT=1,
  DTS_HRA_MODE_DEFAULT=1,
  DTS_HRA_MODE_X96K_ENABLED=2,
  DTS_LBR_MODE_DEFAULT=1,
  AC3_MODE_HI=1,
  AC3_MODE_LOW=2,
  AC3_MODE_MID=3,
  AC3_MODE_MCH_PCM_2=4,
  AC3_MODE_L0R0=5,
  AC3_MODE_MCH_PCM=6,
  DTS_ES_MODE_DTS=1,
  DTS_ES_MODE_DTS9624=2,
  DTS_ES_MODE_MATRIX=3,
  DTS_ES_MODE_DISCRETE=4,
  SGEN_MODE_SGEN=1,
  CROSSBAR_MODE_MAIN_ENABLE=1,
  CROSSBAR_MODE_MAIN_DZ_ENABLE=2,
  CROSSBAR_MODE_COEF_L_R_7CH=3,
  CROSSBAR_MODE_COEF_L_C_R_7CH=4,
  CROSSBAR_MODE_COEF_L_R_LS_RS_7CH=5,
  CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH=6,
  CROSSBAR_MODE_COEF_L_R_5CH=7,
  CROSSBAR_MODE_COEF_L_C_R_5CH=8,
  CROSSBAR_MODE_COEF_L_R_LS_RS_5CH=9,
  PL2X_MODE_MOVIE=1,
  PL2X_MODE_MUSIC=2,
  PL2X_MODE_MC_MOVIE=3,
  PL2X_MODE_MC_MUSIC=4,
  PL2X_MODE_DD_EX=5,
  PL2X_B_MODE_MOVIE=1,
  PL2X_B_MODE_MUSIC=2,
  PL2X_B_MODE_MC_MOVIE=3,
  PL2X_B_MODE_MC_MUSIC=4,
  PL2X_B_MODE_DD_EX=5,
  NEO6_MODE_CINEMA=1,
  NEO6_MODE_MUSIC=2,
  NEO6_MODE_MATRIX=3,
  DVS2_MODE_REFERENCE=1,
  DVS2_MODE_WIDE=2,
  DVS2_MODE_REFERENCE_HEAPMODE1=3,
  DVS2_MODE_WIDE_HEAPMODE1=4,
  DH2_MODE_BYPASS=1,
  DH2_MODE_DH2=2,
  DH2_MODE_BYPASS_HEAPMODE1=3,
  DH2_MODE_DH2_HEAPMODE1=4,
  IRC_MODE_IRC_MODE1=1,
  CROSSBAR_B_MODE_MAIN_DZ_ENABLE=1,
  APPLY_CROSSBAR_B_MODE_DZ_ENABLE=1,
  APPLY_CROSSBAR_B_MODE_MAIN_DZ_ENABLE=2,
  MODE_DONTCARE=0xffff
};

enum{
boot_idle,						
change_to_new_config,			
save_for_power_down,			
shut_down,						
save_para,						
};

enum{
Spdif_AutoSwitch,
HDMI_AutoSwitch,
Multi_Channel_Analog,
DSD,
Spdif_No_AutoSwitch,
HDMI_No_Autoswitch,
Flash_Update,
};

enum{
Fs_128,
Fs_256,
Fs_512,
};

enum{
FS_48,
FS_44_1,
FS_32,
FS_96,
FS_88_2,
FS_64,
FS_24,
FS_22_05,
FS_16,
FS_192,
FS_176_4,
FS_128,
};

#define configuration_lock 	1
#define configuration_unlock 	0

extern void DSP_BOOT(unsigned char config_lock,unsigned char Boot_Action);
extern void DSP_CFG_OUTPUT_FS(unsigned char Freq);
extern void DSP_CFG_AUDIO_SRC(unsigned char Input_Source);
extern void DSP_CFG_DECODER(unsigned char mode,unsigned char decoder_type);
extern void DSP_CFG_MATRIX(unsigned char mode,unsigned char matrix_type);
extern void DSP_CFG_VIRTUALIZER(unsigned char mode,unsigned char virtualizer_type);
extern void DSP_CFG_PPM(unsigned char PPM_type);
extern void DSP_CFG_PPM_Mode(unsigned char Module2_mode,unsigned char Module1_mode);
extern void DSP_CFG_MCLK_FACTOR(unsigned char Freq);
extern void Change_DSP_Decoder_Code(unsigned char mode,unsigned char Decode_type);
extern void Change_DSP_MATRIX_Code(unsigned char mode,unsigned char matrix_type);
extern void Change_DSP_VIRTUALIZER_Code(unsigned char mode,unsigned char virtualizer_type);
extern void Change_DSP_PPM_Code(unsigned char PPM_type);
extern void Change_Input_Audio_Source(unsigned char Source);
extern void DSP_Update_Flash(void);
extern unsigned long DSP_API_Read(unsigned char index);

