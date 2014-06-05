/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                Idm.c											  					**
**																			  				**
** DESCRIPTION: 		IDM Interface Functions												**
**********************************************************************************************/
#include "processor.h"

#ifdef IDM_I2C
#include "Idm.h"

#ifdef _ATMEL
#include "avr_i2c_drv.h"
#include "avr_timer_drv.h"
#elif defined(_PC)
#endif
#include "globals.h"


/******************************************************************
 *
 *      IdmI2CWrite
 *
 *      Description : IdmI2CWrite
 *
 *      Input	    : UINT8 SlaveAddress,UINT16 WriteLen,UINT8 *WriteBuffer
 *      Output      : UINT8
 *
 ******************************************************************/
UINT8 IdmI2CWrite(UINT8 SlaveAddress,
                          UINT16 WriteLen,
                          UINT8 *WriteBuffer)
{
        return I2C_Write(SlaveAddress, WriteLen, WriteBuffer);
}

/******************************************************************
 *
 *      IdmI2CRead
 *
 *      Description : IdmI2CRead
 *
 *      Input	    : SlaveAddress, output buffer pointer
 *      Output      : Copies assembled packet up to 256 bytes to buffer
 *
 ******************************************************************/
UINT8 IdmI2CRead(UINT8 SlaveAddress,
                                 UINT8 *Buffer)

{
    UINT8 I2C_Read_Status;
    UINT16 i;
    UINT16 IDM_Read_Len;
    UINT8 FirstBC;


    for (i=0; i<=10; i++)
    {
        Buffer[i]= 0xFF;
    }

    //read byte count
    I2C_Read_Status = I2C_Read(SlaveAddress,
                0x01,
                1,
                Buffer);


    if (I2C_Read_Status != I2C_OK) return 0;

        Timer_TickDelay(2);

    FirstBC = *Buffer;
    IDM_Read_Len = FirstBC+1;
    if ( (*Buffer > 0x00) )
    {
        //read byte count + available data
        I2C_Read_Status = I2C_Read(SlaveAddress,
                        0x01,
                        IDM_Read_Len,
                        &Buffer[0]);

                if (I2C_Read_Status != I2C_OK) return 0;
        }

//        if(Buffer[0] != Buffer[3] ) return 0;

    return 1;

}
#endif

