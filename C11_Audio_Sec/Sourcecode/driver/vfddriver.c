#include "..\main\macro.h"

#include "vfddriver.h"
#include "i2cdrv.h"
#include "ioport.h"

#include "..\main\task.h"
#include "..\main\debug.h"
#include "..\main\vfd_disp.h"

#ifdef _DEBUG
#define _DEBUG_Vfddriver
#endif

#define ADDR_VFD 0x50
IIC_CONFIG const IIC_VFD =  /*VFD IIC 结构体*/
{
	ADDR_VFD,
	set_vfd_scl,
	set_vfd_sda,
	sense_vfd_scl,
	sense_vfd_sda,

	12,//10,//7,   /* tAA     */
	12,//10,//7,   /* tBUF    */
	12,//8,//5,   /* tHD_STA */
	12,//10,//7,   /* tSU_STA */
	12,//10,//7,   /* tLOW    */
	10,//8,//5,   /* tHIGH   */  
	5,//2,//0,   /* tHD_DAT */
	5,//3,//1,   /* tSU_DAT */
	5,//3,//1,   /* tR      */
	5,//3,//1,   /* tF      */  
	12,//10,//7,   /* tSU_STO */
	5,//3,//1,   /* tDH     */
	25,//20,//10,   /* tWR*/

	25,//20,//10,	/* ack timeout */
};

/*向VFD 送多字节数据*/
void VfdSendNByte(unsigned char *buffer,short num)
{
	int IIC_Return;
	
	IIC_Return=I2CWriteNByte(IIC_VFD,num,buffer);
	
#ifdef _DEBUG_Vfddriver
	if(IIC_Return==-1)
		debug_printf("\nVfdSendNByte NACK\n");
#endif
}

/*向VFD 送一字节数据*/
void VfdSendByte(unsigned char value)
{
	int IIC_Return;
	
	IIC_Return=I2CWriteNByte(IIC_VFD,1,&value);
	
#ifdef _DEBUG_Vfddriver
	if(IIC_Return==-1)
		debug_printf("\nVfdSendByte NACK\n");
#endif
}

/*刷新VFD 显示缓冲区*/
void VfdUpdateBuffer(unsigned char mode,char *buffer)
{
	//unsigned char i;
	switch(mode)
		{
		case VFD_UPDATE_LINE1:  /*更新显示屏第一行*/
		         //VfdSendByte('\v'); /*发送两次，确保不会丢失*/
			  VfdSendByte('\v');   /*\r--显示屏光标指针移到home*/
#if 0
			for(i = VFD_LINE1_START_POSITION;i<VFD_LINE1_LENGTH;i++)
				{
				VfdSendByte(buffer[VFD_LINE1_START_POSITION + i]);
				}
#else
			VfdSendNByte(buffer+VFD_LINE1_START_POSITION,VFD_LINE1_LENGTH);
#endif
			break;
		case VFD_UPDATE_LINE2:  /*更新显示屏第二行*/
		        //VfdSendByte(0x07);/*发送两次，确保不会丢失*/
			 VfdSendByte(0x07);  /*0x07--可以直接切换到第二行*/
#if 0
			for(i = 0;i<VFD_LINE2_LENGTH;i++)
				{
				VfdSendByte(buffer[VFD_LINE2_START_POSITION + i]);
				}
#else
			VfdSendNByte(buffer+VFD_LINE2_START_POSITION,VFD_LINE2_LENGTH);
#endif
			break;
		case VFD_UPDATE_LINE_ALL: /*两行全部更新*/
			VfdSendByte('\v');         /*\r--显示屏光标指针移到home*/
#if 0
			for(i = VFD_LINE1_START_POSITION;i<VFD_MAX_POSITION;i++)
				{
				VfdSendByte(buffer[VFD_LINE1_START_POSITION + i]);
				}
#else
			VfdSendNByte(buffer+VFD_LINE1_START_POSITION,VFD_MAX_POSITION);
#endif
			break;
		default:
			break;
		}
}

/*VFD 屏亮度调节*/
const unsigned char dimer_table[] = {0x0,0x05,0x25,0x95};
void VfdSetDimerValue(unsigned char value)
{
	if(value < 4)
		{
		VfdSendByte(0x1F);
		VfdSendByte(0x58);
		VfdSendByte(dimer_table[value]);
		}
}

