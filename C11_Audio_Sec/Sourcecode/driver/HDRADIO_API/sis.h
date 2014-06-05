/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                sis.h											  					**
**																			  				**
** DESCRIPTION: 		Station Information Service data  functions							**
**********************************************************************************************/

#if     !defined(sis_H)
#define sis_H

#include "globals.h"

/*!
 *      \file sis.h
 *      \brief Station Information Service data function handler
 */

#define STATION_MESSAGE_LENGTH 190

/*! \fn VOID SISbasicSave (UINT8 * pSISdata)
        *  \brief Store SIS data retrieved from extended SIS operation.
        *  \param UINT8 *
        *  \retval None
*/
VOID SISbasicSave (UINT8 * pSISdata);

/*! \fn VOID SISuniversalShortNameSave (UINT8 * pSISdata)
        *  \brief Store SIS universal short station name retrieved from extended SIS operation.
        *  \param UINT8 *
        *  \retval None
*/
VOID SISuniversalShortNameSave (UINT8 * pSISdata);

/*! \fn VOID SISuniversalLongNameSave (UINT8 * pSISdata)
        *  \brief Store SIS universal long station name retrieved from extended SIS operation.
        *  \param UINT8 *
        *  \retval None
*/
VOID SISuniversalLongNameSave (UINT8 * pSISdata);

/*! \fn VOID SIStimeZoneSave (UINT8 * pSISdata)
        *  \brief Store SIS time zone informatoin retrieved from extended SIS operation
        *  \param UINT8 *
        *  \retval None
*/
VOID SIStimeZoneSave (UINT8 * pSISdata);

/*! \fn VOID SISLeapSecondsSave(UINT8 * pSISdata)
        *  \brief Store SIS Leap Seconds informatoin retrieved from extended SIS operation
        *  \param UINT8 *
        *  \retval None
*/
VOID SISLeapSecondsSave(UINT8 * pSISdata);

/*! \fn BOOL isSISdataUpdated (VOID)
        *  \brief Determine if SIS data is available and updated since last read by host.
        *         This is accomplished by checking the data available flag
        *  \param None
        *  \retval BOOL - TRUE if data available flag is set to 2, FALSE otherwise
*/
BOOL isSISdataUpdated (VOID);

/*! \fn BOOL isSISdataAvailable (VOID)
        *  \brief Determine if SIS data is available.
        *  \param None
        *  \retval BOOL
*/
BOOL isSISdataAvailable (VOID);

/*! \fn VOID setSISinvalid (VOID)
        *  \brief Set all SIS information to invalid and uninitialized state.
        *  \param None
        *  \retval None
*/
VOID setSISinvalid (VOID);

/*! \fn UINT8 * SIS_ShortStationName (VOID)
        *  \brief Return short station name to application.
        *  \param None
        *  \retval UINT8 *
*/
UINT8 * SIS_ShortStationName (VOID);

/*! \fn UINT8 * SIS_LongStationName (VOID)
        *  \brief Return long station name.
        *  \param None
        *  \retval UINT8 *
*/
UINT8 * SIS_LongStationName (VOID);

/*! \fn UINT8 * SIS_UniversalShortStationName (VOID)
        *  \brief Return universal short station name to application.
        *  \param None
        *  \retval UINT8 *
*/
UINT8 * SIS_UniversalShortStationName (VOID);

/*! \fn BOOL isUniversalStationShortNameAvailable (VOID)
        *  \brief Determine if universal station short name is available.
        *  \param None
        *  \retval BOOL
*/
BOOL isUniversalStationShortNameAvailable (VOID);

/*! \fn UINT8 * SIS_UniversalLongStationName (VOID)
        *  \brief Return universal long station name to application.
        *  \param None
        *  \retval UINT8 *
*/
UINT8 * SIS_UniversalLongStationName(VOID);

/*! \fn BOOL SIS_IsValidStationMessage(VOID)
        *  \brief return status of station message.
        *  \param None
        *  \retval BOOL
*/
BOOL SIS_IsValidStationMessage(VOID);

/*! \fn UINT8 SIS_GetStationMessagePriority(VOID)
        *  \brief return prority of station message.
        *  \param None
        *  \retval UINT8
*/
UINT8 SIS_GetStationMessagePriority(VOID);

/*! \fn UINT8 SIS_GetStationMessageEncoding(VOID)
        *  \brief return encoding method of station message.
        *  \param None
        *  \retval UINT8
*/
UINT8 SIS_GetStationMessageEncoding(VOID);

/*! \fn UINT8 *SIS_GetStationMessage(VOID)
        *  \brief return station message.
        *  \param None
        *  \retval UINT8 *
*/
UINT8 *SIS_GetStationMessage(VOID);

/*! \fn VOID SIS_StationMessageSave(UINT8 *pSISdata)
        *  \brief Store Station message retrieved from extended SIS operation.
        *  \param UINT8 *
        *  \retval None
*/
VOID SIS_StationMessageSave(UINT8 *pSISdata, UINT8 msg_length);

/*! \fn UINT8 ALFNstatus (VOID)
        *  \brief Return ALFN status to application
        *  \param None
        *  \retval UINT8
*/
UINT8 ALFNstatus (VOID);

/*! \fn BOOL isALFNvalid (VOID)
        *  \brief Return ALFN validity to application
        *  \param None
        *  \retval BOOL - TRUE if ALFN valid, FALSE otherwise
*/
BOOL isALFNvalid (VOID);

/*! \fn BOOL isALFNtimeLocked (VOID)
        *  \brief Return ALFN time lock status to application
        *  \param None
        *  \retval BOOL
*/
BOOL isALFNtimeLocked (VOID);

/*! \fn UINT32 ALFN (VOID)
        *  \brief Convert 4-byte ALFN to 32-bits and return 32-bit number.
        *  \param None
        *  \retval UINT32
*/
UINT32 ALFN (VOID);

/*! \fn VOID ALFNtime (UINT16 * p)
        *  \brief Calculate and save time
        *  \param UINT16 *
        *  \retval None
*/
VOID ALFNtime (UINT16 * p);

 /*! \fn BOOL SIS_IsALFNAvail
         *  \brief Returns TRUE if ALFN is available.
         *  \param None
         *  \retval BOOL
*/
BOOL SIS_IsALFNAvail(VOID);

/*! \fn BOOL SIS_IsStationIDAvail
        *  \brief Returns TRUE if Station ID is available.
        *  \param None
        *  \retval BOOL
*/
BOOL SIS_IsStationIDAvail(VOID);

/*! \fn UINT8 *SIS_GetStationID(VOID)
        *  \brief Returns the Station ID.
        *  \param None
        *  \retval UINT8*
*/
UINT8 *SIS_GetStationID(VOID);

/*! \fn UINT16 SIS_CountryCode(VOID)
        *  \brief Returns the Country Code.
        *  \param None
        *  \retval UINT16
*/
UINT16 SIS_CountryCode(VOID);

/*! \fn UINT32 SIS_FacilityCode(VOID)
        *  \brief Returns the Facility Code.
        *  \param None
        *  \retval UINT32
*/
UINT32 SIS_FacilityCode(VOID);

/*! \fn BOOL SIS_IsStationLocationAvail
        *  \brief Returns TRUE if Station location is available.
        *  \param None
        *  \retval BOOL
*/
BOOL SIS_IsStationLocationAvail(VOID);

/*! \fn UINT8 *SIS_GetStationLocation(VOID)
        *  \brief Returns the Staion Location.
        *  \param None
        *  \retval UINT8*
*/
UINT8 *SIS_GetStationLocation(VOID);

/*! \fn UINT8 *SIS_GetTimeZone(VOID)
        *  \brief Returns the time zone.
        *  \param None
        *  \retval UINT8*
*/
UINT8 *SIS_GetTimeZone(VOID);

/*! \fn UINT8 *SIS_GetLeapSeconds(VOID)
        *  \brief Returns the Leap Seconds.
        *  \param None
        *  \retval UINT8*
*/
UINT8 *SIS_GetLeapSeconds(VOID);


#endif /* End of sis_H */
