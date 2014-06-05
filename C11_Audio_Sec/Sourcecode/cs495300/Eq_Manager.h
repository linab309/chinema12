#define EQ_8_Channel_Enable		0x00FF	/*AN288PPE P6 Table 2-4 Channel Selection Bits*/
#define Direct_Coefficient_Mode	0
#define True_Parametric_Mode	1

typedef enum{	/*AN288PPE pdf P6*/
EQ_CH_L= 0,
EQ_CH_C,
EQ_CH_R,
EQ_CH_LS,
EQ_CH_RS,
EQ_CH_LB,
EQ_CH_RB,
EQ_CH_LFE0,
EQ_CH_LFE1,
EQ_CH_LFE2,
EQ_CH_LFE3,
EQ_CH_Reserve,
EQ_CH_LD, 
EQ_CH_RD,
EQ_CH_LA,
EQ_CH_RA,
}EQ_Channel;

extern unsigned char Movie_Music_Speaker_Flag; 
extern void EqOn(unsigned char on);
extern void DSP_Eq_Control(unsigned char EQ_Global_Enable,unsigned char EQ_Global_Reset,unsigned long EQ_Channel_Enable);
extern void DSP_Eq_Config(unsigned char mode);
extern void DSP_Eq_Channels(unsigned long Eq_Channel_Configuration);
extern void DSP_Eq_Bands(unsigned char bands);
extern void EqModuleconfig(unsigned char channel,unsigned char band);
extern void DSP_EQ_Firmware_Init(void); 
extern unsigned long CALC_EQ_FC(float HZ_Value);
extern unsigned long CALC_EQ_Q(float Q_Value);
extern signed long CALC_EQ_G(float G_Value);
extern void DSP_EQ_CHANGER(unsigned char index);
extern void DSP_Eq_Flash(unsigned char Eq_Flash_Restore,unsigned char Eq_Flash_Save);
extern void DspReadEq(unsigned char index);
																										   
