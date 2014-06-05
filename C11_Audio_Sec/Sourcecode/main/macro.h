#ifndef _MACRO_H
#define _MACRO_H

typedef	char	int8;
typedef	unsigned char	uint8;
typedef	unsigned char	uchar;
typedef	unsigned int	uint;
typedef	short int	int16;
typedef	unsigned short uint16;
typedef	long int	int32;
typedef	unsigned long uint32;

typedef	unsigned char	byte;
typedef	unsigned char	BYTE;
typedef	unsigned int	WORD;
typedef	unsigned long DWORD;

typedef	unsigned long U64;

typedef	unsigned char KEYT;
typedef	unsigned char TASK_ID;
typedef	unsigned char TASK_RESULT;
typedef unsigned char BOOL;

typedef unsigned char R_STATUS;
typedef unsigned int FREQT;

typedef short SETUP_T;
typedef unsigned char SETUP_MODE_T;
typedef unsigned char SETUP_VALUE_T;

#define SUCCESS 0
#define FAILURE	1

#define TRUE 1
#define FALSE 0

#define ON 	1
#define OFF 	0

#ifndef NULL
#define NULL 0
#endif

#define ARRAY_SIZE(m)  (sizeof(m)/sizeof((m)[0]))

#define METER2FEET (3.3)

#define MAX_VOLUME 106
#define MAX_DIMER 3

extern void ProcDimerOff(KEYT key);

#define EQ_0 	0x00
#define EQ_1 	0x10
#define EQ_2 	0x20
#define EQ_3 	0x30
#define EQ_4 	0x40
#define EQ_5 	0x50
#define EQ_6 	0x60
#define EQ_7 	0x70
#define EQ_8 	0x80
#define EQ_9 	0x90
#endif

