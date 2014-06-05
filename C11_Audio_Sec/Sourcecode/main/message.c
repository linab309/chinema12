#include "macro.h"
#include "message.h"
#include "debug.h"
#include "task.h"
#include <stdlib.h>
#include <string.h>

#ifdef _DEBUG
//#define _DEBUG_MESSAGE
#endif

//MSG * pMsgQueue; 		/*MSG 结构体指针*/ /*Dele cjm 2008-05-08 */
MSG pMsgQueue[200];	/*MSG 结构体数组*/ /*add  cjm 2008-05-08 */
short msg_queue_rear;	/*队列尾*/
short msg_queue_front;	/*队列头*/
#define MSGQUEUE_LENGTH MESSAGE_QUEUE_SIZE

unsigned char InitMsgQueue(void)
{
	if(!pMsgQueue)
		{
		/*分配200个结构体长度空间的首地址给pMsgQueue*/
		//pMsgQueue = (MSG *) malloc(MESSAGE_QUEUE_SIZE * sizeof(MSG)); /*sizeof(MSG) = 5*/ /*Dele cjm 2008-05-08 */
		if(!pMsgQueue)
			{
#ifdef _DEBUG_MESSAGE
			debug_printf("out off memory\n");
#endif
			return FALSE;
			}
		}
	msg_queue_rear = msg_queue_front = 0;
	return TRUE;
}

/*判断消息队列是否为空*/
static unsigned char IsMsgQueueEmpty()
{
	if(msg_queue_rear == msg_queue_front)
		return TRUE;
	return FALSE;
}

/*判断消息队列是否已满*/
static unsigned char IsMsgQueueFull()
{
	if((msg_queue_rear+1)%MSGQUEUE_LENGTH == msg_queue_front) 
		{
		return TRUE;
		}
	return FALSE;
}

/*向队列头填充数据*/
unsigned char PostMessage(TASK_ID t_id,unsigned char *buffer)
{
	if(IsMsgQueueFull())
		{
		assert(IsMsgQueueFull());
		return FALSE;
		}
	
	memcpy(pMsgQueue[msg_queue_front].buffer,buffer,MESSAGE_SIZE);
	pMsgQueue[msg_queue_front].t_id = t_id;
	msg_queue_front = (msg_queue_front-1+MSGQUEUE_LENGTH)%MSGQUEUE_LENGTH;
	return TRUE;
}

/*向队列尾填充数据*/
unsigned char SendMessage(TASK_ID t_id,unsigned char *buffer)
{
	if(IsMsgQueueFull())
		{
		assert(IsMsgQueueFull());
		return FALSE;
		}

	msg_queue_rear = (msg_queue_rear+1)%MSGQUEUE_LENGTH;
	memcpy(pMsgQueue[msg_queue_rear].buffer,buffer,MESSAGE_SIZE);
	pMsgQueue[msg_queue_rear].t_id = t_id;
	//DispBuffer(pMsgQueue[msg_queue_rear].buffer,MESSAGE_SIZE);
	return TRUE;
}

/*消息出队列，通过pMsg返回
 return:
 	FALSE 队列空
 	TRUE 有返回
 */
unsigned char GetMessage(MSG * pMsg)
{
	if(IsMsgQueueEmpty())
		return FALSE;

	msg_queue_front = (msg_queue_front+1)%MSGQUEUE_LENGTH;
	memcpy(pMsg, pMsgQueue + msg_queue_front, sizeof(MSG));
	return TRUE;
}

