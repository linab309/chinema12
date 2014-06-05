typedef enum{	/*AN246MPM, pdf P3*/
Pro_Logic_Emulation_Mode=0,
Virtual_Mode,
Music_Mode,
Movie_Mode,
Matrix_Mode,
Custom_Mode,
}Emulation_Mode;

extern void DSP_PLII_Control(unsigned char enable);
extern void DSP_PLII_DECODE_MODE(unsigned char mode);
extern void DSP_PLII_CONTROL_REGISTER(unsigned char Panorama_Enable);
extern void DSP_PLII_CENTER_WIDTH_CONFIGURATION(unsigned char Width);
extern void DSP_PLII_DIMENSION_CONFIGURATION(unsigned char Dimension);
extern void DSP_PLII_Firmware_Cfg(void);

