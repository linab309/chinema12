#ifndef _MAS9116_H
#define _MAS9116_H

#define MAS9116_CHIP_2CH 			0x00
#define MAS9116_CHIP_ZONE2 		0x01
#define MAS9116_CHIP_7CH1_F 		0x02
#define MAS9116_CHIP_7CH1_SF 		0x03
#define MAS9116_CHIP_7CH1_CEN_SW 0x04
#define MAS9116_CHIP_7CH1_SB 		0x05

#define MAS9116_LEFT 				0x10
#define MAS9116_RIGHT 				0x20
#define MAS9116_BOTH 				0x30

extern void Mas9116SetGain(unsigned char chip_channel,float vol);
extern void Mas9116Init(void);
#endif

