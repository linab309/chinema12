#include "mcudef.h"
#include "timer.h"
#include "ioport.h"
#include "..\main\task.h"
#include "..\main\vfd_disp.h"
#include "..\main\Radio_task.h"
#include "..\main\Queue.h"

extern void key_task(void);
extern void RefreshHandle(void);
extern void SetupFlashHandler(void);  
extern void GetEncoderSwitchThread(void);
unsigned char psm_8416_time_tick = 0; /*jian add in 12-01-30*/

static short system_delay_timer;
void DelayMs(int delay_time)
{
	if(delay_time > 0)
		{
		system_delay_timer = delay_time;
		while(system_delay_timer);
		}
}

extern unsigned char softmute_flag;
extern unsigned char eint1_timer_cnter;
extern unsigned char eint2_timer_cnter;
extern unsigned char eint3_timer_cnter;
extern unsigned char Tuner_Text_Disp_Flag;
extern unsigned char DSP_HDMI_Setting_Flag;
unsigned short rs232_timer = 0;
unsigned char Tuner_Text_Count=0;
unsigned char Text_Update_Flag=0;
unsigned char HDMI_tick=0;
unsigned char Hdmi_Stream_tick=0;
unsigned char DSD_Stream_tick=0;
unsigned char DSP_Busy_tick=0;
unsigned char radio_time_tick = 0;
unsigned char system_timer_flag = 0;
unsigned char pcm20_delay_timer = 0;
unsigned char pcm20_pill_timer = 0;
void timer0_irq (void) __irq	/*timer 0 tick: 2ms 产生一次中断*/
{
	static unsigned int t_count = 0; /*Text Display 滚动显示*/
	static unsigned int text_count=0; /*Text Display 实时读取数据*/
	static unsigned int hdmi_cnter=0; /*HDMI 寄存器访问*/
	static unsigned int t_cnter = 0;	
	static unsigned int softmute_cnter=0; /*softmute功能开启*/
	static unsigned int HDMI_Setting_cnter=0; /*HDMI 高清流配置*/
	extern uint8 Artist_len ;
	extern uint8 Title_len ;

	T0IR = 1; /* Clear interrupt flag*/
	VICIntEnClr = VIC_ENABLE_TIMER0; /*写入1清零中断使能寄存器中的对应位并禁止对应的中断请求*/

	/*for remote pluse counter to remember timer tick*/
	eint1_timer_cnter++;
	eint2_timer_cnter++;
	eint3_timer_cnter++;

	/*for DelayMs(int)*/
	if(system_delay_timer > 0)	
		system_delay_timer --;

	/*for Flash Rs232*/
	if(rs232_timer)
		rs232_timer --;	
	system_timer_flag = 1 ; // jian add in 10-08-06
	/*HDMI 寄存器访问*/
	hdmi_cnter++;
	if(hdmi_cnter>500)
		{
		HDMI_tick=1;
		Hdmi_Stream_tick=1;		
		DSD_Stream_tick=1;
		DSP_Busy_tick++;
		hdmi_cnter=0;
		}
  
	t_cnter ++; 
	if(t_cnter >100)  
		{
		t_cnter = 0;  
		psm_8416_time_tick = 1; // jian add in 12-01-30
		radio_time_tick = 1; /*for FM MODE 按键显示功能*/    
		VfdTimerHandler(); /*VFD显示2区信息后再跳变显示1区的信息*/ /*EQ ON OFF 显示*/
		RefreshHandle(); /*FM下音量刷新*/		
		AD0CR = 0; //jian add in 10-08-05
		}
	
	/*for Text Disply 滚动显示*/
	t_count++;
	if(t_count>250) /*250ms*2=500ms--滚动一字节change  50-- 250*/
		{
		t_count=0;
		if(Tuner_Text_Disp_Flag) /*Text Display 显示功能已开启*/
			{
			Tuner_Text_Count++; /*显示的字符从右向左一个个字符滚动过去*/
			if(Tuner_Text_Count == (Artist_len+Title_len -19)) /*最后20个字节就不滚动了,重新从第0个开始显示*/
				Tuner_Text_Count=0;
			MsgSetVfdShow(RADIO_ID,1); /*mode=1--用于Text Display*/
			}
		}

	if(Tuner_Text_Disp_Flag)/*Text Display 显示功能已开启*/
		{
		text_count++;
//		if(text_count>15000) /*15000*2=30000ms--滚屏一次80字节总共的时间*/ 
		if(text_count>(187*(Artist_len+Title_len)))
			{
			text_count=0; 
			Text_Update_Flag=1;
			}
		}

	if(softmute_flag)
		{
		softmute_cnter++;
		if(softmute_cnter>1500) 
			{
			softmute_cnter=0;
			softmute_flag=0;
			MsgSetSoftMute(ZONE1_ID,0);
			}
		}

	if(DSP_HDMI_Setting_Flag)
	{
		HDMI_Setting_cnter++;
		if(HDMI_Setting_cnter>2500) /*2000*2=4000ms*/ 
		{
			HDMI_Setting_cnter=0;
			DSP_HDMI_Setting_Flag=0;
			MsgSetDSPHDMISetting(ZONE1_ID,0); /*加载菜单设置*/
		}
	}
	
	VICIntEnable = VIC_ENABLE_TIMER0; /* 1使能中断请求或软件中断分配为FIQ或IRQ */
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

unsigned char dsp_time_tick = 0;

extern pSTRUCT_QUEUE tuner_queue_pointer;  
//int dsp_setting_timer_cnt = 0;
extern void lpcm_20_config1(void);
void Timer0_Handler(void) /*定时任务放到主循环处理,尽量不要放在定时中断处理*/
{
	static unsigned char key_cnt = 0; /*键盘扫描*/
	
	/*get the encoder switch */	
	GetEncoderSwitchThread(); 

	/*for 按键扫描*/
	key_task(); 
	
	key_cnt ++;
	if(key_cnt >2)
		{
		dsp_time_tick = 1;
		key_cnt = 0;
		}
	
	//jian add in 10-11-03
	if(system_timer_flag)	
		{
		system_timer_flag = 0;
	//	HDMI_secret_loop();
		if(pcm20_delay_timer)
			{
			pcm20_delay_timer--;
			if(pcm20_delay_timer == 0)
				{
				lpcm_20_config();
				pcm20_pill_timer	= 100;	
				}
			
			}
		
		if(pcm20_pill_timer)
			{
			pcm20_pill_timer--;
			if(pcm20_pill_timer==0)
				{
				lpcm_20_config1();
				}
			}
		
		}
	/*for Text Display 实时刷新*/
	if(Text_Update_Flag) /*每30 s 刷新一次*/
		{
		Text_Update_Flag=0;
		if(Tuner_Text_Disp_Flag) /*Text Display 显示功能已开启*/
			{
			Tuner_Text_Disp_Flag=0; /*当在读PSD 包信息时,Update text 的text_count 不计时,用于解决BUG*/
			
	//		Tuner_Reply_Flag=1;	/*准备接收PSD CHANGE 包*/		
//			Tuner_Req_Radio_Monitor_Command(0,0,1); /*向HD Radio 发送命令设置标志位读取PSD CHANGE 包*/				
			#if 0 // jian remove it in 11-08-18
			do{
				C11_Tuner_PSD_Change();    
				}while(Tuner_Reply_Flag);
			#endif
		//	Tuner_Req_Radio_Monitor_Command(1,0,0); /*接收完后清PSD 包标志位*/			
			Tuner_Text_Count=0; /*实时刷新都从第0个开始显示*/
		  	RadioDisplay(system_freq,current_radio_mode,system_position); /*解决长时间显示TextDisplay屏乱问题*/    	  		
			Radio_Text_Display();
			DelayMs(30); 
			debug_printf("initqueue \n");
			InitQueue(tuner_queue_pointer);/*解决长时间显示TextDisplay死机问题*/	      
			
			Tuner_Text_Disp_Flag=1;  /*当在读完PSD 包信息时,Update text 的text_count 开始计时,用于解决BUG*/
			}  
		}

	/*修改输入通道字符时,显示"_"*/
	SetupFlashHandler(); 
}

void Timer0Init()
{
	T0MR0 = TIMER0_PERIOD;
	T0MCR = 3;	/*2设置定时器值匹配时，复位定时器值，并中断*/
	T0TCR = 1;	/* Timer0 Enable--Timer counter are enabled for counting*/
#if 1
	VICVectAddr4= (unsigned long)timer0_irq; /* set interrupt vector in 0 */
	VICVectCntl4 = VIC_PRIORITY_4;		  /* use it for Timer 0 Interrupt Priority*/
	VICIntEnable = VIC_ENABLE_TIMER0; 	  /*中断使能*/ 
#endif
}

