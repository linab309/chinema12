#ifndef _IR_H
#define _IR_H

#if 1
#define IR_PHILIPS /*RC-5Ò£¿ØÆ÷*/
#else
#define IR_NEC	/*NECÒ£¿ØÆ÷*/
#endif

#ifdef IR_PHILIPS

#define IR_IDLE 0
#define IR_LEADER 1
#define IR_CUSTOM 2

#define HALFBIT_TIME 0.000889 //(0.889ms)
#define ONEBIT_TIME 0.001778	//(1.778ms)
#define ADJUST_TIME 0.0004

#if 1
#define HALFBIT_TYP (HALFBIT_TIME * PCLK)	/*9250*/
#define ONEBIT_TYP (ONEBIT_TIME * PCLK)	/*18500*/
#define ADJUST_TIME_TYP (ADJUST_TIME * PCLK) /*2000*/
#else
#define HALFBIT_TYP 19663
#define ONEBIT_TYP 39326
#define ADJUST_TIME_TYP 4423
#endif

#define HALFBIT_MIN	(HALFBIT_TYP - ADJUST_TIME_TYP)
#define HALFBIT_MAX	(HALFBIT_TYP + ADJUST_TIME_TYP)
#define ONEBIT_MIN	(ONEBIT_TYP - ADJUST_TIME_TYP)
#define ONEBIT_MAX	(ONEBIT_TYP + ADJUST_TIME_TYP)

extern void ir_core_philips(unsigned long width,unsigned char falling_edge);
extern void ir2_core_philips(unsigned long width,unsigned char falling_edge);
#endif	/*IR_PHILIPS*/

#ifdef IR_NEC
#define IR_IDLE 0
#define IR_LEADER_LOW 1
#define IR_LEADER_HIGH 2
#define IR_CUSTOM 3
#define IR_REPEAT 4

#define LEADER_HIGH_TIME 0.0045	/*4.5ms (11059.2*4.5) */
#define LEADER_LOW_TIME 0.009	/*9ms*/
#define DATA_0_TIME 0.000565	/*0.565ms*/
#define DATA_1_TIME 0.00169 /*1.69ms*/
#define REPEAT_DATA_TIME 0.00056	/*0.56ms*/
#define REPEAT_HIGH_TIME 0.00225	/*2.25ms*/
#define ADJUST_TIME 0.0002
#define ADJUST_TIME_2 (ADJUST_TIME *2)

#if 0
#define LEADER_HIGH_TYP (LEADER_HIGH_TIME * PCLK)	/*49766*/	/*4.5ms (11059.2*4.5) */
#define LEADER_LOW_TYP (LEADER_LOW_TIME * PCLK)	/*99532*/	/*9ms*/
#define DATA_0_TYP (DATA_0_TIME * PCLK)	/*6248*/	/*0.565ms*/
#define DATA_1_TYP (DATA_1_TIME * PCLK)	/*18690*/ /*1.69ms*/
#define REPEAT_DATA_TYP (REPEAT_DATA_TIME * PCLK)	/*6248*/	/*0.56ms*/
#define REPEAT_HIGH_TYP (REPEAT_HIGH_TIME * PCLK)	/*24883*/	/*2.25ms*/
#define ADJUST_TIME_TYP (ADJUST_TIME * PCLK)	/*2000*/
#define ADJUST_TIME_TYP_2 (ADJUST_TIME_TYP *2)
#else
#define LEADER_HIGH_TYP 49766	/*4.5ms (11059.2*4.5) */
#define LEADER_LOW_TYP 99532	/*9ms*/
#define DATA_0_TYP 6248			/*0.565ms*/
#define DATA_1_TYP 18690 		/*1.69ms*/
#define REPEAT_DATA_TYP 6248	/*0.56ms*/
#define REPEAT_HIGH_TYP 24883	/*2.25ms*/
#define ADJUST_TIME_TYP 2000
#define ADJUST_TIME_TYP_2 (ADJUST_TIME_TYP *2)
#endif

#define DATA_0_MIN (DATA_0_TYP - ADJUST_TIME_TYP)
#define DATA_0_MAX (DATA_0_TYP + ADJUST_TIME_TYP)
#define DATA_1_MIN (DATA_1_TYP - ADJUST_TIME_TYP)
#define DATA_1_MAX (DATA_1_TYP + ADJUST_TIME_TYP)
#define LEADER_LOW_MIN (LEADER_LOW_TYP - ADJUST_TIME_TYP_2)
#define LEADER_LOW_MAX (LEADER_LOW_TYP + ADJUST_TIME_TYP_2)
#define LEADER_HIGH_MIN (LEADER_HIGH_TYP - ADJUST_TIME_TYP_2)
#define LEADER_HIGH_MAX (LEADER_HIGH_TYP + ADJUST_TIME_TYP_2)
#define REPEAT_DATA_MIN (REPEAT_DATA_TYP - ADJUST_TIME_TYP)
#define REPEAT_DATA_MAX (REPEAT_DATA_TYP + ADJUST_TIME_TYP)
#define REPEAT_HIGH_MIN (REPEAT_HIGH_TYP - ADJUST_TIME_TYP)
#define REPEAT_HIGH_MAX (REPEAT_HIGH_TYP + ADJUST_TIME_TYP)

//extern void ir_core_nec(unsigned long int width,uint8 falling_edge);
extern void ir_core_nec(unsigned long int width,unsigned char falling_edge);
#endif	/*IR_NEC*/

#define MAX_REPEAT_KEY 0x07
typedef struct
{
unsigned char buffer[MAX_REPEAT_KEY];
unsigned char current_repeat_cnters;
}STRUCT_IR_REPEAT_KEY,*pSTRUCT_IR_REPEAT_KEY;

extern unsigned char zone1_ir_enable;
extern unsigned char zone1_codeIR;
extern unsigned char OthersystemIR;

extern unsigned char zone2_ir_enable;
extern unsigned char zone2_codeIR;
extern unsigned char OthersystemIR2;
#endif	/*_IR_H*/

