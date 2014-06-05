/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                serialtask.h 									  					**
**																			  				**
** DESCRIPTION: 		Serial functionalities.												**
**********************************************************************************************/

/*!
 *	\file serialtask.h
 *	\brief Serial task
 */

#if     !defined(serialtask_H)
#define serialtask_H

//#ifdef IDM_SERIAL_COMMUNICATION
VOID SerialTask(VOID);
BOOL IdmSerialWrite(UINT8 WriteLen, UINT8 *WriteBuffer);
BOOL IdmSerialRead(UINT8 opcode, UINT16 buffsize, UINT8 * buffer);
//#endif //IDM_SERIAL_COMMUNICATION

#endif