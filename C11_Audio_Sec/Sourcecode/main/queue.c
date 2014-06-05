#include "queue.h"

void InitQueue(pSTRUCT_QUEUE queue_name)
/*消息队列初始化*/
{
	queue_name->front = -1;
	queue_name->rear = -1;
}

int IsQueueEmpty(pSTRUCT_QUEUE queue_name)
/*队列空,让队头front与队尾rear值相同时，队列处于空状态*/
{
	if((queue_name->front) % queue_name->queue_max_element == queue_name->rear)
		{
		return QUEUE_EMPTY;
		}
	return QUEUE_NORMAL;
}

int IsQueueFull(pSTRUCT_QUEUE queue_name)
/*当队尾rear加1之后，值与对头front相同时，队列处于满状态*/
{
	if((queue_name->rear+1) % queue_name->queue_max_element == queue_name->front)
		return QUEUE_FULL;
	return QUEUE_NORMAL;
}

int QueueFront(pSTRUCT_QUEUE queue_name)
/*返回当前队列头front位置*/
{
	return queue_name->front;
}

int DeQueue(pSTRUCT_QUEUE queue_name,queue_type *OutValue)
/*出队列，值放在*OutValue，成功返回0，失败返回1*/
{
	if(IsQueueEmpty(queue_name) == QUEUE_NORMAL)
		{
		queue_name->front = (queue_name->front + 1) % queue_name->queue_max_element;
		*OutValue = queue_name->queue_buf[queue_name->front];
		return 0;
		}
	return 1;
}

int InQueue(pSTRUCT_QUEUE queue_name,queue_type InValue)
/*入队列，值放在InValue，成功返回0，失败返回1,往队尾添加元素*/
{
	if(IsQueueFull(queue_name) == QUEUE_NORMAL)
		{
		queue_name->rear = (queue_name->rear + 1) % queue_name->queue_max_element;
		queue_name->queue_buf[queue_name->rear] = InValue;
		return 0;
		}
	return 1;
}

int GetQueueElementNum(pSTRUCT_QUEUE queue_name)
/*返回当前队列中的元素个数*/
{
	if(IsQueueEmpty(queue_name) == QUEUE_EMPTY)
		return 0;
	if(IsQueueFull(queue_name))
		return queue_name->queue_max_element;
	if((queue_name->front) > (queue_name->rear))
		{
		return (queue_name->queue_max_element) - ((queue_name->front) - queue_name->rear);
		}
	else
		return ((queue_name->rear) - (queue_name->front));
}

int GetQueueElementRemain(pSTRUCT_QUEUE queue_name)
/*返回当前队列中的元素个数*/
{
	return queue_name->queue_max_element - GetQueueElementNum(queue_name);
}

int GetQueueElement(queue_type *buffer,pSTRUCT_QUEUE queue_name,int start_position,int size)
{
	int i;
	for(i=0;i<size;i++)
		{
		*(buffer+i) = queue_name->queue_buf[(start_position+i)%queue_name->queue_max_element];
		}
	return 0;
}

