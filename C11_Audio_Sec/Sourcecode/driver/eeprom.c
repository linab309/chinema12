#include "eeprom.h"
#include "i2cdrv.h"
#include "ioport.h"

#include "..\main\debug.h"

#ifdef _DEBUG
#define _DEBUG_EEPROM
#endif

#define ADDR_EEPROM 0xA0
const IIC_CONFIG IIC_EEPROM = /*EEPROM IIC ½á¹¹Ìå*/
{
	ADDR_EEPROM,
	set_scl1,
	set_sda1,
	sense_scl1, 
	sense_sda1,

	7,   /* tAA     */		//0.9us
	7,   /* tBUF    */		//1.3us
	10,   /* tHD_STA */	//0.6us
	7,   /* tSU_STA */		//0.6us
	7,   /* tLOW    */		//1.3us
	5,   /* tHIGH   */		//0.6us
	0,   /* tHD_DAT */ 	//0
	1,   /* tSU_DAT */		//0.1us
	5,   /* tR      */		//0.3us
	5,   /* tF      */		//0.3us  
	10,   /* tSU_STO */	//0.6us
	1,   /* tDH     */
	60000,    /* tWR*/

	10,	/* ack timeout */
};  

void EepromWriteByte(unsigned int addr,unsigned char value)
{
	int IIC_Return;
		
	IIC_Return=I2C_bytewrite(IIC_EEPROM,addr,value);
	
#ifdef _DEBUG_EEPROM
	if(IIC_Return==-1)
		debug_printf("\nEEPROM NACK\n");
#endif
}

unsigned char EepromReadByte(unsigned int addr)
{
	return (unsigned char)I2C_byteread(IIC_EEPROM,addr);
}

#ifdef _DEBUG_EEPROM
void FillEepromValue(unsigned char value)
{
	int i;

	for(i=0;i<EEPROM_SIZE;i++) //for(i=161;i<562;i++)
		{
		EepromWriteByte(i,value);
		}
}

void DebugEepromValue()
{
	int i;
	for(i=0;i<EEPROM_SIZE;i++)
		{
		if(i%16 == 0)
			{
			debug_printf("\n%04X:",i);
			}
		debug_printf("%02X ",EepromReadByte(i));
		}
	debug_printf("\n");
}
#endif

