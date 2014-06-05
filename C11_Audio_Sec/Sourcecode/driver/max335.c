#include "max335.h"
#include "ioport.h"
//#include "ic4094.h"
#include "..\main\debug.h"

#ifdef _DEBUG
#define _DEBUG_MAX335
#endif

extern void wait_timer(int timer);

/*MAX335为串联，此数组保存MAX335内部值*//*8片MASS的设置值*/
static unsigned char shadow_max335[MAX335_CHIP_NUMBER] = 
{
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00, 
};

/*MAX335串行发送一个字节数据*/
void Max335SendByte(unsigned char send_value)
{
	unsigned char i;

	SetMax335Clk(0);
	for(i=0;i<8;i++)
		{
		wait_timer(50);
		SetMax335Data(send_value & 0x80);
		wait_timer(50);
		SetMax335Clk(1);
		send_value <<= 1;
		wait_timer(50);
		SetMax335Clk(0);
		}
}

/*MAX335设置某一颗IC的开关，switch_value中包含开关值以及IC的位置值*/
void SetMax335Switch(unsigned int switch_value,unsigned char on)
{
	unsigned char chip;
	unsigned char value;

	chip = (switch_value >> 8) & 0xFF;

	if(chip > MAX335_CHIP_NUMBER)
		{
		return ;
		}

	value = switch_value & 0xFF;

	if(on)
		{
		shadow_max335[chip] |= value;
		}
	else
		{
		shadow_max335[chip] &= (~value);
		}
}

const unsigned char Max335SelTab[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
/*刷新shadow_max335数组中的数据到MAX335芯片中*/
void UpdateMax335()
{
	unsigned char i = 0;
#ifdef _DEBUG_MAX335
	debug_printf("\nMAX335:")	;
#endif
	for(i=0;i<MAX335_CHIP_NUMBER;i++)
		{
#ifdef _DEBUG_MAX335
		debug_printf("%02X ",shadow_max335[i]);
#endif
		SelectMax335(Max335SelTab[i]); /*对应的片选置低*/	
		Max335SendByte(shadow_max335[i]);
		SelectMax335(0xFF); /*片选置高*/ 
		}
}

