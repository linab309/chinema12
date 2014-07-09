#include "..\driver\ioport.h"
#include "..\driver\uart.h"
#include "..\driver\timer.h"
#include "..\driver\cs8416.h"
#include "..\driver\Dac.h"
#include "..\driver\vfddriver.h"
#include "..\driver\eeprom.h"
#include "..\driver\ir.h"
#include "..\driver\pin_config.h"
#include "..\driver\Hdmi.h"

#include "debug.h"
#include "task.h"
#include "vfd_disp.h"
#include "message.h"
#include "key_tab.h"
#include "keydefine.h"
#include "macro.h"
#include <stdlib.h>
#include "queue.h"
#include "Radio_task.h"
#include <LPC23xx.H>

#include "..\cs495300\dspdrv.h"

PFV Rs232ProtocolHandler = C11Rs232;

#define TEST_GITHUb 1
/*jian addd this test in 2014-07-09*/
void Systeminit(void)
{
	HardwareInit(); /*硬件初始化*/
	UartInit();	     /*串口初始化*/
	Timer0Init();     /*定时器初始化*/
	InitRemoteRepeatKey();
	AddRemoteRepeatKey(_IR_VOL_UP_KEY);	/*音量+键是允许连按的*/
	AddRemoteRepeatKey(_IR_VOL_DOWN_KEY);/*音量-键是允许连按的*/
}

void ProcDimerOff(KEYT key)
{
	if(sys_flag.system_status == POWER_ON)
		{
		if(key != _REMOTE_BRIGHT_KEY)
			{
			if(sys_flag.system_dimer == 0)
				{
				VfdSetDimerValue(2);	/*off状态下，如果有按键操作，则亮度设置在low状态*/
				temp_dimer_time = 50;
				}
			}
		}
}

extern void debug_message(MSG message);
int main()
{
	MSG message;
	unsigned char status;

	Systeminit(); /*系统初始化 */

	status=InitMsgQueue();
	assert(status == FALSE);

	//PowerLed(ON);
	LedStandby();

	LoadSystemStartupValue(); //Load the saved start up value
	SetIoStandby();
	InitDSDMode(2);
//        DspMasterBoot();//jian add in 10-09-01
	while(1)
	{
	Rs232ProtocolHandler();

	Timer0_Handler();

	if(GetMessage(&message) == TRUE)
		{
#ifdef _DEBUG
		//debug_message(message);
#endif
		ProcMessage(&message);
		}
//
	DspLoop();

	HDMI_DSD_Loop();

	if(need_refresh_vfd)
		{
		need_refresh_vfd = 0;
		VfdRefresh();
		}
	if(need_refresh_dimer)
		{
		need_refresh_dimer = 0;
		VfdSetDimerValue(sys_flag.system_dimer);
		}

	RadioLoop();
	}
}

#if 0
//Debug --最小系统
int main()
{
	unsigned char i=0;

	HardwareInit(); /*硬件初始化*/
	UartInit();	     /*串口初始化*/
	Timer0Init();     /*定时器初始化*/
	PowerLed(ON);

	//SystemPowerOn();

	debug_printf("\n11111111111111111");
	i = EepromReadByte(0);
	debug_printf("PRODUCT_ID_POSITION:%02X\n",i);

	SetTrigerOut(1);
	debug_printf("\n11111111111111111");

	while(1)
		{
		SetTrigerOut(1);
		debug_printf("\n11111111111111111");
		}
}
#endif

