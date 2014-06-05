/************Input Configuration*************************/
/*********************Input A*************************/
extern void DSP_Cfg_Input_I2s(void);
extern void DSP_Cfg_Input_Left_Justify(void);
extern void DSP_Cfg_Input_Data_Format(int type);

/*********************Input B*************************/
extern void DSP_Cfg_input_SCLK_Rising_Edge(void);
extern void DSP_Cfg_input_SCLK_Falling_Edge(void);
extern void DSP_Cfg_Input_SCLK_Polarity(int Polarity);

/*********************Input C*************************/
extern void DSP_Cfg_Input_LRCLK_Left_Polarity(void);
extern void DSP_Cfg_Input_LRCLK_Right_Polarity(void);
extern void DSP_Cfg_Input_LRCLK_Polarity(int Polarity);

/*********************Input D*************************/
extern void DSP_Cfg_Input_On_DAI_D4(void);
extern void DSP_Cfg_Input_On_DAI_D0(void);
extern void DSP_Cfg_Input_DAI_Mode(int mode);


/************Output Configuration*************************/
/*********************Output A*************************/
extern void DSP_Cfg_Output_Clock_Mode0(void);
extern void DSP_Cfg_Output_Clock_Mode1(void);
extern void DSP_Cfg_Output_Clock_Mode(int mode);

/*********************Output B*************************/
extern void DSP_Cfg_Output_Clock_Relationship0(void);
extern void DSP_Cfg_Output_Clock_Relationship1(void);
extern void DSP_Cfg_Output_Clock_Relationship(int Relation);

/*********************Output C*************************/
extern void DSP_Cfg_Output_SCLK_Freq96k(void);
extern void DSP_Cfg_Output_SCLK_Freq192k(void);
extern void DSP_Cfg_Output_Clock_Frequence(int Fre);

/*********************Output D*************************/
extern void DSP_Cfg_Output_I2S(void);
extern void DSP_Cfg_Output_Left_Justify(void);
extern void DSP_Cfg_Output_Data_Format(int Format);

/*********************Output E*************************/
extern void DSP_Cfg_Output_LRCLK_Polarity0(void);
extern void DSP_Cfg_Output_LRCLK_Polarity1(void);
extern void DSP_Cfg_Output_LRCLK_Polarity(int Polarity);

/*********************Output F*************************/
extern void DSP_Cfg_Output_SCLK_Polarity0(void);
extern void DSP_Cfg_Output_SCLK_Polarity1(void);
extern void DSP_Cfg_Output_SCLK_Polarity(int Polarity);

extern void DSPInputOutputCfg(unsigned char channel_mode);

