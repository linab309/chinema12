#ifndef _VFD_DISP_H
#define _VFD_DISP_H

#define VFD_LINE1_START_POSITION 0
#define VFD_LINE2_START_POSITION 20
#define VFD_MAX_POSITION 40
#define VFD_LINE1_LENGTH 20
#define VFD_LINE2_LENGTH 20

enum{
VFD_LINE1,
VFD_LINE2,
VFD_LINE_ALL,
};

enum
{
VFD_UPDATE_NONE = 0,
VFD_UPDATE_LINE1,
VFD_UPDATE_LINE2,
VFD_UPDATE_LINE_ALL,
};

#define VFD_MAX_POSITION 40


#define VFD_BUFFER_SUCESS 0
#define VFD_BUFFER_LENGTH_ERROR 1

extern const char cary_logo[];
extern const char Surround_Processor[];

extern const char Cary_Logo[];
extern const char Product_Name[];

extern unsigned char need_refresh_vfd;
extern unsigned char need_refresh_dimer;
extern short temp_dimer_time;

extern void VfdTimerHandler(void);
extern void VfdRefresh(void);
extern void SetVfdOwner(unsigned char);

extern unsigned char VfdDisplay(unsigned char start_position,unsigned char mode,const char *format,...);
extern void VfdDisplayBlank(unsigned char line,unsigned char update);

extern unsigned char VfdTempDisp(unsigned char start_position,unsigned char mode,unsigned short time, const char *format,...);
extern void VfdTempDispBlank(unsigned char line,unsigned char update);
extern char GetVfdbufferChar(unsigned char position);

#endif

