#include "..\main\macro.h"
#include "..\main\task.h"
#include "..\main\debug.h"

#include "cs8416.h"
#include "i2cdrv.h"
#include "ioport.h"

#ifdef _DEBUG
#define _DEBUG_CS8416  
#endif

#define ADDR_CS8416 0x20
IIC_CONFIG const IIC_CS8416 = /*CS8416 IIC 通讯结构体*/
{
	ADDR_CS8416,
	set_vfd_scl,
	set_vfd_sda,
	sense_vfd_scl,
	sense_vfd_sda,

	7,   /* tAA     */
	7,   /* tBUF    */
	25,   /* tHD_STA */
	15,   /* tSU_STA */
	7,   /* tLOW    */
	15,   /* tHIGH   */
	0,   /* tHD_DAT */
	1,   /* tSU_DAT */  
	10,   /* tR      */
	5,   /* tF      */
	25,   /* tSU_STO */
	1,   /* tDH     */  	
	10,    /* tWR*/
	
	10,	/* ack timeout */   
};

/*
 * @brief 向CS8416 写数据
 *
 * @author chenjianming
 * @date 2007-07-10
 * @refer chenshiguang Cll code
 */
void Cs8416WriteByte(unsigned char addr,unsigned char value)
{
	int IIC_Return;
	
	IIC_Return=I2C_bytewrite(IIC_CS8416,addr,value);
	
#ifdef _DEBUG_CS8416
	if(IIC_Return==-1)
		debug_printf("\nCs8416WriteByte NACK\n");
#endif 
}

/*
 * @brief 读CS8416 寄存器
 *
 * @author chenjianming
 * @date 2007-07-10
 * @refer chenshiguang Cll code
 */
unsigned char Cs8416ReadByte(unsigned char addr)
{
	return (unsigned char)I2C_byteread(IIC_CS8416,addr);
}

/*
 * @brief 返回当前Digital Stream Freq
 *
 * @author chenjianming
 * @date 2007-07-10
 * @refer chenshiguang Cll code
 */
unsigned char Cs8416GetFreq(void)
{
	uchar i;
	i = Cs8416ReadByte(REG8416_CLK_RATIO);

#ifdef _DEBUG_CS8416
//if(i> 0x42*2)
//	debug_printf("\n%02X",i);
#endif
	
	if(i < 0x1e*2) /*change by cjm 2008-7-3 (晶振由12.288改为24.576)*/
		{
		i = FREQ_192KHZ;
#ifdef _DEBUG_CS8416
	//	debug_printf("\n192khz\n");
#endif
		}
	else if(i < 0x22*2) 
		{
		i = FREQ_96KHZ;
#ifdef _DEBUG_CS8416
	//	debug_printf("\n96khz\n");
#endif
		}
	else if(i < 0x42*2) 
		{
		i = FREQ_48KHZ;
#ifdef _DEBUG_CS8416		  
	//	debug_printf("\n48khz\n");
#endif
		}
	else if(i < 0x48*2) 
		{
		i = FREQ_44_1KHZ;
#ifdef _DEBUG_CS8416			
	//	debug_printf("\n44.1khz\n");
#endif
		}
	 else 
	 	 i = 0x7f;
	return i;
}

/*
 * @brief 判断当前数字流是否失锁
 *
 * @author chenjianming
 * @date 2007-07-10
 * @refer chenshiguang Cll code
 */
unsigned char CheckCs8416Unlock(unsigned char ms_value)
{
	unsigned char temp_value,i;
	for(i=0;i<ms_value;i++)
		{
		temp_value = Cs8416ReadByte(REG8416_RECV_ERR);
		if(temp_value & 0x10)	/*unlock*/
			{
			DelayMs(1);
			}
		else
			{
#ifdef _DEBUG_CS8416
			//debug_printf("\nLock\n");
#endif
			return 0;	/*Lock*/	
			}
		}
#ifdef _DEBUG_CS8416
	//debug_printf("\nUnLock\n");
#endif
	return 1;	/*unlock*/
}

unsigned char IsCs8416Unlock(void)
{
	static unsigned char error_count = 0;
	unsigned char temp_value;
	temp_value = Cs8416ReadByte(REG8416_RECV_ERR);
	if(temp_value & 0x10)	/*unlock*/
		{
		error_count ++;
		if(error_count > 10)	/*一个类似按键消抖的过程*/
			{
			error_count = 11;
			return 1;
			}
		}
	else
		{
		error_count = 0;
		}

	return 0;
}



unsigned char IsCs8416Pcm(void)
{
	static unsigned char error_count = 0;
	unsigned char temp_value;
	temp_value = Cs8416ReadByte(REG8416_DETECT_STATUS);
	if(temp_value & 0x40)	/*pcm*/
		{
		error_count ++;
		if(error_count > 10)	/*一个类似按键消抖的过程*/
			{
			error_count = 11;
			return 1;
			}
		}
	else
		{
		error_count = 0;
		}

	return 0;
}



/*
 * @brief CS8416初始化
 *
 * @author chenjianming
 * @date 2007-07-10
 * @refer chenshiguang Cll code
 */
void InitCs8416()
{
	Cs8416WriteByte(REG8416_CNTL0,0x00);	  
	Cs8416WriteByte(REG8416_CNTL1,0x84);  	
	Cs8416WriteByte(REG8416_SER_AUD_FMT,0x85);	/*设置I2S模式，左对齐*/
	Cs8416WriteByte(REG8416_REC_ERR_MASK,0x10);	
	Cs8416WriteByte(REG8416_CNTL4,0x98); /*Channel Setting*/	
	
#ifdef _DEBUG_CS8416
	debug_printf("\n intiCs8416\n");
#endif
	
}



