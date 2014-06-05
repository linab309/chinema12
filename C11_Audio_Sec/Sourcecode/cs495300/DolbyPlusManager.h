extern void DSP_EC3_Control(unsigned char EC3_enable,unsigned char LFE_enable,unsigned char LFE2_enable,unsigned char Transcoded_enable,unsigned char compressed_enable);
extern void DSP_EC3_Stereo_Mode_Control(unsigned char mode);
extern void DSP_EC3_Compression_Control(unsigned char mode);
extern void DSP_EC3_CUT_X(unsigned long value);
extern void DSP_EC3_BOOST_Y(unsigned long value);
extern unsigned long DspReadEC3(unsigned char index);

