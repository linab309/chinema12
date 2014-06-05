#ifndef _QUEUE_H
#define _QUEUE_H

typedef unsigned char queue_type;

typedef struct{
	queue_type * queue_buf;
	short front,rear;
	short queue_max_element;
}STRUCT_QUEUE,*pSTRUCT_QUEUE;

#define QUEUE_NORMAL 0
#define QUEUE_FULL 1
#define QUEUE_EMPTY 2

extern void InitQueue(pSTRUCT_QUEUE queue_name);
extern int IsQueueEmpty(pSTRUCT_QUEUE queue_name);
extern int IsQueueFull(pSTRUCT_QUEUE queue_name);
extern int InQueue(pSTRUCT_QUEUE queue_name,queue_type InValue);
extern int DeQueue(pSTRUCT_QUEUE queue_name,queue_type *OutValue);

extern int GetQueueElementNum(pSTRUCT_QUEUE queue_name);	/*获取队列中已经拥有的元素个数*/
extern int GetQueueElementRemain(pSTRUCT_QUEUE queue_name);	/*获取队列剩余的元素个数*/
extern int GetQueueElement(queue_type *buffer,pSTRUCT_QUEUE queue_name,int start_position,int size);

#endif

