#ifndef _RS232_PROTOCOL_H
#define _RS232_PROTOCOL_H

enum{
MSG_IDLE = 0x0,
MSG_RECEIVING,
};

#define STX '@'
#define ETX 0x0D

typedef BOOL (*cmd_proc)(unsigned char *,unsigned char);

typedef struct{
	char *cmd;
	cmd_proc run;
}MSG_T;

#endif

