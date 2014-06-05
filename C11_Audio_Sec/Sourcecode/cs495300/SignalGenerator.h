typedef enum{	/*AN246DH pdf P4*/
Signal_White_Noise=0,
Signal_Dolby_Pink_Noise,
Signal_THX_Pink_Noise,
Signal_Bass_Noise,
Signal_Sine_Wave,
Signal_Square_Wave,
Signal_DC_Wave,
Signal_Impulse_Wave,
Signal_Stop, //add 
}SIGNAL_TYPE;

extern void DSP_SGEN_ENABLE(unsigned char mode);
extern void DSP_SGEN_CHANNEL_ENABLE(unsigned long value);
extern void DSP_CHANNEL_SIGNAL_SELECT(Dsp_Channel channel,unsigned char signal_type);

