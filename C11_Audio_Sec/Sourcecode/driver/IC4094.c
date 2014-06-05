/* @file   IC4094.C
 * @brief IC4094 驱动文件
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 code
 */
#include "ioport.h"
#include "ic4094.h"
#include "..\main\debug.h"

#ifdef _DEBUG
#define _DEBUG_4094
#endif

#define SL_SR_Mas9116_MUTE		0x01
#define HDMI_ISP				0x02
#define HDMI_RST				0x04
#define UATR_SEL				0x08 //0-LPC2366 1-LPC2132
#define ZONE2_MAS9116_MUTE	0x10
#define FL_FR_MAS9116_MUTE	0x20
#define CEN_LFE_MAS9116_MUTE	0x40
#define SBL_SBR_MAS9116_MUTE	0x80

extern void wait_timer(int timer);

/*  
 * @brief 根据IC4094的时序图发送一字节数据
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 code
 */
static void Iec4094Sendbyte(unsigned char send_value)
{
	unsigned char i;

	for(i=0;i<8;i++)
		{
		SetData4094(send_value & 0x80);
		wait_timer(50);
		Set4094Clk(0);
		send_value <<= 1;
		wait_timer(50);
		Set4094Clk(1);
		}
	SetData4094(1);
}

/*
 * @brief 像4094 发送一字节数据 
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 code
 */
void Ext4094Sendbyte(unsigned char send_value)
{
#ifdef _DEBUG_4094
	//debug_printf("EXT4094:%02X\n",send_value);
#endif
	Iec4094Sendbyte(send_value);
	Set4094Cs(1);  
	wait_timer(50);
	Set4094Cs(0);
}

/*
 * @brief 发送给4094 字节中相应位置1或置0
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 code
 */
static unsigned char shadow_ext4094 = 0;
void Ext4094SetBit(unsigned char bit_value)
{
	shadow_ext4094 |= bit_value;
	Ext4094Sendbyte(shadow_ext4094);
}

void Ext4094ClearBit(unsigned char bit_value)
{
	shadow_ext4094 &= ~bit_value;
	Ext4094Sendbyte(shadow_ext4094);
}

/*
 * @brief 4094 字节中D0位置1或置0
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void Set_SLSR_Mas9116_Mute(short high)
{
	if(high) 
		Ext4094SetBit(SL_SR_Mas9116_MUTE); 
	else 
		Ext4094ClearBit(SL_SR_Mas9116_MUTE);
}

/*
 * @brief 4094 字节中D1位置1或置0
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void Set_HDMI_ISR(short high)
{
	if(high) 
		Ext4094SetBit(HDMI_ISP); 
	else 
		Ext4094ClearBit(HDMI_ISP);
}

/*
 * @brief 4094 字节中D2位置1或置0
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void Set_HDMI_RST(short high)
{
	if(high) 
		Ext4094SetBit(HDMI_RST); 
	else 
		Ext4094ClearBit(HDMI_RST);
}

/*
 * @brief 4094 字节中D3位置1或置0
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void Set_UART_SEL(short high)
{
	if(high) 
		Ext4094SetBit(UATR_SEL);  
	else 
		Ext4094ClearBit(UATR_SEL);
}

/*
 * @brief 4094 字节中D4位置1或置0
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void Set_Zone2_Mas9116_Mute(short high)
{
	if(high) 
		Ext4094SetBit(ZONE2_MAS9116_MUTE); 
	else 
		Ext4094ClearBit(ZONE2_MAS9116_MUTE);
}

/*
 * @brief 4094 字节中D5位置1或置0
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void Set_FLFR_Mas9116_Mute(short high)
{
	if(high) 
		Ext4094SetBit(FL_FR_MAS9116_MUTE); 
	else 
		Ext4094ClearBit(FL_FR_MAS9116_MUTE);
}

/*
 * @brief 4094 字节中D6位置1或置0
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void Set_CENLFE_Mas9116_Mute(short high)
{
	if(high) 
		Ext4094SetBit(CEN_LFE_MAS9116_MUTE); 
	else 
		Ext4094ClearBit(CEN_LFE_MAS9116_MUTE);
}

/*
 * @brief 4094 字节中D7位置1或置0
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void Set_SBLSBR_Mas9116_Mute(short high)
{
	if(high) 
		Ext4094SetBit(SBL_SBR_MAS9116_MUTE); 
	else 
		Ext4094ClearBit(SBL_SBR_MAS9116_MUTE);
}

void Set_Mas9116_HardMute(unsigned char mute)
{
	if(mute)
	{
#ifdef _DEBUG
		debug_printf("\nSet_Mas9116_HardMute1111111");
#endif
		Set_FLFR_Mas9116_Mute(0); 
		Set_SLSR_Mas9116_Mute(0);
		Set_CENLFE_Mas9116_Mute(0);
		Set_SBLSBR_Mas9116_Mute(0);
	}
	else
	{
#ifdef _DEBUG
		debug_printf("\nSet_Mas9116_HardMute00000000");
#endif	
		Set_FLFR_Mas9116_Mute(1);
		Set_SLSR_Mas9116_Mute(1);
		Set_CENLFE_Mas9116_Mute(1);
		Set_SBLSBR_Mas9116_Mute(1);
	}
}

/*
 * @brief LPC2132 hardware reset
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void LPC2132HwReset(void)
{
	Set_HDMI_RST(0);
	DelayMs(10);
	Set_HDMI_RST(1);
}

/*
 * @brief 4094 上电后D0-D7默认值
 *
 * @author chenjianming
 * @date 2009-03-20
 * @refer 
 */
void IC4094_Default(void)
{
	Set4094OE(1); /*默认下设置高电平*/
	Set_SLSR_Mas9116_Mute(1); 
	Set_HDMI_ISR(1); /*lpc2132默认下高电平*/
	Set_HDMI_RST(1);
	Set_UART_SEL(0); /*0-LPC2366;1-LPC2132*/
	Set_Zone2_Mas9116_Mute(1); 
	Set_FLFR_Mas9116_Mute(1);
	Set_CENLFE_Mas9116_Mute(1);
	Set_SBLSBR_Mas9116_Mute(1);
}

/*
* @brief HDMI 软件升级操作
*
* @author chenjianming
* @date 2009-3-20
*/
void HDMI_Software_Update(void)
{
	SetCurFocusTid(); 
	Set_HDMI_ISR(0);  
	LPC2132HwReset();
	Set_UART_SEL(1);  
	while(1); /*升级完后死机,让客户自己拔电*/
}

#if 0
/*
 * @brief 像4094 发送一字节数据 
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 code
 */
void Channel4094Sendbyte(unsigned char send_value)
{
#ifdef _DEBUG_4094
	debug_printf("Vol4094:%02X\n",send_value);
#endif
	Iec4094Sendbyte(send_value);
	Set4094Cs(1);
	wait_timer(50);
	Set4094Cs(0);
}

/*
 * @brief 像4094 发送一字节数据 
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 code
 */
void Vol4094Sendbyte(unsigned char send_value)
{
#ifdef _DEBUG_4094
	debug_printf("Vol4094:%02X\n",send_value);
#endif
	Iec4094Sendbyte(send_value);
	Set4094Cs(1);
	wait_timer(50);
	Set4094Cs(0);
}
#endif

