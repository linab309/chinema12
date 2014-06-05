#include "..\main\macro.h"
#include "..\main\debug.h"
#include "..\driver\ioport.h"
#include "ir.h"
#include "mcudef.h"
#include <string.h>

#define IRREP

#define sysIR1code 0x13	/*C11 IR1 system code--ZONE1*/

unsigned char zone1_ir_enable = 0;
unsigned char zone1_codeIR;
unsigned char OthersystemIR;

#define sysIR2code 0x12   /*C11 IR2 system code--ZONE2*/


unsigned char zone2_ir_enable = 0;
unsigned char zone2_codeIR;
unsigned char OthersystemIR2;

#ifdef IRREP
int zone2_repeat_IRkey_allowed(unsigned char ir_key);
#endif

#ifdef IRREP

#define REMOTE_INVALID_KEY 0xFF
STRUCT_IR_REPEAT_KEY remote_repeat =
{
{0},
0,
};

BOOL repeat_IRkey_allowed(uint8 ir_key)
{
	uchar i;

	for(i=0;i<remote_repeat.current_repeat_cnters;i++)
		{
		if(remote_repeat.buffer[i] == REMOTE_INVALID_KEY)
			return 0;
		if(ir_key == remote_repeat.buffer[i])
			return TRUE;
		}
	return FALSE;
}

void InitRemoteRepeatKey(void)
{
	memset(remote_repeat.buffer,REMOTE_INVALID_KEY,MAX_REPEAT_KEY); /*把remote_repeat.buffer 前0x07个字节设置成0xFF*/
	remote_repeat.current_repeat_cnters = 0;
}

BOOL AddRemoteRepeatKey(unsigned char key)
{
	if(remote_repeat.current_repeat_cnters < MAX_REPEAT_KEY)
		{
		if(repeat_IRkey_allowed(key) == FALSE)	/*如果此按键已经在数组中，则无需再添加*/
			{
			remote_repeat.buffer[remote_repeat.current_repeat_cnters] = key;
			remote_repeat.current_repeat_cnters ++;
			return TRUE;
			}
		}
	return FALSE;
}

void DeleteRemoteRepeatKey(unsigned char key)
{
	uint8 i,valid_key_flag = 0;
	for(i=0;i<remote_repeat.current_repeat_cnters;i++)
		{
		if(key == remote_repeat.buffer[i])
			{
			/*确定删除一个按键，设置标志位，让后续能将current_repeat_cnters减1*/
			valid_key_flag = 1;
			break;
			}
		}
	for(;i<remote_repeat.current_repeat_cnters;i++)
		{
		if((i+1) < remote_repeat.current_repeat_cnters)
			{
			remote_repeat.buffer[i] = remote_repeat.buffer[i+1];
			}
		else
			{
			remote_repeat.buffer[i] = REMOTE_INVALID_KEY;
			}
		}
	if((remote_repeat.current_repeat_cnters) && (valid_key_flag == 1))
		remote_repeat.current_repeat_cnters --;
}
/*
unsigned char zone1_repeat_IRkey_allowed(unsigned char ir_key)
{
	if((ir_key == 0x10) || (ir_key == 0x11))
		return 1;
	return 0;
}
*/
#endif

#ifdef IR_PHILIPS
#define BIT_PHASE_FALLINGEDGE_1
void ir_core_philips(unsigned long width,unsigned char falling_edge)
{
	static unsigned short dataIR;
	static unsigned char cntIRbits = 0;
	static unsigned char stateIR = IR_IDLE;
	static unsigned char data_half = 0;  /* Mark whether or nor have half_bit data before edge of intr. coming*/
	static unsigned char IR_ctlbit;
	static unsigned char IR_rep_cnt;

	unsigned char reset;
	unsigned char is_half_bit;
	unsigned char is_full_bit;
	unsigned char prevctlbit;
	reset = 0;
	
	is_half_bit = (width >= HALFBIT_MIN) && (width <= HALFBIT_MAX);
	is_full_bit = (width >= ONEBIT_MIN)  && (width <= ONEBIT_MAX);

	if (stateIR == IR_IDLE)
		{
		if (falling_edge)
			stateIR = IR_LEADER;
/*		else
			prevIRtime = 0;*/
		}
	else if (stateIR == IR_LEADER)
		{
		/* We shall get a rising edge, since the first bit is fixed */
		dataIR = 0;
		cntIRbits = 0;
		stateIR = IR_CUSTOM;

		if (is_half_bit)
			data_half = 0;
		else if (is_full_bit)
			data_half = 1;
		else
			{
			stateIR = IR_IDLE;
			//prevIRtime = 0;
			}
		}
	else
		{
		if (data_half)
			{
			/* 
			* We were in half bit position, so this edge shall either
			* conclude the previous cycle or go the the half position
			* of the next bit. Record the last bit.
			*/
			dataIR <<= 1;
#ifdef BIT_PHASE_FALLINGEDGE_1
			if (!falling_edge)
				dataIR |= 1;
#else
			if (falling_edge)
				dataIR |= 1;
#endif
			cntIRbits++;

			if (is_half_bit)
				data_half = 0;
			else if (!is_full_bit)
				reset = 1;
			}
		else
			{
			/* 
			* We started at a sampling cycle, so we shall only get half bit,
			* otherwise, something is wrong!
			*/
			if (is_half_bit)
				data_half = 1;
			else
				reset = 1;
			}

		if ((cntIRbits == 12) && !falling_edge && data_half)
			{
			/* This is the last rising edge, no more. So collect the bit */
			dataIR <<= 1;
#ifdef BIT_PHASE_FALLINGEDGE_1
#else
			dataIR |= 1;
#endif
			cntIRbits = 13;
			}

		if (reset)
			{
			/* 
			* Abnormal exist. Maybe we are out of sync. If this
			* is falling edge, maybe this is the sync of a new
			* input!
			*/
			stateIR = IR_IDLE;
			if (falling_edge)
				stateIR = IR_LEADER;
			}
		else if (cntIRbits == 13)	/*接收到一组完整的按键*/
			{
			/* We only care the case when system code matches */
			if (((dataIR >> 6) & 0x1f) == sysIR1code)
				{
				prevctlbit = IR_ctlbit;
				IR_ctlbit = (dataIR >> 11) & 1;
				
				if (((unsigned int) IR_ctlbit != prevctlbit) || ((dataIR & 0x3F) != zone1_codeIR))
					/*((dataIR & 0x3F) != zone1_codeIR) 的判断是为了防止用户按下某一个按键之后，中间过程遥控头没有接收到信号，等遥控头再次接收到信号的时候，翻转位仍然一样，但是用户码已经变了*/
					{
					zone1_codeIR = dataIR & 0x3F;		
					if(front_enable_zone1_ir)
						zone1_ir_enable = 1;
					IR_rep_cnt = 0;
					} 
#ifdef IRREP
				else
					{
					if (repeat_IRkey_allowed(zone1_codeIR))
						{
						if(IR_rep_cnt > 3)
							{
						if(front_enable_zone1_ir)
							zone1_ir_enable = 1;
							}
						else
							{
							IR_rep_cnt++;
							}
						}
					}
#endif
				}		
#if 1//def _DEBUG
			else if(((dataIR >> 6) & 0x1f) == sysIR2code)
				{
				prevctlbit = IR_ctlbit;
				IR_ctlbit = (dataIR >> 11) & 1;
				if (((unsigned int) IR_ctlbit != prevctlbit) || ((dataIR & 0x7F) != zone2_codeIR))
					/*((dataIR & 0x3F) != codeIR) 的判断是为了防止用户按下某一个按键之后，中间过程遥控头没有接收到信号，等遥控头再次接收到信号的时候，翻转位仍然一样，但是用户码已经变了*/
					{
					zone2_codeIR = dataIR & 0x7F;
					if(front_enable_zone2_ir)
						zone2_ir_enable = 1;
					IR_rep_cnt = 0;
					} 
				
#ifdef IRREP
				else
					{
					if (zone2_repeat_IRkey_allowed(zone2_codeIR))
						{
						if(IR_rep_cnt > 3)
							{
							if(front_enable_zone2_ir)
								zone2_ir_enable = 1;
							}
						else
							{
							IR_rep_cnt++;
							}
						}
					}
#endif
				}
#endif
			else
				{
				zone1_ir_enable = 2;	/*其他的客户码*/
				zone1_codeIR = dataIR & 0x3F;
				OthersystemIR = ((dataIR >> 6) & 0x1f);
				}
			stateIR = IR_IDLE;
			}
		}
}

#ifdef IRREP
int zone2_repeat_IRkey_allowed(unsigned char ir_key)
{
	if((ir_key == 0x13) || (ir_key == 0x10))
		return 1;
	return 0;
}
#endif


void ir2_core_philips(unsigned long width,unsigned char falling_edge)
{
	static unsigned short dataIR;
	static unsigned char cntIRbits = 0;
	static unsigned char stateIR = IR_IDLE;
	static unsigned char data_half = 0;  /* Mark whether or nor have half_bit data before edge of intr. coming*/
	static unsigned char IR_ctlbit;
	static unsigned char IR_rep_cnt;

	unsigned char reset;
	unsigned char is_half_bit;
	unsigned char is_full_bit;
	unsigned char prevctlbit;
	reset = 0;

	is_half_bit = (width >= HALFBIT_MIN) && (width <= HALFBIT_MAX);
	is_full_bit = (width >= ONEBIT_MIN)  && (width <= ONEBIT_MAX);

	if (stateIR == IR_IDLE)
		{
		if (falling_edge)
			stateIR = IR_LEADER;
/*		else
			prevIRtime = 0;*/
		}
	else if (stateIR == IR_LEADER)
		{
		/* We shall get a rising edge, since the first bit is fixed */
		dataIR = 0;
		cntIRbits = 0;
		stateIR = IR_CUSTOM;

		if (is_half_bit)
			data_half = 0;
		else if (is_full_bit)
			data_half = 1;
		else
			{
			stateIR = IR_IDLE;
			//prevIRtime = 0;
			}
		}
	else
		{
		if (data_half)
			{
			/* 
			* We were in half bit position, so this edge shall either
			* conclude the previous cycle or go the the half position
			* of the next bit. Record the last bit.
			*/
			dataIR <<= 1;
#ifdef BIT_PHASE_FALLINGEDGE_1
			if (!falling_edge)
				dataIR |= 1;
#else
			if (falling_edge)
				dataIR |= 1;
#endif
			cntIRbits++;

			if (is_half_bit)
				data_half = 0;
			else if (!is_full_bit)
				reset = 1;
			}
		else
			{
			/* 
			* We started at a sampling cycle, so we shall only get half bit,
			* otherwise, something is wrong!
			*/
			if (is_half_bit)
				data_half = 1;
			else
				reset = 1;
			}

		if ((cntIRbits == 12) && !falling_edge && data_half)
			{
			/* This is the last rising edge, no more. So collect the bit */
			dataIR <<= 1;
#ifdef BIT_PHASE_FALLINGEDGE_1
#else
			dataIR |= 1;
#endif
			cntIRbits = 13;
			}

		if (reset)
			{
			/* 
			* Abnormal exist. Maybe we are out of sync. If this
			* is falling edge, maybe this is the sync of a new
			* input!
			*/
			stateIR = IR_IDLE;
			if (falling_edge)
				stateIR = IR_LEADER;
			}
		else if (cntIRbits == 13)	/*接收到一组完整的按键*/
			{
			/* We only care the case when system code matches */
			if (((dataIR >> 6) & 0x1f) == sysIR2code)
				{
				prevctlbit = IR_ctlbit;
				IR_ctlbit = (dataIR >> 11) & 1;
				if (((unsigned int) IR_ctlbit != prevctlbit) || ((dataIR & 0x3F) != zone2_codeIR))
					/*((dataIR & 0x3F) != codeIR) 的判断是为了防止用户按下某一个按键之后，中间过程遥控头没有接收到信号，等遥控头再次接收到信号的时候，翻转位仍然一样，但是用户码已经变了*/
					{
					zone2_codeIR = dataIR & 0x3F;
					zone2_ir_enable = 1;
					IR_rep_cnt = 0;
					} 
#ifdef IRREP
				else
					{
					if (zone2_repeat_IRkey_allowed(zone2_codeIR))
						{
						if(IR_rep_cnt > 3)
							{
							zone2_ir_enable = 1;
							}
						else
							{
							IR_rep_cnt++;
							}
						}
					}
#endif
				}		
			else
				{
				zone2_ir_enable = 2;	/*其他的客户码*/
				zone2_codeIR = dataIR & 0x3F;
				OthersystemIR2 = ((dataIR >> 6) & 0x1f);
				}
			stateIR = IR_IDLE;
			}
		}
}

#endif

#ifdef IR_NEC
unsigned char ir_enable;
void ir_core_nec(unsigned long width,unsigned char falling_edge)
{
	static unsigned char stateIR = IR_IDLE; //add by cjm 
	static unsigned short dataIR; //add by cjm
	static unsigned short codeIR; //add by cjm
	static unsigned char cntIRbits = 0;
	unsigned char reset;
	reset=0;

	if (stateIR == IR_IDLE)
		{
		if (falling_edge)
			stateIR = IR_LEADER_LOW;
//		else
//			prevIRtime = 0;
		}
	else if (stateIR == IR_CUSTOM)
		{
		if(falling_edge)
			{
			dataIR <<= 1;
			if ((width >= DATA_1_MIN) && (width <= DATA_1_MAX))
				{
				dataIR |= 0x1;
				}
			else if ((width < DATA_0_MIN) || (width > DATA_1_MAX))
				{
				reset = 1;
				}

			cntIRbits++;

			/* First 16 bits are syscode */
#if 0
			if ((cntIRbits == 16) && (dataIR != sysIRcode))
				reset = 1;
#endif	//csg
			if (cntIRbits == 24)
				{
#ifdef IRREP
//				previous_data = data;
#endif
				codeIR = dataIR;// | 0x100;	/* Indicate a new code */
				ir_enable = 1;
				}
			else if (cntIRbits == 32)
				reset = 1;
			}
		}
	else if ((stateIR == IR_LEADER_LOW) && (!falling_edge))
		{
		if ((width >= LEADER_LOW_MIN) && (width <= LEADER_LOW_MAX))
			{
			stateIR = IR_LEADER_HIGH;
			}
		else
			reset = 1;
		}
	else if((stateIR == IR_LEADER_HIGH) && (falling_edge))
		{
		/* stateIR has to be equal to IR_LEADER_HIGH */
		if ((width >= LEADER_HIGH_MIN) && (width <= LEADER_HIGH_MAX))
			{
			dataIR = cntIRbits = 0;
			stateIR = IR_CUSTOM;
			}
#ifdef IRREP
		else if ((width >= REPEAT_HIGH_MIN) && (width <= REPEAT_HIGH_MAX))
			{
			/* if the width is 2.25 ms, it is repeat code leader */
			stateIR = IR_REPEAT;
			}
#endif
		else
			reset = 1;
		}
#ifdef IRREP
	else if (stateIR == IR_REPEAT)
		{
		if ((width >= REPEAT_DATA_MIN) && (width <= REPEAT_DATA_MAX))
			{ 
			/* the width is 0.56 ms */
//			if (repeat_IRkey_allowed(previous_data))
//				codeIR = previous_data | 0x100; /* Indicate a new code */
			ir_enable = 1;
			}
		reset = 1;
		}
#endif
	else
		{
		reset = 1;
		}

	if (reset)
		{
		/* Reset all, start from the very beginning */
		stateIR = IR_IDLE; 
//		prevIRtime = 0; 
		}
}
#endif /* IR_NEC */

