/*************************************************************************************************
** Copyright © 2005 iBiquity Digital Inc.
** All rights reserved.
**
** PROJECT:             Atmel HD Radio
**
** FILE:                rbds.h
**
** Description:        	RDBS(Radio Broadcast Data System) Decoder
**
*************************************************************************************************/


/*!
 *      \file rbds.h
 *      \brief radio broadcasting data system and it's related functions.
 */

#ifndef _RDBS_H
#define _RDBS_H

//#include "processor.h"
//#include "globals.h"
#include "..\main\Macro.h"

/*! \fn void RBDS_Init(void)
        *  \brief Initialize the RBDS
        *  \param None
        *  \retval None
*/
void RBDS_Init(void);

/*! \fn void RBDS_Decode(void)
        *  \brief RBDS Main routine
        *  \param None
        *  \retval None
*/
void RBDS_Decode(void);

/*! \fn BOOL RBDS_IsValid(void)
        *  \brief Check if RBDS data available
        *  \param None
        *  \retval BOOL
*/
BOOL RBDS_IsValid(void);

/*! \fn void RBDS_SetInvalid(void)
        *  \brief Invlaidate RBDS data availablity
        *  \param None
        *  \retval None
*/
void RBDS_SetInvalid(void);

/*! \fn uint8 *RBDS_GetCallSignStr(void)
        *  \brief To get te Call Letter for current frequency
        *  \param None
        *  \retval uint8 *
*/
uint8 *RBDS_GetCallSignStr(void);

/*! \fn uint8 RBDS_GetPTYCodeStr(void)
        *  \brief To get the PTY code
        *  \param None
        *  \retval uint8 *
*/
uint8 RBDS_GetPTYCode(void);

/*! \fn uint8 *RBDS_GetPSDescriptionStr(void);
        *  \brief Get the Program Service Description.
        *  \param None
        *  \retval uint8 *
*/
uint8 *RBDS_GetPSDescriptionStr(void);

/*! \fn uint8 *RBDS_GetRadioTextStr(void);
        *  \brief Get the radio text
        *  \param None
        *  \retval uint8 *
*/
uint8 *RBDS_GetRadioTextStr(void);

void RBDS_SetEnable(BOOL bEnabled);

BOOL RBDS_IsEnabled(void);

#endif //_RDBS_
