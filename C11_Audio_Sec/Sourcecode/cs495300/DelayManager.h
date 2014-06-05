extern void DSP_AV_Delay(unsigned char Delay_time);
extern void DSP_MAIN_PPM_CONTROL(unsigned char Delay_PPM_Master_Enable,unsigned char MidProc_Delay_Offset_Enable,unsigned char Externnal_Memory_Enable);
extern void DSP_Delay_Unit(unsigned char Dly_Unit_Select);
extern void DSP_Delay_Max_Setting(Dsp_Channel channel,unsigned long Max_Delay_Time);
extern void DSP_EXT_BUFF_ADDR(unsigned long Externnal_Buff_Addr);
extern void DSP_EXT_BUFF_SIZE(unsigned long External_Buff_Size);
extern void DSP_Delay_Channel_Setting(Dsp_Channel channel,unsigned long Delay_Ms);
extern void DSP_Delay_Firmware_Init(void);
extern unsigned long DspReadDelay(unsigned char index);

