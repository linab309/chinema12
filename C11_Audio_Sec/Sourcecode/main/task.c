#include "macro.h"
#include "task.h"
#include "debug.h"
#include "message.h"

#include <stdlib.h>

const TASK_T pZone1Task = {ZONE1_ID,Zone1ProcMessage};
const TASK_T pSetupTask = {SETUP_ID,SetupProcMessage};
const TASK_T pRadioTask = {RADIO_ID,RadioProcMessage};  
const TASK_T pZone2Task = {ZONE2_ID,Zone2ProcMessage};
const TASK_T pAutoSoundTask = {AUTO_SOUND_ID,AutoSoundProcMessage};
//const TASK_T pFlashUpdateTask = {FLASH_UPDATE_ID,FlashUpdateProcMessage};

const TASK_T * tasks[] = 
{
	&pZone1Task,
	&pZone2Task,
	&pSetupTask,
	&pRadioTask,
	&pAutoSoundTask,
//	&pFlashUpdateTask,
	NULL,
};

TASK_ID current_tid = ZONE1_ID;

BOOL IsCurFocusTid(TASK_ID t_id)
{
	if(current_tid == t_id)
		return TRUE;
	return FALSE;
}

BOOL SetCurFocusTid(TASK_ID t_id)
{
	if(t_id == NULL_ID)
		return FALSE;
	current_tid = t_id;
	return TRUE;
}

TASK_ID GetCurFocusTid(void)
{
	return current_tid;
}

TASK_RESULT ProcMessage(PMSG pMsg)
{
	unsigned char i;
	TASK_ID t_id;

	t_id = pMsg->t_id;

	if(t_id == NULL_ID)
		t_id = GetCurFocusTid(); /*得到当前的Tid*/

	for(i=0;tasks[i];i++)
		{
		if(tasks[i]->t_id == t_id) /*所有的Tid 与当前的Tid 一样时,执行run 函数*/
			{
			return tasks[i]->run(pMsg->buffer);
			}
		}
	return FALSE;
}

