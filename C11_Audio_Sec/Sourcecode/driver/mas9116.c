#include "mas9116.h"
#include "ioport.h"
#include "..\main\debug.h"
#include "..\main\macro.h"

#ifdef _DEBUG
//#define _DEBUG_MAS9116
#endif

#define MAX9116_DELAY

#ifdef MAX9116_DELAY
extern void wait_timer(int timer);
#endif

/* 
 * @brief 向Mas9116 写数据
 *
 * @author chenjianming
 * @date 2007-10-10
 * @refer chenshiguang Cll code
 */
static void Mas9116WriteWord(int word_value)
{
	unsigned char i;
	for(i=0;i<16;i++)  
		{  
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		SetMas9116Clk(0);
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		SetMas9116Data(word_value & 0x8000);
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		SetMas9116Clk(1);
		word_value <<= 1;
		}
#ifdef MAX9116_DELAY
	wait_timer(200);
#endif
}

/* 
 * @brief 从Mas9116 读数据
 *
 * @author chenjianming
 * @date 2010-3-15
 * @refer  
 */
uchar Mas9116Read(void)
{
	uchar i=0;
	uchar Reg_value=0;
	uchar Read_Value=0;

	Reg_value=0xFF; //CR5 Regist
	//Reg_value=0xF7; //CR1 Regist
	for(i=0;i<8;i++)  
	{  
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		SetMas9116Clk(0);
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		SetMas9116Data(Reg_value & 0x80);
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		SetMas9116Clk(1);
		Reg_value <<= 1;
	}

	IODIR0 &=(~MAS9116_DATA); //设置为输入,开始读数据
#ifdef MAX9116_DELAY
	wait_timer(200);
#endif	
	for(i=0;i<8;i++)
	{
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		SetMas9116Clk(0);
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		Read_Value<<=1;
		if(SensePort0(MAS9116_DATA))
		{
			Read_Value|=0x01;
		}
#ifdef MAX9116_DELAY
		wait_timer(200);
#endif
		SetMas9116Clk(1);
	}
	
	return Read_Value;
}

/* 
 * @brief mas9116 音量计算
 *
 * @author chenjianming
 * @date 2007-10-10
 * @refer chenshiguang Cll code
 */
unsigned char CalcMas9116Volume(float volume)	/*mas9116 音量计算公式(0xE0 + (2*x))*/
{
	unsigned char rtn_value;

	if(volume>15) //Add by cjm 
		volume=15;  
	
	volume *= 2;
	rtn_value = volume + 0xE0;
	return rtn_value;
}

/* 
 * @brief mas9116 setting gain
 *
 * @author chenjianming
 * @date 2007-10-10
 * @refer chenshiguang Cll code
 */
void Mas9116SetGain(unsigned char chip_channel,float vol)
{
	unsigned short reg_value = 0;
	unsigned char send_value = 0;

	send_value = CalcMas9116Volume(vol);

	switch(chip_channel & 0xF0)
		{
		case MAS9116_LEFT:	/*left*/
			reg_value = 0xEB00 | send_value;
			break;
		case MAS9116_RIGHT:	/*right*/
			reg_value = 0xF300 | send_value;
			break;
		case MAS9116_BOTH:	/*both channel*/
		default:
			reg_value = 0xCB00 | send_value;
			break;
		}
	
	SetMas9116Clk(1);

#ifdef MAX9116_DELAY  
	wait_timer(300);
#endif

	SelectMas9116(chip_channel&0x0F); /*片选置低开始写数据*/
	Mas9116WriteWord(reg_value);
	SelectMas9116(0xFF);  /*片选置高数据写完*/
	
#ifdef MAX9116_DELAY
	wait_timer(100);
#endif

	SetMas9116Clk(0);
	SetMas9116Data(0); //add cjm 2008-3.31 (mas9116 芯片时序)
	SetMas9116Clk(1); //add cjm 2008-3.31 (mas9116 芯片时序)
}

uchar Mas9116ReadRC5(unsigned char chip_channel)
{
	uchar return_value;


       return 0;//test  jian add in 10-09-16
        
	SetMas9116Clk(1);
#ifdef MAX9116_DELAY  
	wait_timer(300);
#endif

	SelectMas9116(chip_channel&0x0F); /*片选置低开始读数据*/
	return_value=Mas9116Read();
	SelectMas9116(0xFF);  /*片选置高数据读完*/

#ifdef MAX9116_DELAY
	wait_timer(100);
#endif
	IODIR0 |= MAS9116_DATA; /*Input change to OUTPUT*/
	SetMas9116Clk(0);
	SetMas9116Data(0); //add cjm 2008-3.31 (mas9116 芯片时序)
	SetMas9116Clk(1); //add cjm 2008-3.31 (mas9116 芯片时序)

	return return_value;
}

/* 
 * @brief mas9116 Init
 * @author chenjianming
 * @date 2007-10-10
 * @note 由于Mas9116上电复位后,寄存器值(RC5)会飞掉,故上电后重新初始化。
 */
 
void Mas9116Reset(void)
{
	unsigned short reg_value;

	reg_value=0xFB00; /*RC5寄存器设置为0x00*/

	SetMas9116Clk(1);
	wait_timer(100);
	SetMas9116Cs0(0);	SetMas9116Cs1(0); /*Mas9116全部选中*/
	SetMas9116Cs2(0);    SetMas9116Cs3(0);
	SetMas9116Cs4(0);	SetMas9116Cs5(0);
				
	Mas9116WriteWord(reg_value);

	wait_timer(100);
	SetMas9116Cs0(1);  	SetMas9116Cs1(1);  /*Mas9116全部释放*/
	SetMas9116Cs2(1); 	SetMas9116Cs3(1);
	SetMas9116Cs4(1);	SetMas9116Cs5(1);		
	
	SetMas9116Clk(0);
	SetMas9116Data(0); //add cjm 2008-3.31 (mas9116 芯片时序)
	SetMas9116Clk(1); //add cjm 2008-3.31 (mas9116 芯片时序)  
}

#if 0
void Debug_Mas9116(void)
{
	unsigned short reg_value = 0;
	unsigned char send_value = 0;

	debug_printf("\n2 Channel Volume Debug");
	send_value = CalcMas9116Volume(1);
	reg_value = 0xCB00 | send_value;

	SetMas9116Clk(0);
	SetMas9116Cs0(0);
	Mas9116WriteWord(reg_value);
	SetMas9116Cs0(1);
	SetMas9116Clk(0);
}
#endif

