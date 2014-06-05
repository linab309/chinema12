#ifndef _KEY_TASK_H
#define _KEY_TASK_H

enum{
KEY_NULL,
KEY_OK,
KEY_TMO,	/*time over flow*/
};

extern uchar key_enable;
#define WAIT_KEY_VALID 0
#define GET_KEY_ONLY 1
extern uchar ReadKey(KEYT * key,unsigned char mode,unsigned short timer);
#endif

