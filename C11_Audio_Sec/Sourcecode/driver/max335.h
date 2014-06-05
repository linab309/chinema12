#ifndef _MAX335_H
#define _MAX335_H

#define MAX335_CHIP_NUMBER 	8 

extern void SelInput(unsigned char num);
extern void SetMax335Switch(unsigned int switch_value,unsigned char on);
extern void UpdateMax335(void);
#endif

