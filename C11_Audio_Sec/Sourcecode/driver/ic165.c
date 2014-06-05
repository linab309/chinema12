#include "..\main\macro.h"
#include "ioport.h"
#include "ic165.h"

/* 
 * @brief  74LS165 检测前置面板按键驱动程序
 *
 * @author chenjianming
 * @date 2007--
 * @refer 
 */
#define KEY_MASK 0xFFFFFFFF
uint32 read165()  
{
	uint32 rtn_key = 0;
	unsigned char i;

	//set_ic165_ld(1);
	set_ic165_ld(0);
	set_ic165_ld(1);	/*load the parallel data*/ /*LD从低到高锁住数据*/
	//for(i = 0; i < 24; i++) /*读取24位并行数据*/
	for(i=0;i<32;i++)
		{
		rtn_key <<= 1;
		set_ic165_clk(0);
		if(sense_ic165_data())
			{
			rtn_key |= 1;
			}
		set_ic165_clk(1);
		}
	rtn_key = ~rtn_key;/*低电平表示按键按下-取反后变高电平表示按键按下*/
	rtn_key &= KEY_MASK;
	
	return rtn_key;
}

