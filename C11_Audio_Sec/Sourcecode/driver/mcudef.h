/********************************************************
 *mcudef.h
 *陈世光
 *2007-04-27
 *Ver:0.1
 *定义与ARM相关的时钟信息
 *此设置必须和startup.s里面的设置一样
 ********************************************************/
 /*
#ifndef _MCUDEF_H
#define _MCUDEF_H

#include <LPC21xx.H>                 // LPC21xx definitions                  
#include "pin_config.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

#define FOSC 11059200L	//晶体频率

// <e> PLL ENABLE
//   <o1.0..4>   MSEL: PLL Multiplier Selection
//               <0-31>
//               <i> PLL Multiplier "M" Value
//               <i> CCLK = M * Fosc
// </e>
#define PLL_ENABLE 1
#define PLL_MVALUE 4

// <e> VPBDIV ENABLE
//   <o1.0..2>   VPBDIV: VPB Clock
//               <4=> VPB Clock = CPU Clock / 4
//               <1=> VPB Clock = CPU Clock
//               <2=> VPB Clock = CPU Clock / 2
// </e>
#define VPBDIV_ENABLE 1
#define VPBDIV_VALUE 2

#ifdef PLL_ENABLE
#define CCLK ((FOSC) * (PLL_MVALUE))	//CPU时钟 //44M
#else
#define CCLK (FOSC)
#endif

#ifdef VPBDIV_ENABLE
#define PCLK ((CCLK) / (VPBDIV_VALUE))	//内部各个子模块时钟 //22M
#else
#define PCLK (CCLK)
#endif

//================================================================

#define T0_TICK 1000	//Timer 0 tick
#define T0_RLV ((unsigned long)(((double)PCLK*(double)T0_TICK)/1E6)-1)	//Timer 0 Reload Value  //1E6--10 的6次方 //1ms 产生一次中断

#endif
*/

/* @file  mcudef.h
 * @brief Clock Setting
 *
 * @author chenjianming
 * @date 2007-12-20
 * @refer 
 *    定义与ARM相关的时钟信息
 *    此设置必须和startup.s里面的设置一样
 */
#ifndef _MCUDEF_H
#define _MCUDEF_H

#include <LPC23xx.H>                 // LPC23xx definitions                  
#include "pin_config.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

#define FOSC 12000000L	//晶体频率

#define PCLK (FOSC*13/12)	/*晶体频率为12.000MHz*/
#define MUL_VALUE 1		/*设备频率为晶体频率,无倍频*/
#define TIMER1_FREQ 2	/*定时器1频率为2Hz*/
#define TIMER1_PERIOD (((PCLK* MUL_VALUE) /TIMER1_FREQ) - 1)
#define TIMER0_FREQ 1000/*定时器0频率为500Hz*/
#define TIMER0_PERIOD (((PCLK* MUL_VALUE) /TIMER0_FREQ) - 1)

#endif

