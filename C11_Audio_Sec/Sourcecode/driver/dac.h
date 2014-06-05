#ifndef _DAC_H
#define _DAC_H

enum{
DSD_CS1=0,
DSD_CS2,
DSD_CS3,
DSD_CS4,
DSD_CS0,
DSD_ALL_None,
DSD_ALL_Select=8,  
};

extern void DacMute(int on);
extern void InitDACReg(void);
extern void Auto_Setup_DAC_Phase(void);
extern void DacInvert(int mode,unsigned char channel);
extern void InitDSDMode(unsigned char mode);
#endif

