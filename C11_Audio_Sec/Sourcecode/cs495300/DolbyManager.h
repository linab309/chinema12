enum{
Auto,	//0x00
LtRt,	//0x01
LoRo,	//0x10
};

enum{
Custom_Mode0,			//0x00
Custom_Mode1,			//0x01
Line_Out_Mode,			//0x10
RF_Remodulation_Mode,	//0x11
};

extern void DSP_AC3_Control(unsigned char LFE_Enable,unsigned char AC3_Enable);
extern void DSP_AC3_STEREO_MODE_CONTROL(unsigned char Stereo_Mode);
extern void DSP_AC3_COMPRESSION_CONTROL(void);
extern void DSP_AC3_CUT_X(unsigned long value);
extern void DSP_AC3_BOOST_Y(unsigned long value); 
extern void DSP_AC3_DRC_INIT(void);
extern  unsigned long DspReadDolby(unsigned char index);

