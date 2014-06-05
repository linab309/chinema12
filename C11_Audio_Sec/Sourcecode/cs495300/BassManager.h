#define NEGATIVE_INFINITE 					-200
#define POSITIVE_INFINITE 					200

#define BASS_ENABLE 						1
#define BASS_DISABLE						0

#define SUB1_SUM_ENABLE 					1
#define SUB1_SUM_DISABLE					0

#define SUB2_SUM_ENABLE					1
#define SUB2_SUM_DISABLE					0

#define PEAK_LIMITER_ENABLE 				1
#define PEAK_LIMITER_DISABLE 				0

#define PEAK_LIMITER_SUB_OUTPUT_ENABLE	0
#define PEAK_LIMITER_LFE_OUTPUT_ENABLE	1

typedef enum{	/*AN246PPB pdf P13*/
BASS_L_HPF = 0,
BASS_C_HPF,
BASS_R_HPF,
BASS_LS_HPF,
BASS_RS_HPF,
BASS_LB_HPF,
BASS_RB_HPF,
BASS_SUM1_LPF,
BASS_SUM2_LPF,
BASS_SUM3_LPF,
BASS_SUM4_LPF,
BASS_SUM5_LPF,
}Bass_Channel;

typedef enum{	/*AN246PPB pdf P13*/
CORNER_FREQUENCY_FULL=0, /*change by cjm 2007-10-11*/
CORNER_FREQUENCY_40HZ,  // 1
CORNER_FREQUENCY_45HZ, // 2	
CORNER_FREQUENCY_50HZ, // 3
CORNER_FREQUENCY_55HZ, // 4
CORNER_FREQUENCY_60HZ, // 5
CORNER_FREQUENCY_65HZ, // 6
CORNER_FREQUENCY_70HZ, // 7
CORNER_FREQUENCY_75HZ, // 8
CORNER_FREQUENCY_80HZ, // 9
CORNER_FREQUENCY_90HZ, // A
CORNER_FREQUENCY_100HZ, // B
CORNER_FREQUENCY_110HZ, // C
CORNER_FREQUENCY_120HZ, // D
CORNER_FREQUENCY_130HZ, // E
CORNER_FREQUENCY_140HZ, // F
CORNER_FREQUENCY_150HZ, // 10
CORNER_FREQUENCY_175HZ, // 11
CORNER_FREQUENCY_200HZ, // 12
CORNER_FREQUENCY_225HZ, // 13
CORNER_FREQUENCY_250HZ, // 14
}Corner_Frequency;

#define MIXER_L_COEF 0x00
#define MIXER_C_COEF 0x08
#define MIXER_R_COEF 0x10
#define MIXER_Ls_COEF 0x18
#define MIXER_Rs_COEF 0x20
#define MIXER_Lb_COEF 0x28
#define MIXER_Rb_COEF 0x30
#define MIXER_LFE0_COEF 0x38

enum{
MIXER_L_OFFSET = 0,
MIXER_C_OFFSET,
MIXER_R_OFFSET,
MIXER_LS_OFFSET,
MIXER_RS_OFFSET,
MIXER_LB_OFFSET,
MIXER_RB_OFFSET,
MIXER_SW_OFFSET,
};

typedef struct{
unsigned char fl_corner;
unsigned char fr_corner;
unsigned char cen_corner;
unsigned char sl_corner;
unsigned char sr_corner;
unsigned char sbl_corner;
unsigned char sbr_corner;
unsigned char sum1_corner;
unsigned char sum2_corner;
unsigned char sum3_corner;
unsigned char sum4_corner;
unsigned char sum5_corner;
}_BASS_CORNER;

extern _BASS_CORNER dsp_bass_corner;

extern unsigned long ConvertDbValue(float db_value);
extern void DSP_BASS_CONTROL(unsigned char Bass_Enable,unsigned char Sub1_Sum_Enable,unsigned char Sub2_Sum_Enable,unsigned char Peak_Limiter_Enable,unsigned char Peak_Limiter_Select);
extern void DSP_BASS_MGR_5XOVER_CONTROL(unsigned long value);
extern void DSP_BASS_MGR_HPF_CONTROL(unsigned long value);
extern void DSP_SET_BASS_CORNER(Bass_Channel channel,unsigned char freq);
extern void DSP_BASS_INPUT_LEVEL(Dsp_Channel channel,float level);
extern void DSP_BASS_LFE_OUTPUT_LEVEL(Dsp_Channel channel,float level);
extern void DSP_BASS_FRONT_SUM_OUTPUT_LEVEL(Dsp_Channel channel,float level);
extern void DSP_BASS_BACK_SUM_OUTPUT_LEVEL(Dsp_Channel channel,float level);
extern void DSP_BASS_SUM1_INPUT_LEVEL(Dsp_Channel channel,float level);
extern void DSP_BASS_SUM2_INPUT_LEVEL(Dsp_Channel channel,float level);
extern void DSP_BASS_SUM3_INPUT_LEVLE(Dsp_Channel channel,float level);void DSP_BASS_SUM4_INPUT_LEVLE(Dsp_Channel channel,float level);
extern void DSP_BASS_SUM5_INPUT_LEVLE(Dsp_Channel channel,float level);
extern void DSP_BASS_X_INPUT_LEVEL_FRONT(Sum_Channel channel,float level);
extern void DSP_BASS_X_INPUT_LEVEL_BACK(Sum_Channel channel,float level);
extern void DSP_BASS_INPUT_SUB_LEVEL(float level);
extern void DSP_BASS_FRONT_SUM_OUTPUT_SUB_LEVEL(float level);
extern void DSP_BASS_BACK_SUM_OUTPUT_SUB_LEVEL(float level);

extern void ConfigSpkMode(void);
extern void C11SetBass(void);
extern void BassOn(unsigned char on);
extern void SetBassManagerSpeaker(void);

