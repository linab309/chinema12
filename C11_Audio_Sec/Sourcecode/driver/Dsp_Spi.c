#include "ioport.h"
#include "Dsp_Spi.h"
#include "..\cs495300\Dspdrv.h"
#include "..\main\debug.h"
#include "..\main\macro.h"
#include "..\main\task.h"
#include "..\driver\channel.h"

#ifdef _DEBUG
#define _DEBUG_SPI_DATA
#endif

#define SPI_BIT_DELAY_NUM 			1
#define SPI_BYTE_DELAY_NUM 		2
#define STOP_READING       			0
#define KEEP_READING       			1
#define DSP_WRITE					0x80
#define DSP_READ					0x81

void Delay_SPI(int n)
{
	int i;

	do{
		for(i=100;i>0;i--) 
			{}
		}while(n --);
}

/*
 * @brief 模拟SPI 时序写数据
 *
 * @author chenjianming
 * @date 2008-05-27
 * @refer chengshiguang C11 code
 */
static void Write_Byte(unsigned char uData)
{
	unsigned char bit_number;
	  
#ifdef _DEBUG_SPI_DATA
	//debug_printf("%02X ",uData);
#endif

	Set_Spi_Clk(0);
	//Delay_SPI(SPI_BIT_DELAY_NUM); //Dele by cjm 2009-2-4
	for(bit_number = 0;bit_number < 8;bit_number++)
		{
		Set_Spi_MOSI(uData & 0x80);
		//Delay_SPI(SPI_BIT_DELAY_NUM); //Dele by cjm 2009-2-4
		Set_Spi_Clk(1);
		uData <<= 1;
		//Delay_SPI(SPI_BIT_DELAY_NUM); //Dele by cjm 2009-2-4
		Set_Spi_Clk(0);
		//Delay_SPI(SPI_BIT_DELAY_NUM); //Dele by cjm 2009-2-4
		}
	return;
}

/*
 * @brief 根据SPI 时序读数据
 *
 * @author chenjianming
 * @date 2008-05-27
 * @refer chengshiguang C11 code
 */
static char ReadByteDsp(unsigned char *pbData)
{
	char bit_number;	
	char more_data = KEEP_READING;
	*pbData = 0;

	for (bit_number=0;bit_number < 8;bit_number++)
		{
		Set_Spi_Clk(1);
		if (bit_number == 7)
			{
			if(SenseDspIrq())
				more_data=STOP_READING;  
			}
		(*pbData) <<= 1;
		if(Sense_Spi_MISO())
			{
			(*pbData) |= 0x01;
			}
		Set_Spi_Clk(0);	 /* ensure that clock is left low */
		} /* bit read loop */
	return more_data;
}

extern unsigned char Pre_value;
extern unsigned long last_stream;
extern unsigned char SetDspPower(unsigned char mode);
void DspSpiWriteNByte(unsigned char *message,unsigned int size)
{
	unsigned int index;
	unsigned char loop=1;
	unsigned char Busy_time=0;
	extern uchar new_stream_flag ;
		
	Set_Spi_Clk(0);
	Set_Spi_Cs(0);
	//Delay_SPI(SPI_BYTE_DELAY_NUM); //Dele by cjm 2009-2-4

	Write_Byte(DSP_WRITE); /*Write address byte 0x80*/      
	//Delay_SPI(SPI_BYTE_DELAY_NUM); //Dele by cjm 2009-2-4

	for(index=0;index < size;index++)
		{
		Write_Byte(message[index]);    

		//if((index+1)%4==0)
		if(((index+1)%4==0)&&((size-(index))>0)) /*当写完4字节,且还有数据要写就要判断DSP_BUSY引脚*/
			{					
			while(loop)	  
				{
				if(SenseDspBusy())
					{
					loop=0;
					Busy_time=0;
					}
				else
					{
#ifdef _DEBUG_SPI_DATA
//					debug_printf("\nDSP_BUSY PIN is low");	
#endif
					Busy_time++;
					if(Busy_time>100)  
						{    
#ifdef _DEBUG_SPI_DATA
						debug_printf("\nDSP_BUSY Busy time is up to 100 ms");	
#endif						
						loop=0; /*t跳出死循环*/
						Busy_time=0;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
#ifdef _DEBUG_SPI_DATA
							debug_printf("\nWhen DSP is busy,the Input is HDMI");	
#endif								
							Pre_value=8; /*DSP 重新复位*/
							last_stream=0; /*清掉当前流信息*/   
							SetDspPower(0); /*DSP 保持复位状态*/	

							}
						else
							{
#ifdef _DEBUG_SPI_DATA
							debug_printf("\nWhen DSP is busy,the Input is Spdif");	   
#endif								
							Pre_value=8; /*DSP 重新复位*/
							SetDspPower(1); /*DSP 重新开始引导*/							
							}
						}
					DelayMs(1);  
					}			
				}
			}		  	  
		//Delay_SPI(SPI_BYTE_DELAY_NUM); //Dele by cjm 2009-2-4
		}
	Set_Spi_Cs(1);
	//Delay_SPI(SPI_BYTE_DELAY_NUM); //Dele by cjm 2009-2-4
}

unsigned char DspSpiReadNByte(unsigned char *bArray, unsigned char uBufSize)
{
	int offset = 0;
	int not_end_of_read=1;
	unsigned char uAddrAndCmd;

	do{
		if(!SenseDspIrq())
			break;

		else
			 DelayMs(20);
#ifdef _DEBUG_SPI_DATA				
//			debug_printf("\nDSP IRQ is Still hight)");
#endif
		}while(1);	

	Set_Spi_Clk(0);
	Set_Spi_Cs(0);
	Delay_SPI(SPI_BYTE_DELAY_NUM); 

	Write_Byte(DSP_READ);
	Delay_SPI(SPI_BYTE_DELAY_NUM);   
	offset = 0;
	while (not_end_of_read) 
		{
		not_end_of_read = ReadByteDsp(&uAddrAndCmd);    
		if(offset < uBufSize)
			{
			bArray[offset] = uAddrAndCmd;
			}
		else
			{
			break;
			}
		++offset;
		}
	
	Set_Spi_Cs(1);
	Delay_SPI(SPI_BYTE_DELAY_NUM);
	return offset;	
}

