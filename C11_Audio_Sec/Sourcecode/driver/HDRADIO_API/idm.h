/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                Idm.h											  					**
**																			  				**
** DESCRIPTION: 		IDM Interface declarations											**
**********************************************************************************************/

#ifdef IDM_I2C

#include "globals.h"

#define I2C_OK 1


/*!
 *	\file idm.h
 *	\brief IDM Interface
 */


//Prototypes

/*! \fn UINT8 IdmI2CWrite(UINT8 SlaveAddress,
                                                  UINT16 WriteLen,
                                                  UINT8 *WriteBuffer)
	*  \brief IdmI2CWrite
	*  \param UINT8 SlaveAddress
	*  \param UINT16 WriteLen
	*  \param UINT8 *WriteBuffer
	*  \retval  UINT8
*/
UINT8 IdmI2CWrite(UINT8 SlaveAddress,UINT16 WriteLen,UINT8 *WriteBuffer);

/*! \fn UINT8 IdmI2CRead(UINT8 SlaveAddress, UINT8 *Buffer)
	*  \brief IdmI2CRead
	*  \param UINT8 SlaveAddress
	*  \param UINT8 *Buffer
	*  \retval  UINT8
*/
UINT8 IdmI2CRead(UINT8 SlaveAddress,UINT8 *Buffer);


// Real Time Clock

/*! \fn INT8 I2C_RTCstatusRead(UINT8 status, UINT8 * pval)
	*  \brief I2C_RTCstatusRead
	*  \param UINT8 status
	*  \param UINT8 * pval
	*  \retval  INT8
*/
INT8 I2C_RTCstatusRead(UINT8 status, UINT8 * pval);

/*! \fn INT8 I2C_RTCstatusWrite(UINT8 status, UINT8 val)
	*  \brief I2C_RTCstatusWrite
	*  \param UINT8 status
	*  \param UINT8 val
	*  \retval  INT8
*/
INT8 I2C_RTCstatusWrite(UINT8 status, UINT8 val);

/*! \fn INT8 I2C_RTCtimeRead(UINT8 * pdt)
	*  \brief I2C_RTCtimeRead
	*  \param UINT8 * pdt
	*  \retval  INT8
*/
INT8 I2C_RTCtimeRead(UINT8 * pdt);

/*! \fn INT8 I2C_RTCtimeWrite(UINT8 * pdt)
	*  \brief I2C_RTCtimeWrite
	*  \param UINT8 * pdt
	*  \retval  INT8
*/
INT8 I2C_RTCtimeWrite(UINT8 * pdt);

#endif
