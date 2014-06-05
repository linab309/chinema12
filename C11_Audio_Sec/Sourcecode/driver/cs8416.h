#ifndef _CS8416_H
#define _CS8416_H

#define REG8416_CNTL0				0x0
#define REG8416_CNTL1				0x1
#define REG8416_CNTL2				0x2
#define REG8416_CNTL3				0x3
#define REG8416_CNTL4				0x4
#define REG8416_SER_AUD_FMT 		0x5
#define REG8416_REC_ERR_MASK 		0x6
#define REG8416_INTER_MASK 		0x7
#define REG8416_INTER_MODE_MSB 	0x8
#define REG8416_INTER_MODE_LSB 	0x9
#define REG8416_DETECT_STATUS		0x0B
#define REG8416_RECV_ERR 			0x0c
#define REG8416_CLK_RATIO 			0x18
#define REG8416_ID_AND_VERSION 	0x7F

#define FREQ_48KHZ 					0x00
#define FREQ_44_1KHZ 				0x01
#define FREQ_32KHZ  					0x02
#define FREQ_96KHZ 					0x04
#define FREQ_88_2KHZ 				0x05
#define FREQ_64KHZ 					0x06
#define FREQ_192KHZ 				0x0C
#define FREQ_176_4KHZ 				0x0D
#define FREQ_128KHZ 				0x0E

extern void InitCs8416(void);
extern unsigned char Cs8416GetFreq(void);
extern unsigned char IsCs8416Unlock(void);
extern unsigned char Cs8416ReadByte(unsigned char addr);
extern unsigned char CheckCs8416Unlock(unsigned char times);
extern void Cs8416WriteByte(unsigned char addr,unsigned char value);
extern unsigned char IsCs8416Pcm(void);
#endif

