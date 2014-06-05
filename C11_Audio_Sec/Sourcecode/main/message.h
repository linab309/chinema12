#ifndef _MESSAGE_H
#define _MESSAGE_H

#define MESSAGE_SIZE 		8
typedef struct {
	TASK_ID t_id;
	unsigned char buffer[MESSAGE_SIZE];
} MSG,*PMSG;

#define MESSAGE_QUEUE_SIZE 200

enum{
MSG_NULL = 0,
MSG_KeyPress,
MSG_KeyHold,
MSG_KeyUp,
MSG_Stream,

MSG_IrcDelay,
MSG_IrcTrims,
MSG_IrcEq,
MSG_Irc,
MSG_Show,

MSG_Flash,
MSG_Dimer,
MSG_RadioSearch,
MSG_RadioPreset,
MSG_RadioFreq,

Msg_Volume,
MSG_Power, 
MSG_InputSource,
MSG_Mute,
MSG_HDRADIO_Text,

MSG_DSP_Setting, //20
MSG_DSP_HDMI_Setting,
MSG_DSP_Channel_Setting,
MSG_Soft_Mute,
}; /*Message type*/

extern unsigned char InitMsgQueue(void);
extern unsigned char PostMessage(TASK_ID t_id,unsigned char *buffer);
extern unsigned char SendMessage(TASK_ID t_id,unsigned char *buffer);
extern unsigned char GetMessage(MSG * pMsg);
extern TASK_RESULT ProcMessage(PMSG pMsg);
#endif

