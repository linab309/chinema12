#ifndef _KEY_TAB_H
#define _KEY_TAB_H

#define _IR_VOL_UP_KEY 0x10
#define _IR_VOL_DOWN_KEY 0x11
#define _IR_ARROW_DOWN_KEY 0x21
#define _IR_ARROW_UP_KEY 0x23
#define _IR_ARROW_LEFT_KEY 0x32
#define _IR_ARROW_RIGHT_KEY 0x2A

extern void DeleteRemoteRepeatKey(unsigned char key);
extern BOOL AddRemoteRepeatKey(unsigned char key);
extern void InitRemoteRepeatKey(void);

extern const KEYT zone1_remote_key_table[];
extern const KEYT zone2_remote_key_table[];
#endif

