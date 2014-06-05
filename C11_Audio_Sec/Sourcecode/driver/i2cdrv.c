#include "i2cdrv.h"
#include "..\main\debug.h"

#ifdef _DEBUG
#define _DEBUG_I2C_DRV
#endif

#define BIG_EEPROM

#define TRY_COUNT		 3	/* retry if no ack */
#define ack_failure 	 	-1
#define ack_success  	   	 0

static char timeout = 0;		/* acknowledge timeout    */
static char need_to_stop = 0; 	/* stop if no ack 	 */

/****************************************************
延时程序，必须让输入的timer单位为1us
*****************************************************/
void wait_timer(short timer)
{
	while(timer--)
		{}
}

#ifdef _CHECK_SCL_HIGH	/*检测SCL是否已经为高*/
static void i2c_raise_scl(IIC_CONFIG *cptr)
{
	cptr->SCL_set(1);
/*
	while (1)
		{
		if (cptr->SCL_sense())
			break;
		}
*/
}
#endif


/****************************************************
 i2c_ack:
        I2C acknowledge of byte transfer.
 
        arguments:	
		*cptr	ptr to the current device parameters.
		mode	0 (no ack needed)		
				1 (get ack)
				2 (give ack)
        return:    
		 0		okay (got ack)
		-1		error (no ack)
*****************************************************/
static short i2c_ack(IIC_CONFIG *cptr, unsigned char mode)
{
	short tmp,k = -1;

	if (!need_to_stop)
		{
		switch(mode)
			{
			case 1:	/* get acknowledge */
				cptr->SDA_set(1);  /* SDA high */			
#ifndef CHEAT
#ifdef REDUCE_TIME
				wait_timer((cptr->tSU_DAT));	/* SDA rise time and setup time */
#else
				wait_timer((cptr->tR)+(cptr->tSU_DAT)); /* SDA rise time and setup time */
#endif
#endif
				wait_timer(cptr->tLOW);	/* SCL low time */
#ifdef _CHECK_SCL_HIGH
				i2c_raise_scl(cptr);
#else
				cptr->SCL_set(1);
#endif

				wait_timer(cptr->tHIGH);	/* SCL rise time and high time */
				while (1)
					{
					if (!(cptr->SDA_sense()))	/*低电平为ACK*/
						{
						k = 0;
						break;		/* got acknowledge */
						}
					else
						{
						k = -1;
						wait_timer(1);
						tmp ++;
						if (tmp >= cptr->ack_wtime)
							{
							timeout++;
							break;		/* acknowledge timeout */							
							}
						}
					}
				break;
			case 2:	/* acknowledge is needed */
				wait_timer(cptr->tLOW);	/* SCL low time */

				cptr->SDA_set(0);		/* SDA low */   
#ifdef REDUCE_TIME
				wait_timer((cptr->tSU_DAT));	/* SDA fall time and setup time */
#else
				wait_timer((cptr->tF)+(cptr->tSU_DAT));	/* SDA fall time and setup time */
#endif
				wait_timer(cptr->tLOW);	/* SCL low time */

				/* clock cycle for acknowledge */
#ifdef _CHECK_SCL_HIGH
				i2c_raise_scl(cptr);	/* SCL high */
#else
				cptr->SCL_set(1);
#endif

#ifdef REDUCE_TIME
				wait_timer((cptr->tHIGH));	/* SDA and SCL rise time + SCL high time */
#else
				wait_timer((cptr->tR)+(cptr->tHIGH));	/* SDA and SCL rise time + SCL high time */
#endif
				break;
			default:	/* no acknowledge is needed */
#ifdef _CHECK_SCL_HIGH
				i2c_raise_scl(cptr);                /* SCL high */   
#else
				cptr->SCL_set(1);
#endif

#ifdef REDUCE_TIME
				wait_timer((cptr->tHIGH));	/* SDA and SCL rise time + SCL high time */
#else
				wait_timer((cptr->tR)+(cptr->tHIGH));	/* SDA and SCL rise time + SCL high time */
#endif
				break;
			}

		cptr->SCL_set(0);		/* SCL low */
#ifdef REDUCE_TIME
		wait_timer(cptr->tLOW);	/* SCL fall time + SCL low time */
#else
		wait_timer((cptr->tF)+(cptr->tLOW));	/* SCL fall time + SCL low time */
#endif

		/* For mode 2, need to tristate SDA again. MY20010314 */
		if( mode == 2 )
			{
			cptr->SDA_set(1);		/* SDA tristate */   
			wait_timer((cptr->tR));	/* SCL rise time */
			}
		}
	return(k);
}

/****************************************************
  I2C start condition.

  arguments:
	*cptr	ptr to the current device parameters.
	
  return:
*****************************************************/
static void i2c_start(IIC_CONFIG *cptr)
{

	if (!need_to_stop)
		{
		cptr->SDA_set(1);			/* SDA high */
#ifndef CHEAT
		wait_timer(cptr->tLOW);			/* SCL low time */
#endif
#ifdef _CHECK_SCL_HIGH
		i2c_raise_scl(cptr);			/* SCL high */
#else
		cptr->SCL_set(1);
#endif
#ifdef REDUCE_TIME
		wait_timer((cptr->tSU_STA));	/* SCL rise time + start condition setup time */
#else
		wait_timer((cptr->tR)+(cptr->tSU_STA));	/* SCL rise time + start condition setup time */
#endif
		cptr->SDA_set(0);			/* SDA low */
#ifdef REDUCE_TIME
		wait_timer((cptr->tHD_STA));	/* SDA fall time + start condition hold time */
#else
		wait_timer((cptr->tF)+(cptr->tHD_STA));	/* SDA fall time + start condition hold time */
#endif
		cptr->SCL_set(0);			/* SCL low */
#ifndef CHEAT
#ifdef REDUCE_TIME
		wait_timer((cptr->tLOW));	/* SCL fall time + SCL low time */
#else
		wait_timer((cptr->tF)+(cptr->tLOW));	/* SCL fall time + SCL low time */
#endif
#endif
		}
}

/****************************************************
 i2c_stop:
        I2C stop condition.
 
        arguments:
		*cptr	ptr to the current device
			parameters.

        return:    
*****************************************************/
static void i2c_stop(IIC_CONFIG *cptr)
{
	need_to_stop = 0;			/* reset "need_to_stop" */

	cptr->SDA_set(0);			/* SDA low */
#ifndef CHEAT
	wait_timer(cptr->tLOW);		/* SCL low time */
#endif
#ifdef _CHECK_SCL_HIGH
	i2c_raise_scl(cptr);			/* SCL high */
#else
	cptr->SCL_set(1);
#endif

#ifdef REDUCE_TIME
	wait_timer((cptr->tSU_STO));	/* SCL rise time + stop condition setup time */
#else
	wait_timer((cptr->tR)+(cptr->tSU_STO));	/* SCL rise time + stop condition setup time */
#endif
	cptr->SDA_set(1);			/* SDA high */
#ifndef CHEAT
	wait_timer(cptr->tR);			/* SDA rise time */
#endif
}

/****************************************************
 i2c_writebyte:
        Write a byte to I2C bus.
 
        arguments:
		*cptr	ptr to the current device
			parameters.
		c	a byte to be written			

        return:    
*****************************************************/
static void i2c_writebyte(IIC_CONFIG *cptr, unsigned char c)
{
	unsigned char i;

	if (!need_to_stop)
		{
		for (i = 0; i < 8; i++)
			{
			wait_timer(cptr->tLOW);		/* SCL low time */
			if (c & 0x80)	/* most significant bit */
				{
				cptr->SDA_set(1);		/* SDA high */
				}
			else
				{
				cptr->SDA_set(0);		/* SDA low */
				}
#ifndef CHEAT
			wait_timer(cptr->tSU_DAT);	/* data in setup time */
#endif
#ifdef _CHECK_SCL_HIGH
			i2c_raise_scl(cptr);			/* SCL high */
#else
			cptr->SCL_set(1);
#endif

#ifdef REDUCE_TIME
			wait_timer((cptr->tHIGH));	/* SCL rise time +	SCL high time */
#else
			wait_timer((cptr->tR)+(cptr->tHIGH));	/* SCL rise time +	SCL high time */
#endif
			cptr->SCL_set(0);				/* SCL low */
#ifndef CHEAT
#ifdef REDUCE_TIME
			wait_timer((cptr->tHD_DAT));	/* SDA and SCL fall time + data in hold time */
#else
			wait_timer((cptr->tF)+(cptr->tHD_DAT));	/* SDA and SCL fall time + data in hold time */
#endif
#endif
			c <<= 1;
			}
			
		//i2c_ack(cptr, 1);				  /* get acknowledge */ /*change by cjm 2007-08-30*/		
		if(i2c_ack(cptr, 1)==ack_failure) /*写完每字节看是否有应答*/
			{
#ifdef _DEBUG_I2C_DRV
				debug_printf("no ack all together\n");	
#endif
			}

		if (timeout)
			need_to_stop = 1;
		else	
			need_to_stop = 0;
		}
}

/****************************************************
 i2c_readbyte:
        Read a byte from the I2C bus. 
 
        arguments:
		*cptr 	ptr to the current device
			parameters.
        return:    
		b		byte read
*****************************************************/
static unsigned char i2c_readbyte(IIC_CONFIG *cptr)
{
	unsigned char i;
	unsigned char rtn_value;

	rtn_value = 0;

	cptr->SDA_set(1);			/* SDA input */
	if (!need_to_stop)
		{
		for (i = 0; i < 8; i++)
			{
			rtn_value <<= 1;
				
			wait_timer(cptr->tAA);		/* data output delay time */

#ifdef _CHECK_SCL_HIGH
			i2c_raise_scl(cptr);			/* SCL high */
#else
			cptr->SCL_set(1);
#endif
	
			wait_timer(cptr->tR);		/* SCL rise time */

			if (cptr->SDA_sense())		/* SDA out */	
				rtn_value |= 1;
			else
				rtn_value |= 0;
			wait_timer(cptr->tHIGH);		/* SCL high time */
			cptr->SCL_set(0);			/* SCL low */
#ifdef REDUCE_TIME
			wait_timer((cptr->tLOW));	/* SCL fall time + data out hold time + clock low time */
#else
			wait_timer((cptr->tF)+(cptr->tDH)+(cptr->tLOW));	/* SCL fall time + data out hold time + clock low time */
#endif
			}
		}
	return(rtn_value);
}

/****************************************************
  Write n bytes to the i2c device specified by
  its device id.

  arguments:
	dev_id	device id
	n	no. of bytes to be written 
	s	array of n data to be written 

  return:
 	0	okay (got ack) 
       	-1	error (no ack)
*****************************************************/
short I2CWriteNByte(IIC_CONFIG dev, unsigned char n, unsigned char *s)
{
	unsigned char i, rt;
	IIC_CONFIG  *cptr;
	cptr = &dev;
	for (rt = 0; rt < TRY_COUNT; rt++)	/* try again if no ack */
		{
		timeout = 0;                    		/* reset timeout */
		wait_timer(cptr->tBUF);         	/* bus free time: both SCL and SDA are high */
		i2c_start(cptr);                    	/* start condition */
		i2c_writebyte(cptr, cptr->dev_addr);  /* control byte */
		for (i = 0; i < n; i++)
			{
			i2c_writebyte(cptr, s[i]);
			}
		i2c_stop(cptr);                     	/* stop condition */
		wait_timer(cptr->tWR);          	/* write cycle time */
		if (!timeout)
			return(0);
		}
	return(-1);
}

/****************************************************
  Read n bytes from the i2c device specified by
  its device id.  No subaddress is specified.

  arguments:
	dev_id	device id read
	n	no. of bytes to be written
	s	array of storing n data to be read 

  return:
	0 	okay (got ack)
	-1	error (no ack)
*****************************************************/
short I2CReadNByte(IIC_CONFIG dev, unsigned char n, unsigned char *s)
{
	unsigned char i, rt;
	unsigned char tmp = (n-1);
	IIC_CONFIG 	*cptr;
	cptr = &dev;
	for (rt = 0; rt < TRY_COUNT; rt++)	/* try again if no ack */
		{
		timeout = 0;                    		/* reset timeout */
		wait_timer(cptr->tBUF);         	/* bus free time: both SCL and SDA are high */
		i2c_start(cptr);                    	/* start condition */
		i2c_writebyte(cptr, (cptr->dev_addr) | 0x01);
		/* control byte: for read */
		for (i = 0; i < tmp; i++)
			{
			s[i] = i2c_readbyte(cptr);
			i2c_ack(cptr, 2);			/* give ack */
			}

		s[n-1] = i2c_readbyte(cptr);
		i2c_ack(cptr, 0); 				/* ack clock cycle: no ack is needed for last read */
		i2c_stop(cptr);                    	 /* stop condition */
		if (!timeout)                                    
			return(0);                  		/* data read */
		}
	return(-1);
}

/****************************************************
  Write a byte to the i2c device specified by
  its device id.
 
  arguments:
	dev_id 	device id 
	addr	address to be written ( ignore if < 0 )
	data_value	data to be written
 
  return:
	0	okay (got ack)
	-1	error (no ack)	
*****************************************************/
short I2C_bytewrite(IIC_CONFIG dev, unsigned int addr, unsigned char data_value)
{
	unsigned char i;
	IIC_CONFIG  *cptr;

	unsigned char w_addr = (unsigned char)(addr & 0xff);
#ifdef BIG_EEPROM
	unsigned char hw_addr = (unsigned char)(((addr & 0x700)>>7)&0x0e);
#endif

	cptr = &dev;
#if 0
	if(dev_id < 0)	/*此设备未认到,返回*/
		{
#ifdef _DEBUG_I2C_DRV
		printf("dev_id error\n");
#endif
		return (-1);
		}
#endif
	/* ptr to device parameters */
//	cptr = i2c_dev[dev_id];
	for (i = 0; i < TRY_COUNT; i++)	/* try again if no ack */
		{
		timeout = 0;                    /* reset timeout */
		wait_timer(cptr->tBUF);  /* bus free time:both SCL and SDA are high */
		i2c_start(cptr);               /* start condition */
#ifdef BIG_EEPROM
		{
		if(addr>255)
			{
			i2c_writebyte(cptr, (cptr->dev_addr)|hw_addr);
			}
		else
			i2c_writebyte(cptr, cptr->dev_addr);
		}
#else
		i2c_writebyte(cptr, cptr->dev_addr);  /* control byte */
#endif
//		if( addr >= 0 )
			{
			i2c_writebyte(cptr, w_addr);          /* word address */
			}
		i2c_writebyte(cptr, data_value);            /* data_value */
		i2c_stop(cptr);                     /* stop condition */
#ifndef CHEAT
		wait_timer(cptr->tWR);          /* write cycle time */
#endif
		if (!timeout)
			{
			return(0);
			}
		}    
	return(-1); /* timeout without acknowledge after several attempts */
}  

/****************************************************
  Read a byte from the i2c device specified by
  its device id.

  arguments:
	dev_id	device id
	addr	address to be read (ignore if < 0 )

  return:
	>= 0	data read 
	-1	error (no ack)
*****************************************************/
short I2C_byteread(IIC_CONFIG dev, unsigned int addr)
{
	unsigned char i;
	unsigned char b;
	IIC_CONFIG 	*cptr;

	unsigned char w_addr = (unsigned char)(addr & 0xff);
#ifdef BIG_EEPROM
	unsigned char hw_addr = (unsigned char)(((addr & 0x700)>>7)&0x0e);
#endif
	cptr = &dev;

#if 0
	if(dev_id < 0)	/*此设备未认到,返回*/
		{
#ifdef _DEBUG_I2C_DRV
		printf("dev_id error\n");
#endif
		return (-1);
		}

	/* ptr to device parameters */
	cptr = i2c_dev[dev_id];
#endif
	for (i = 0; i < TRY_COUNT; i++)	/* try again if no ack */
		{
		timeout = 0;                    	/* reset timeout */
		wait_timer(cptr->tBUF);     /* bus free time: both SCL and SDA are high */
//		if( addr >= 0 )
			{
			i2c_start(cptr);           /* start condition */
#ifdef BIG_EEPROM
			if(addr>255)
				{
				i2c_writebyte(cptr, (cptr->dev_addr)|hw_addr);
				}
			else
				i2c_writebyte(cptr, cptr->dev_addr);
#else
			i2c_writebyte(cptr, cptr->dev_addr);  /* control byte: for write */
#endif
			i2c_writebyte(cptr, w_addr);          /* word address */
			}
		i2c_start(cptr);                    /* start condition */
#ifdef BIG_EEPROM
		if(addr>255)
			{
			i2c_writebyte(cptr, ((cptr->dev_addr) | hw_addr) | 0x01 );
			}
		else
			i2c_writebyte(cptr, (cptr->dev_addr) | 0x01);
#else
		i2c_writebyte(cptr, (cptr->dev_addr) | 0x01);
#endif
		/* control byte: for read */
		b = i2c_readbyte(cptr);             /* read one byte of data */
		/* ack clock cycle: no ack is needed for read */
		i2c_ack(cptr, 0);
		i2c_stop(cptr);                     /* stop condition */
		if (!timeout)
			return(b);                  /* data read */
		}
	return(-1);                         /* timeout without acknowledge after several attempts */
}

#if 0
/****************************************************
  Write n bytes to the DSP i2c device specified by its device id.

  arguments:
	dev_id	device id
	n		no. of bytes to be written 
	s		array of n data to be written 

  return:
 	0		okay (got ack) 
       -1		error (no ack)
*****************************************************/
short DSPI2CWriteNByte(IIC_CONFIG dev, unsigned char n, unsigned char *s)
{
	unsigned char rt;
	unsigned char i;
	IIC_CONFIG  *cptr;
	cptr = &dev;
	
	for (rt = 0; rt < TRY_COUNT; rt++)	/*try again if no ack*/ 
		{
		timeout = 0;                    	  	/* reset timeout */
		wait_timer(cptr->tBUF);         	/* bus free time: both SCL and SDA are high */
		i2c_start(cptr);                    	/*start condition */
		i2c_writebyte(cptr, cptr->dev_addr);  /* control byte --设备地址*/

		for (i = 0; i < n; i++) 		                /*循环写数据*/
			{
				i2c_writebyte(cptr, s[i]);
				
				if(((i+1)%4==0)&&((n-(i+1))>0)) /*每当写完4字节,且还有数据要写就要判断DSP_BUSY引脚*/
				{					
					while(1)
					{
						if(SenseDspBusy())
							break;
#ifdef _DEBUG_I2C_DRV
						else
							debug_printf("DSP_BSY is low\n");
#endif						
					}
				}
			}		
		i2c_stop(cptr);                     /*stop condition */		
		wait_timer(cptr->tWR);       /* wait cycle time */
		
		if (!timeout)
			return(0);
		}
	return(-1);
}

/****************************************************************************
  Read n bytes from the DSP i2c device specified by its device id.  No subaddress is specified.

  arguments:
	dev_id	device id read
	n		no. of bytes to be written
	s		array of storing n data to be read 

  return:
	0 		okay (got ack)
	-1		error (no ack)
*****************************************************************************/
short DSPI2CReadNByte(IIC_CONFIG dev, unsigned char n, unsigned char *s)
{
	unsigned char i, rt;
	unsigned char tmp = (n-1);
	IIC_CONFIG 	*cptr;
	cptr = &dev;
	
	for (rt = 0; rt < TRY_COUNT; rt++)	/* try again if no ack */ 
		{
		timeout = 0;                    		/* reset timeout */ 	
		wait_timer(cptr->tBUF);         	/* bus free time: both SCL and SDA are high */				
		i2c_start(cptr);                    	/*start condition */ 	
		i2c_writebyte(cptr, (cptr->dev_addr) | 0x01); /* control byte --设备地址*/

		for (i = 0; i < tmp; i++)
			{	
			s[i] = i2c_readbyte(cptr);		
			i2c_ack(cptr,2);			/*give ack*/ 				
			}		
		 
		s[n-1] = i2c_readbyte(cptr);	
		i2c_ack(cptr, 0); 		/* ack clock cycle: no ack is needed for last read */
		i2c_stop(cptr);           /* stop condition */
		if (!timeout)                                    
			return(0);           /* data read */
		}
	return(-1);	
}
#endif

