#ifndef _I2CDRV_H
#define _I2CDRV_H

/* device specification */
typedef struct {
	unsigned char dev_addr;	/* device I2C address*/
	void (*SCL_set)(int);		/* SCL_set fn ptr */
	void (*SDA_set)(int);		/* SDA_set fn ptr */
	int (*SCL_sense)();		/* SCL_sense fn ptr */
	int (*SDA_sense)();		/* SDA_sense fn ptr */

	/* timing info */
	int tAA;			/* SCL Low to SDA Data Out Valid. */
	int tBUF;			/* Time the Bus Must be Free before a New Transmission Can Start. */
	int tHD_STA;		/* Start Condition Hold Time. */
	int tSU_STA;		/* Start Condition Setup Time (for a Repeated Start Condition). */
	int tLOW;		/* Clock Low Time. */
	int tHIGH;		/* Clock High Time. */
	int tHD_DAT;		/* Data In Hold Time. */
	int tSU_DAT;		/* Data In Setup Time. */
	int tR;			/* SDA and SCL Rise Time. */
	int tF;			/* SDA and SCL Fall Time. */
	int tSU_STO;		/* Stop Condition Setup Time. */
	int tDH;			/* Data Out Hold Time. */
	int tWR;			/* Write Cycle Time. */

	/* timeout periods */
	int ack_wtime;	/* ack timeout */
} IIC_CONFIG;

extern short I2CWriteNByte(IIC_CONFIG dev, unsigned char n, unsigned char *s);
extern short I2CReadNByte(IIC_CONFIG dev, unsigned char n, unsigned char *s);
extern short I2C_byteread(IIC_CONFIG dev, unsigned int addr);
extern short I2C_bytewrite(IIC_CONFIG dev, unsigned int addr, unsigned char data_value);
#endif

