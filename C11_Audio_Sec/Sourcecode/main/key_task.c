#include "macro.h"

#include "..\driver\ic165.h"
#include "..\driver\ir.h"
#include "..\driver\ioport.h"

#include "message.h"
#include "task.h"
#include "key_task.h"
#include "keydefine.h"
#include "key_tab.h"
#include "queue.h"
#include "debug.h"
#include "radio_task.h"
#include "channel.h"
#include <stdlib.h>
#include <string.h>

#ifdef _USE_ARTX
#define _KEY_USE_TIME_TICK
#endif
typedef struct {
unsigned char key_value;
unsigned char key_mode;
}_PANEL_KEY,*_pPANEL_KEY;

#define KEY_NONE 	0
#define KEY_PRESS 	1
#define KEY_HOLD 	2
#define KEY_UP 		3

#define _NEW_KEY_PRESS_TIME 	7
#define _KEY_HOLD_TIME 			10
enum{
_TYPE_PANEL_KEY,
_TYPE_ZONE1_REMOTE_KEY,
_TYPE_ZONE2_REMOTE_KEY,
_TYPE_RS232_KEY,
};

unsigned char key_enable = 1;

#if 0
#define KEY_QUEUE_SIZE 10
KEY_TASK_MSG * pKeyQueue;
unsigned char key_queue_rear;
unsigned char key_queue_front;

/******************************************************************************
	Name:	InitKeyMsgQueue

	Description:	初始化按键队列

	Inputs:	NULL

	Outputs:	TRUE--创建队列成功
			FALSE--创建队列失败

	notes:
 *****************************************************************************/
unsigned char InitKeyMsgQueue()
{
	if(!pKeyQueue)
		{
		pKeyQueue = (KEY_TASK_MSG *) malloc(KEY_QUEUE_SIZE * sizeof(KEY_TASK_MSG));	/*size = KEY_QUEUE_SIZE * 4; */
		if(!pKeyQueue)
			return FALSE;
		key_queue_rear = key_queue_front = 0;
		}
	return TRUE;
}

/*
 *检测按键队列是否为空
 */
unsigned char IsKeyMsgQueueEmpty()
{
	if(key_queue_rear == key_queue_front)
		{
		return TRUE;
		}
	return FALSE;
}

unsigned char IsKeyMsgQueueFull()
{
	if((key_queue_rear+1)%KEY_QUEUE_SIZE == key_queue_front)
		{
		return TRUE;
		}
	return FALSE;
}

unsigned char InKeyMsgQueue(uchar key_type,uint16 key_value)
{
	if(IsKeyMsgQueueFull())
		{
		assert(IsKeyMsgQueueFull());
		return FALSE;
		}
	key_queue_rear = (key_queue_rear+1)%KEY_QUEUE_SIZE;
	pKeyQueue[key_queue_rear].key_type = key_type;
	pKeyQueue[key_queue_rear].key_value = key_value;
	return TRUE;
}

unsigned char GetKeyMsgQueue(KEY_TASK_MSG ** message)
{
	if(! IsKeyMsgQueueEmpty())
		{
		key_queue_front = (key_queue_front + 1) % KEY_QUEUE_SIZE;
		*message = pKeyQueue + key_queue_front;
		return TRUE;
		}
	return FALSE;
}

unsigned char SendKeyMsg(unsigned char type,KEYT value)
{
	if(InKeyMsgQueue(type,value))
		return TRUE;
	return FALSE;
}
#endif

typedef unsigned int PANEL_KEY_TYPE;
short panel_key_value = _NO_KEY;

struct PanelKey{
PANEL_KEY_TYPE ic165_value;
KEYT key;
};


#if 0
const struct PanelKey panel_key_table[] = 
{
{0x00000001,_PANEL_ARROW_UP_KEY},		/*1*/	/*up*/
{0x00000002,_PANEL_ARROW_LEFT_KEY},		/*2*/	/*left*/
{0x00000004,_PANEL_ENTER_KEY},			/*3*/	/*enter*/
{0x00000008,_PANEL_ARROW_RIGHT_KEY},	/*4*/	/*right*/
{0x00000010,_PANEL_ARROW_DOWN_KEY},	/*5*/	/*down*/
{0x00000040,_PANEL_POWER_KEY},			/*7*/	/*power*/
{0x00000100,_PANEL_INPUT1_KEY},			/*9*/	/*XLR*/
{0x00000200,_PANEL_INPUT2_KEY},			/*10*/	/*DVD1*/
{0x00000400,_PANEL_INPUT3_KEY},			/*11*/	/*DVD2*/
{0x00000800,_PANEL_INPUT4_KEY},			/*12*/	/*CD*/
{0x00001000,_PANEL_AM_FM_KEY},			/*13*/	/*AM_FM*/
{0x00002000,_PANEL_HD_RADIO_KEY},		/*14*/	/*HDRADIO*/
{0x00004000,_PANEL_PRESET_DN_KEY},		/*15*/	/*preset -*/
{0x00008000,_PANEL_PRESET_UP_KEY},		/*16*/	/*preset +*/
{0x00010000,_PANEL_PLIIX_KEY},			/*17*/	/*pro logic IIx*/
{0x00020000,_PANEL_7CH1_INPUT_KEY},		/*18*/	/*7.1CH In*/
{0x00040000,_PANEL_HDMI1_KEY},			/*19*/	/*HDMI1*/
{0x00080000,_PANEL_HDMI2_KEY},			/*20*/	/*HDMI2*/ 
{0x00100000,_PANEL_INPUT5_KEY},			/*21*/	/*D-VCR*/
{0x00200000,_PANEL_INPUT6_KEY},			/*22*/	/*VCR1*/
{0x00400000,_PANEL_INPUT7_KEY},			/*23*/	/*VCR2*/
{0x00800000,_PANEL_INPUT8_KEY},			/*24*/	/*SAT/DBS*/
{0x01000000,_PANEL_add_0,},				/*25*/
{0x02000000,_PANEL_add_1,},				/*26*/
{0x04000000,_PANEL_add_2,},				/*27*/
{0x08000000,_PANEL_add_3,},				/*28*/
{0x10000000,_PANEL_SUR_MODE_KEY},		/*29*/ 	/*sur. mode*/
{0x20000000,_PANEL_NEO6_KEY}, 			/*30*/	/*neo:6*/
{0x40000000,_PANEL_CES7_1_KEY}, 			/*31*/	/*ces 7.1*/
{0x80000000,_PANEL_add_4},				/*32*/			
{0x00000041,_PANEL_UPDATE_KEY,},			/*up + power*/
{0,0}
};
#else // JIAN ADD IN 10-09-13
const struct PanelKey panel_key_table[] = 
{
{0x00000001,_PANEL_ARROW_UP_KEY},		/*1*/	/*up*/
{0x00000002,_PANEL_ARROW_LEFT_KEY},		/*2*/	/*left*/
{0x00000004,_PANEL_ENTER_KEY},			/*3*/	/*enter*/
{0x00000008,_PANEL_ARROW_RIGHT_KEY},	/*4*/	/*right*/
{0x00000010,_PANEL_ARROW_DOWN_KEY},	/*5*/	/*down*/
{0x00000040,_PANEL_POWER_KEY},			/*7*/	/*power*/
{0x00000100,_PANEL_INPUT1_KEY},			/*9*/	/*XLR*/
{0x00000200,_PANEL_INPUT2_KEY},			/*10*/	/*DVD1*/
{0x00000400,_PANEL_INPUT3_KEY},			/*11*/	/*DVD2*/
{0x00000800,_PANEL_INPUT4_KEY},			/*12*/	/*CD*/
{0x00001000,_PANEL_HDMI1_KEY},			/*13*/	/*AM_FM*/
{0x00002000,_PANEL_HDMI2_KEY},		/*14*/	/*HDRADIO*/
{0x00004000,_PANEL_HDMI3_KEY},		/*15*/	/*preset -*/
{0x00008000,_PANEL_HDMI4_KEY},		/*16*/	/*preset +*/
{0x00010000,_PANEL_PLIIX_KEY},			/*17*/	/*pro logic IIx*/
{0x00020000,_PANEL_7CH1_INPUT_KEY},		/*18*/	/*7.1CH In*/
{0x00040000,_PANEL_RADIO_KEY},			/*19*/	/*HDMI1*/
{0x00080000,_PANEL_TVSOUND_KEY},			/*20*/	/*HDMI2*/ 
{0x00100000,_PANEL_INPUT5_KEY},			/*21*/	/*D-VCR*/
{0x00200000,_PANEL_INPUT6_KEY},			/*22*/	/*VCR1*/
{0x00400000,_PANEL_INPUT7_KEY},			/*23*/	/*VCR2*/
{0x00800000,_PANEL_INPUT8_KEY},			/*24*/	/*SAT/DBS*/
{0x01000000,_PANEL_add_0,},				/*25*/
{0x02000000,_PANEL_add_1,},				/*26*/
{0x04000000,_PANEL_add_2,},				/*27*/
{0x08000000,_PANEL_add_3,},				/*28*/
{0x10000000,_PANEL_SUR_MODE_KEY},		/*29*/ 	/*sur. mode*/
{0x20000000,_PANEL_NEO6_KEY}, 			/*30*/	/*neo:6*/
{0x40000000,_PANEL_CES7_1_KEY}, 			/*31*/	/*ces 7.1*/
{0x80000000,_PANEL_add_4},				/*32*/			
{0x00000041,_PANEL_UPDATE_KEY,},			/*up + power*/
{0,0}
};

#endif
static KEYT panel_convert_key_value(PANEL_KEY_TYPE key)
{
	unsigned char i;
	for(i=0;panel_key_table[i].ic165_value;i++)
		{
		if(key == panel_key_table[i].ic165_value)
			return panel_key_table[i].key;
		}
	return _NO_KEY;
}

#define FRONT_BUTTON_MASK 	0xFFFFFFDF
#define HP_DET_MASK 			0x20

void HeadphoneDetect(unsigned char switch_mode)
{
	static uchar pre_mode,det_counter = 0;
	if(switch_mode)
		{
		if(pre_mode == 0)
			{
			det_counter = 0;
			}
		else
			{
			det_counter ++;
			if(det_counter == _NEW_KEY_PRESS_TIME * 3)
				{
				MsgSetKey(ZONE1_ID,MSG_KeyPress, _HP_ON_KEY); 
				}
			if(det_counter == 0xFF)
				{
				det_counter = _NEW_KEY_PRESS_TIME * 3;
				}
			}
		}
	else
		{
		if(pre_mode)
			{
			det_counter = 0;
			}
		else
			{
			det_counter ++;
			if(det_counter == _NEW_KEY_PRESS_TIME * 3)
				{
				MsgSetKey(ZONE1_ID,MSG_KeyPress, _HP_OFF_KEY); 
				}
			if(det_counter == 0xFF)
				{
				det_counter = _NEW_KEY_PRESS_TIME * 3;
				}
			}
		}
	pre_mode = switch_mode;
}

static _PANEL_KEY read_key()
{
	_PANEL_KEY temp_key;
	static unsigned char pre_key = 0;
	static unsigned char key_counter = 0;  
	unsigned char key,key_mode;
	uint32 hardware_key_value;

	hardware_key_value = read165();
	HeadphoneDetect(hardware_key_value & HP_DET_MASK); 
	key = panel_convert_key_value(hardware_key_value & FRONT_BUTTON_MASK);

	key_mode = KEY_NONE;
	if(key)
		{
		if(key != pre_key)	/*新按键，重新开始按键计数*/
			{
			key_counter = 0;
			pre_key = key;
			}
		else	
			{
			key_counter ++;
			if(key_counter == _NEW_KEY_PRESS_TIME)	/*按键要连续按住_NEW_KEY_PRESS_TIME*10ms (周期为10ms)才算有效*/
				{
				key_mode = KEY_PRESS;	/*新按键按下*/
				}
			if(key_counter == (_NEW_KEY_PRESS_TIME * 2 + _KEY_HOLD_TIME + 1))	/*连按之间的间隔为_KEY_HOLD_TIME*10ms */
				{
				key_counter = _NEW_KEY_PRESS_TIME * 2 +1;
				key_mode = KEY_HOLD; /*按键连按*/
				}
			}
		}
	else
		{
		if(pre_key)
			{
			if(key_counter >= _NEW_KEY_PRESS_TIME)	/*之前必须已经有按键按下才能认为是按键松开*/
				{
				key_mode = KEY_UP;	/*按键松开*/
				key = pre_key;
				}
			pre_key = 0;
			}
		key_counter = 0;
		}
	temp_key.key_value = key;
	temp_key.key_mode = key_mode;
	return temp_key;
}

#define ENCODER_COUNTER 120
#define NEW_KEY_COUNTER 5
unsigned char encoder_flag = 0;
unsigned char encoder_key = 0;
/* 
 * @brief 音量旋转器驱动程序
 *
 * @author chenjianming
 * @date 2007-12-7
 * @refer chenshiguang C11 code
 */
void GetEncoderSwitchThread(void)
{
	unsigned char current_flag = 0;
	static unsigned char pre_encoder_flag = 0,new_flag = 0;

	static int counter = 0,key_counter = 0;
	static short pre_key_value = _NO_KEY;

	if(counter < ENCODER_COUNTER)	/*counter 用来计数两次有效按键之间的时间*/
		{
		counter ++;
		}
	else
		{
		counter = ENCODER_COUNTER;
		key_counter = 0;
		}

	if(SenseEncoderSwitchPlus()) /*P1.0*/
		current_flag |= 0x01; 
	if(SenseEncoderSwitchMinus()) /*P0.26*/
		current_flag |= 0x02;

	if(pre_encoder_flag != current_flag)
		{
		switch(current_flag)
			{
			case 0x01:
				if(new_flag)	/*after started, get value*/
					{
					new_flag = 0;
					panel_key_value = _PANEL_ENCODER_MINUS_KEY; /*逆时针方向*/

					if((counter == ENCODER_COUNTER) || (pre_key_value != panel_key_value))
						{
						key_counter = 0;
						encoder_flag = 1;
						encoder_key = panel_key_value;
						}	/*新操作*/
					else /*当count !=ENCODER_COUNTER,且按键为同一个按键时*/
						{
						key_counter ++;
						if(key_counter > NEW_KEY_COUNTER)
							{
							encoder_flag = 1;
							encoder_key = panel_key_value;
							}
						}	/*连动*/
					counter = 0;	/*有效按键之后，将counter清零*/
					}
				break;
			case 0x02:
				if(new_flag)
					{
					new_flag = 0;
					panel_key_value = _PANEL_ENCODER_PLUS_KEY; /*顺时针方向*/

					if((counter == ENCODER_COUNTER) || (pre_key_value != panel_key_value))
						{
						key_counter = 0;
						encoder_flag = 1;
						encoder_key = panel_key_value;
						}	/*新操作*/
					else /*当count !=ENCODER_COUNTER,且按键为同一个按键时*/
						{
						key_counter ++;
						if(key_counter > NEW_KEY_COUNTER)
							{
							encoder_flag = 1;
							encoder_key = panel_key_value;
							}
						}	/*连动*/
					counter = 0;	/*有效按键之后，将counter清零*/
					}
				break;
			case 0x00:
				new_flag = 1;	/*两个编码电位器同时有效( 同为低电平)，以此为开始*/
				break;
			default:
				new_flag = 0;
				break;
			}
		pre_encoder_flag = current_flag;
		}
}

static void proc_key_press(KEYT key_value)
{

extern unsigned char HDRadio_FM_Flag; /*面板上FM 与AM共用一个键*/
extern unsigned char HDRadio_HDFM_Flag; /*面板上HDFM 与HDAM 共用一个键*/
extern SYSTEM_FLAG sys_flag;

	if(key_value == _PANEL_RADIO_KEY)
		{
		if(sys_flag.zone1_channel !=INPUT_TUNER)
			{
			 HDRadio_FM_Flag = 0;
			 key_value = _PANEL_AM_FM_KEY;
			  MsgSetKey(NULL_ID,MSG_KeyPress,key_value); 
			 return;
			}
		
		switch(current_radio_mode)
			{
			case _FM_MODE:				
				if(HDRadio_FM_Flag  == 1)
					 key_value = _PANEL_AM_FM_KEY;				
				break;
			case _AM_MODE:
				if(HDRadio_FM_Flag  == 0)				
					{
					 key_value = _PANEL_HD_RADIO_KEY;				
					 HDRadio_HDFM_Flag = 0;
					}
				break;
			case _HD_AM_MODE:
				if(HDRadio_HDFM_Flag  == 0)
					{
					 key_value = _PANEL_AM_FM_KEY;		
					  HDRadio_FM_Flag = 0;
					}
				break;
			case _HD_FM_MODE:
				if(HDRadio_HDFM_Flag  == 1)
					 key_value = _PANEL_HD_RADIO_KEY;				
				break;

			}
			 MsgSetKey(NULL_ID,MSG_KeyPress,key_value); 
		}
	else if(key_value != _PANEL_UPDATE_KEY)
		{
		MsgSetKey(NULL_ID,MSG_KeyPress,key_value); 
		}
}

static void proc_key_hold(KEYT key_value,unsigned short hold_time)
{
	if((key_value == _PANEL_UPDATE_KEY) && (hold_time == 10))
		MsgSetKey(NULL_ID,MSG_KeyPress,key_value); 
}

//static void proc_key_up(KEYT key_value,unsigned short hold_time)
//{
//}

static void GetPanelKeyThread(void)
{
	_PANEL_KEY panel_key;
	static int key_hold_timer;
	panel_key = read_key();
	switch(panel_key.key_mode)
		{
		case KEY_PRESS: /*按键按下*/
			key_hold_timer = 0;
			proc_key_press(panel_key.key_value); /*发送按前置面板按键信息*/
			break;
		case KEY_HOLD:	/*连按之间的间隔为_KEY_HOLD_TIME*10ms ,在这里定为100ms*/
			key_hold_timer ++;
			proc_key_hold(panel_key.key_value,key_hold_timer); /*发送连按前置面板按键信息*/
			break;
		case KEY_UP: /*按键松开*/
			//proc_key_up(panel_key.key_value,key_hold_timer); /*发送放开按键信息*/
			key_hold_timer = 0;
			break;			
//		case KEY_NONE:
//			break;
		}
}
#if 0
uchar ReadKey(KEYT * key,unsigned char mode,unsigned short timer)
{
	KEY_TASK_MSG *tmp_key;
	switch(mode)
		{
		case WAIT_KEY_VALID:
			if(timer == 0xFFFF)
				{
				while(GetKeyMsgQueue(&tmp_key) == FALSE);
				*key = tmp_key->key_value;
				return KEY_OK;
				}
			else
				{
				do{
					if(GetKeyMsgQueue(&tmp_key) == TRUE)
						break;
					else
						DelayMs(1);
					}while(timer--);
				}
			if(timer)
				{
				*key = tmp_key->key_value;
				return KEY_OK;
				}
			return KEY_TMO;	/*time over flow*/
		case GET_KEY_ONLY:
		default:
			if(GetKeyMsgQueue(&tmp_key) == TRUE)
				{
				*key = tmp_key->key_value;
				return KEY_OK;
				}
			return KEY_NULL;
		}
}
#endif

extern unsigned char ir_enable ;
extern unsigned char IR_Debug_Flag1;
extern unsigned char IR_Debug_Flag2;
extern unsigned char IR_Debug_Count;
extern unsigned short Debug_dataIR;
#ifdef _USE_ARTX
void key_task (void) __task {
#else
void key_task(void) {
#endif
#ifdef _KEY_USE_TIME_TICK
	os_itv_set (KEY_TASK_TICK);	/*设定每一个tick执行一次*/
#endif
#ifdef _USE_ARTX
	while(1)
#endif
		{
#ifdef _KEY_USE_TIME_TICK
		os_itv_wait ();
#endif
#if 0
		if(!key_enable)
			{
			encoder_flag = 0;
			zone1_ir_enable = 0;
			zone2_ir_enable = 0;
			return;
			}
#endif
		GetPanelKeyThread();
		if(encoder_flag)
			{
			encoder_flag = 0;
			MsgSetKey(NULL_ID,MSG_KeyPress,encoder_key);  /*发送EncoderSwitch(音量旋转器) 消息*/			
			}
		
		if(zone1_ir_enable)
			{
			if(zone1_ir_enable == 1)
				{  
				//debug_printf("\nzone1_codeIR=%d\n",zone1_codeIR&0x3F);
				MsgSetKey(NULL_ID,MSG_KeyPress,zone1_remote_key_table[zone1_codeIR & 0x3F]);  /*发送zone1 遥控器信息*/
				}
			zone1_ir_enable = 0;
			}

		if(zone2_ir_enable)
			{
			if(zone2_ir_enable == 1)
				{
				debug_printf("\nzone2_codeIR=%d\n",zone2_codeIR&0x3F);
				if((zone2_codeIR > 0x31)||((zone2_codeIR >0x23)&&(zone2_codeIR < 0x28)))
					MsgSetKey(ZONE1_ID,MSG_KeyPress,zone2_remote_key_table[zone2_codeIR & 0x3F]);  /*发送zone1 遥控器信息*/
				else	
					MsgSetKey(ZONE2_ID,MSG_KeyPress,zone2_remote_key_table[zone2_codeIR & 0x3F]);	/*发送Zone2 遥控器信息*/
				}
			zone2_ir_enable = 0;
			}
#ifdef _KEY_USE_TIME_TICK
		os_tsk_pass();
#endif
		}
}

