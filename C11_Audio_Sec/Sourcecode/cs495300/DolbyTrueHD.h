enum{
decode_no_ch,
decode_2chs,
decode_6chs,
decode_6and2_chs,
decode_8_chs,
decode_8and2_chs,
};

enum{
Disable_dynamic_range,
Follow_the_bitstream,
Force_dynamic_range, 
};

extern void DSP_TRUEHD_Control(unsigned char TrueHD_Enable);
extern void DSP_TRUEHD_Decode_Mode_Control(unsigned char channel);
extern void DSP_TRUEHD_DRC_Control(void);
extern void DSP_TRUEHD_Cut_Scalefactor(unsigned long value);
extern void DSP_TRUEHD_Boost_Scalefactor(unsigned long value);
extern void DSP_TRUEHD_DRC_INIT(void);
extern unsigned long DspReadTRUEHD(unsigned char index);

