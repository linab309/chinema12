/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                idmstat.h										  					**
**																			  				**
** DESCRIPTION: 		IDM Status functions												**
**********************************************************************************************/

#if     !defined(idmstat_H)
#define idmstat_H

#include "globals.h"


/*!
 *      \file idmstat.h
 *      \brief IDM Status function handler.
 */

/*! \fn VOID AcqStatusClear (VOID)
        *  \brief Clear Acquisition data
        *  \param None
        *  \retval  None
*/
VOID AcqStatusClear (VOID);

/*! \fn VOID AcqStatusQuickSave (UINT8 * pAcqData)
        *  \brief Store Basic Acquisition data retrieved from IDM acq status operation
        *  \param UINT8 *
        *  \retval  None
*/
VOID AcqStatusQuickSave (UINT8 * pAcqData);

/*! \fn VOID AcqStatusSave (UINT8 * pAcqData)
        *  \brief Store Acquisition data retrieved from IDM acq status operation
        *  \param UINT8 *
        *  \retval  None
*/
VOID AcqStatusSave (UINT8 * pAcqData);

/*! \fn VOID SetStereoStatus (UINT8 mono_stereo)
        *  \brief Store mono/stereo status
        *  \param UINT8
        *  \retval  None
*/
VOID SetStereoStatus (UINT8 mono_stereo);

/*! \fn UINT8 GetStereoStatus (VOID)
        *  \brief Gete mono/stereo status
        *  \param None
        *  \retval  UINT8
*/
UINT8 GetStereoStatus (VOID);

/*! \fn UINT8 DAAI (VOID)
        *  \brief Return signal quality based on DAAI
        *  \param None
        *  \retval  Signal Quality
*/
UINT8 DAAI (VOID);

/*! \fn UINT8 SignalStrength(VOID)
        *  \brief Return signal quality 0 - 255
        *  \param None
        *  \retval  Signal Quality
*/
UINT8 SignalStrength(VOID);

/*! \fn VOID ClearMulticastIndex (VOID)
        *  \brief Clear the multicast index
        *  \param None
        *  \retval None
*/
VOID ClearMulticastIndex (VOID);

/*! \fn UINT8 GetMulticastIndex (VOID)
        *  \brief Get the multicast index
        *  \param None
        *  \retval  UINT8
*/
UINT8 GetMulticastIndex (VOID);

/*! \fn BOOL CheckMulticastIndex (UINT8 index)
        *  \brief Check the availabilty of given multicast index.
        *  \param UINT8
        *  \retval BOOL
*/
BOOL CheckMulticastIndex (UINT8 index);

/*! \fn VOID SetMulticastIndex (UINT8 index)
        *  \brief Set the multicast index
        *  \param UINT8 index
        *  \retval  None
*/
VOID SetMulticastIndex (UINT8 index);

/*! \fn UINT8 GetMulticastIndexNext (VOID)
        *  \brief Get the next multicast index
        *  \param None
        *  \retval  UINT8
*/
UINT8 GetMulticastIndexNext (VOID);
/*! \fn VOID SetMulticastIndexNext (VOID)
        *  \brief Set the next multicast index
        *  \param None
        *  \retval  None
*/
VOID SetMulticastIndexNext (VOID);
/*! \fn UINT8 GetMulticastIndexPrev (VOID)
        *  \brief Get the previous multicast index
        *  \param None
        *  \retval  UINT8
*/
UINT8 GetMulticastIndexPrev (VOID);

/*! \fn VOID SetMulticastIndexPrev (VOID)
        *  \brief Set the previous multicast index
        *  \param None
        *  \retval  None
*/
VOID SetMulticastIndexPrev (VOID);

/*! \fn UINT8 ProgramType (VOID)
        *  \brief Get the program type (genre)
        *  \param None
        *  \retval  UINT8
*/
UINT8 ProgramType (VOID);

/*! \fn BOOL NextProgram (VOID)
        *  \brief Get the status of next program
        *  \param None
        *  \retval  BOOL
*/
BOOL NextProgram (UINT8);

/*! \fn BOOL CurrentProgram (VOID)
        *  \brief Get the actively playing program
        *  \param None
        *  \retval  UINT8
*/
UINT8 CurrentProgram (VOID);

/*! \fn BOOL PreviousProgram (VOID)
        *  \brief Get the status of previous program
        *  \param None
        *  \retval  BOOL
*/
BOOL PreviousProgram (VOID);

/*! \fn VOID HighestProgram (VOID)
        *  \brief Store the highest program
        *  \param None
        *  \retval  None
*/
VOID HighestProgram (VOID);

/*! \fn UINT8 NumberOfPrograms (VOID)
        *  \brief Number of programs
        *  \param None
        *  \retval UINT8
*/
UINT8 NumberOfPrograms (VOID);

/*! \fn UINT8 DigitalAudioPrograms (VOID)
        *  \brief Digital audio programs
        *  \param None
        *  \retval UINT8
*/
UINT8 DigitalAudioPrograms (VOID);

/*! \fn BOOL CAProgram (VOID)
        *  \brief Get the conditional access status of current program
        *  \param None
        *  \retval  BOOL
*/
BOOL CAProgram (VOID);

/*! \fn BOOL IsCAProgram (VOID)
        *  \brief Check if the current program is a CA program
        *  \param None
        *  \retval  BOOL
*/
BOOL IsCAProgram (VOID);

/*! \fn UINT8 CAProgramsAvailable(VOID)
        *  \brief Get the conditional access status of all programs
        *  \param None
        *  \retval  UINT8
*/
UINT8 CAProgramsAvailable (VOID);

/*! \fn UINT8 DigitalAudioMode (VOID)
        *  \brief Digital audio mode
        *  \param None
        *  \retval UINT8
*/
UINT8 DigitalAudioMode (VOID);

/*! \fn BOOL isDigitalAudioAcquired (VOID)
        *  \brief Checks whether digital audio acquired or not
        *  \param None
        *  \retval BOOL
*/
BOOL isDigitalAudioAcquired (VOID);

/*! \fn BOOL isDigitalSignalAcquired (VOID)
        *  \brief Check whether digital signal acquired or not
        *  \param None
        *  \retval  BOOL
*/
BOOL isDigitalSignalAcquired (VOID);

/*! \fn BOOL isSISCRCvalid (VOID)
        *  \brief Get SIS CRC Status
        *  \param None
        *  \retval BOOL
*/
BOOL isSISCRCvalid (VOID);

/*! \fn UINT32 SNR (VOID)
        *  \brief Return Signal-To-Noise Ratio
        *  \param None
        *  \retval UINT32
*/
UINT32 SNR (VOID);

/*! \fn UINT8 * pSNR (VOID)
        *  \brief Return the base address of array which stores Signal-To-Noise Ratio
        *  \param None
        *  \retval UINT8 *
*/
UINT8 * pSNR (VOID);

/*! \fn VOID SNRSave (UINT8 * pData)
        *  \brief Saves SNR which is returned from the IDM command
        *  \param UINT8* pointer to SNR array
        *  \retval None
*/
VOID SNRSave (UINT8 * pData);

/*! \fn VOID BERSave (UINT8 * pData)
        *  \brief Saves BER which is returned from the IDM command
        *  \param UINT8* pointer to BER array
        *  \retval None
*/
VOID BERSave(UINT8 * pData);

/*! \fn UINT32 * GetBER (VOID)
        *  \brief Returns BER
        *  \param None
        *  \retval UNT32 *
*/
UINT32 * GetBER (VOID);

#endif /* End of idmstat_H */
