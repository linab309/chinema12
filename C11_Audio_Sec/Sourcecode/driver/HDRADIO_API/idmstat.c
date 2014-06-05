/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                idmstat.c										  					**
**																			  				**
** DESCRIPTION: 		IDM Status functions												**
**********************************************************************************************/

#include "processor.h"
#include "globals.h"
#include "idmstat.h"

typedef struct
{
    UINT8 AM_FM;            // offset = 0  - AM or FM Mode
    UINT8 Mode;             // offset = 1  - Digital Mode: MP1,MA1, etc
    UINT8 Acq_Stat;         // offset = 3  - Digital signal acquired
    UINT8 SIS_CRC_status;   // offset = 5  - SIS CRC Status
    UINT8 Dig_Audio_Acq;    // offset = 6  - Digital audio acquired
    UINT8 Audio_Programs;   // offset = 12 - Audio Programs available
    UINT8 Program_Type[8];  // offset = 13 - Genre for each audio program
	UINT8 current_prog;     // offset = 21 - Current Audio Program playing
	UINT8 DAAI;             // offset = 22 - Digital Audio Availability Indicator
    UINT8 num_programs;
    UINT8 mono_stereo_status; // 0 = Mono, 1 = Partial Stereo (Stereo in AM), 2 = Full Stereo (FM only)
	UINT8 CA_Audio_Programs;
} ACQ_STAT_TYPE;

static ACQ_STAT_TYPE AcqData;
static UINT8 multicast_index;
static UINT8 snr[4];
static UINT32 BER[10];

/******************************************************************
 *
 *      AcqStatusClear
 *
 *      Description: Clear Acquisition data
 *
 *      Input: 	None
 *      Output: None
 *
 ******************************************************************/
#ifdef DRI352
VOID AcqStatusClear (VOID)
{
//	AcqData.AM_FM         = Tune_GetBand();
	AcqData.Mode          = 0;
	AcqData.Acq_Stat      = 0;
	AcqData.SIS_CRC_status= 0;
	AcqData.Dig_Audio_Acq = 0;
	AcqData.Audio_Programs= 0;
	AcqData.current_prog  = 0;
	AcqData.DAAI          = 0;
	AcqData.num_programs  = 0;
	AcqData.CA_Audio_Programs	= 0;
}
#endif


/******************************************************************
 *
 *      AcqStatusQuickSave
 *
 *      Description: Store basic Acquisition data retrieved from IDM acq status operation
 *
 *      Input: 	Pointer to acquisition data
 *      Output: None
 *
 ******************************************************************/
VOID AcqStatusQuickSave (UINT8 * pAcqData)
{
	UINT8 * p;

	p = pAcqData;

	AcqData.AM_FM         = *p;
	AcqData.Mode          = *(p + 1);
	AcqData.Acq_Stat      = *(p + 3);
	AcqData.SIS_CRC_status= *(p + 5);
	AcqData.Dig_Audio_Acq = *(p + 6);
	AcqData.Audio_Programs= *(p + 12);
	AcqData.current_prog  = *(p + 21);
	AcqData.DAAI          = *(p + 22);
	AcqData.CA_Audio_Programs	= *(p + 23);
}

/******************************************************************
 *
 *      AcqStatusSave
 *
 *      Description: Store Acquisition data retrieved from IDM acq status operation
 *
 *      Input: 	Pointer to acquisition data
 *      Output: None
 *
 ******************************************************************/
#ifdef DRI352
VOID AcqStatusSave (UINT8 * pAcqData)
{
    UINT8 num_programs;
    UINT8 aud_programs;
    UINT8 i;

    UINT8 * p;

    p = pAcqData;

    AcqData.AM_FM         = *p;
	AcqData.Mode          = *(p + 1);
	AcqData.Acq_Stat      = *(p + 3);
    AcqData.SIS_CRC_status= *(p + 5);
    AcqData.Dig_Audio_Acq = *(p + 6);
    AcqData.Audio_Programs= *(p + 12);
    AcqData.current_prog  = *(p + 21);
    AcqData.DAAI          = *(p + 22);
	AcqData.CA_Audio_Programs	= *(p + 23);
    /*
    ** Store Program types (genre)
    */
    p += 0x0D;
    for (i = 0; i < 8; i++)
    {
        AcqData.Program_Type[i] = *p++;
    }


	//If the band is AM then we can always set the subchannel as 1
	if(AcqData.AM_FM == BAND_AM)
	{
		AcqData.Audio_Programs = 1;
		AcqData.num_programs = 1;
	}
	else
	{
//		if (isDigitalAudioAcquired()) //test for HD audio
		{
			// Determine number of available multicast programs.

			num_programs = 0;
			aud_programs = AcqData.Audio_Programs;

			for (i = 0; i < 8; i++)
			{
				if ((aud_programs >> i) & 0x01)
				{
					num_programs++;
				}
			}

			AcqData.num_programs = num_programs;
		}
	}

	if(AcqData.Acq_Stat == 0) {
		AcqData.Dig_Audio_Acq = 0;
		AcqData.Audio_Programs= 0;
		AcqData.DAAI          = 0;
	}

}
#endif

/******************************************************************
 *
 *      SetStereoStatus
 *
 *      Description: Store mono/stereo status.
 *
 *      Input: 	Mono/Stereo status acquired from IDM.
 *      Output: None
 *
 ******************************************************************/
#ifdef DRI352
VOID SetStereoStatus (UINT8 mono_stereo)
{
    AcqData.mono_stereo_status = mono_stereo;
}
#endif

/******************************************************************
 *
 *      DAAI
 *
 *      Description: Digital Audio Availability Indicator.
 *
 *      Input: 	None
 *      Output: Integer value between from 0 to 6 indicating signal quality.
 *              0 is lowest.
 *
 ******************************************************************/
UINT8 DAAI (VOID)
{
    UINT8 sig = AcqData.DAAI & 0x3F;
    UINT8 qual;

    if (sig >= 0x38 && sig <= 0x3F)
    {
        qual = 6;
    }
    else if (sig >= 0x32 && sig <= 0x37)
    {
        qual = 5;
    }
    else if (sig >= 0x2A && sig <= 0x31)
    {
        qual = 4;
    }
    else if (sig >= 0x24 && sig <= 0x29)
    {
        qual = 3;
    }
    else if (sig >= 0x1C && sig <= 0x23)
    {
        qual = 2;
    }
    else if (sig >= 0x14 && sig <= 0x1B)
    {
        qual = 1;
    }
    else
    {
        qual = 0;
    }

    return (AcqData.Dig_Audio_Acq == 1) ? qual : 0;
}

/******************************************************************
 *
 *      SignalStrength
 *
 *      Description: Digital Audio Availability Indicator.
 *
 *      Input: 	None
 *      Output: Integer value between from 0 to 255
 *
 ******************************************************************/
UINT8 SignalStrength(VOID)
{
	return AcqData.DAAI;
}

/******************************************************************
 *
 *      ClearMulticastIndex
 *
 *      Description: Clear the multicast index.
 *
 *      Input: 	None
 *      Output: None
 *
 ******************************************************************/
VOID ClearMulticastIndex (VOID)
{
    AcqData.num_programs = 0;
    AcqData.Audio_Programs = 0;
    multicast_index = 0;
}

/******************************************************************
 *
 *      GetMulticastIndex
 *
 *      Description: Get multicast index.
 *
 *      Input: 	None
 *      Output: UINT8
 *
 ******************************************************************/
UINT8 GetMulticastIndex (VOID)
{
    return multicast_index;
}

/******************************************************************
 *
 *      CheckMulticastIndex
 *
 *      Description: Check the availabilty of given multicast index.
 *
 *      Input: 	UINT8
 *      Output: None
 *
 ******************************************************************/
BOOL CheckMulticastIndex (UINT8 index)
{
	if((1<<index) & AcqData.Audio_Programs)
		return TRUE;
	else
		return FALSE;
}

/******************************************************************
 *
 *      SetMulticastIndex
 *
 *      Description: Set multicast index.
 *
 *      Input: 	UINT8 index
 *      Output: None
 *
 ******************************************************************/
VOID SetMulticastIndex (UINT8 index)
{
    multicast_index = index;
}

/******************************************************************
 *
 *      SetMulticastIndexNext
 *
 *      Description: Set the next multicast index
 *
 *      Input: 	None
 *      Output: None
 *
 ******************************************************************/
VOID SetMulticastIndexNext (VOID)
{
    INT8 prog_number = multicast_index + 1;
    INT8 i;

    for (i = prog_number; i < 8; i++)
    {
        if ((1 << i) & AcqData.Audio_Programs)
        {
            break;
        }
    }

    multicast_index = (i == 8) ? 0 : i;
}

/******************************************************************
 *
 *      GetMulticastIndexNext
 *
 *      Description: Get the next multicast index
 *
 *      Input:  None
 *      Output: Next multicast index
 *
 ******************************************************************/
UINT8 GetMulticastIndexNext (VOID)
{
    INT8 prog_number = multicast_index + 1;
    INT8 i;

    for (i = prog_number; i < 8; i++)
    {
        if ((1 << i) & AcqData.Audio_Programs)
        {
            break;
        }
    }

    return ((i == 8) ? 0 : i);
}

/******************************************************************
 *
 *      SetMulticastIndexPrev
 *
 *      Description: Set the prev multicast index
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
VOID SetMulticastIndexPrev (VOID)
{
    INT8 prog_number = (multicast_index == 0) ? 7 : multicast_index - 1;
    INT8 i;

    for (i = prog_number; i >= 0; i--)
    {
        if ((1 << i) & AcqData.Audio_Programs)
        {
            break;
        }
    }

    multicast_index = i;
}

/******************************************************************
 *
 *      GetMulticastIndexPrev
 *
 *      Description: Get the previous multicast index
 *
 *      Input:  None
 *      Output: Previous multicast index
 *
 ******************************************************************/
UINT8 GetMulticastIndexPrev (VOID)
{
    INT8 prog_number = (multicast_index == 0) ? 7 : multicast_index - 1;
    INT8 i;

    for (i = prog_number; i >= 0; i--)
    {
        if ((1 << i) & AcqData.Audio_Programs)
        {
            break;
        }
    }

    return i;
}

/******************************************************************
 *
 *      NextProgram
 *
 *      Description: Get the status of next program
 *
 *      Input: 	None
 *      Output: BOOL
 *
 ******************************************************************/
BOOL NextProgram (UINT8 nDAPrograms)
{
    INT8 prog_number = multicast_index + 1;
    INT8 i;
    BOOL next = FALSE;


    for (i = prog_number; i < 8; i++)
    {
        if ((1 << i) & nDAPrograms)
        {
            next = TRUE;
            break;
        }
    }

    if (next)
    {
        multicast_index = i;
    }

    return next;
}

/******************************************************************
 *
 *      CurrentProgram
 *
 *      Description: Get the active program
 *
 *      Input: 	None
 *      Output: UINT8
 *
 ******************************************************************/
UINT8 CurrentProgram (VOID)
{
	UINT8 i, c;

	i = AcqData.current_prog;
	c = 0;
	while (i)
	{
		c++;
		i>>=1;
	}

	return c - 1;
}

/******************************************************************
 *
 *      PreviousProgram
 *
 *      Description: Get the status of previous program
 *
 *      Input: 	None
 *      Output: BOOL
 *
 ******************************************************************/
BOOL PreviousProgram (VOID)
{
    INT8 prog_number = multicast_index - 1;
    INT8 i;
    BOOL prev = FALSE;

    for (i = prog_number; i >= 0; i--)
    {
        if ((1 << i) & AcqData.Audio_Programs)
        {
            prev = TRUE;
            break;
        }
    }

    if (prev)
    {
        multicast_index = i;
    }

    return prev;
}

/******************************************************************
 *
 *      HighestProgram
 *
 *      Description: Get the highest program
 *
 *      Input: 	None
 *      Output: None
 *
 ******************************************************************/
VOID HighestProgram (VOID)
{
    INT8 i;

    for (i = 7; i >= 0; i--)
    {
        if ((1 << i) & AcqData.Audio_Programs)
        {
            break;
        }
    }

    multicast_index = i;
}

/******************************************************************
 *
 *      ProgramType
 *
 *      Description: Get the program type (genre)
 *
 *      Input:  None
 *      Output: Integer number representing the genre
 *
 ******************************************************************/
UINT8 ProgramType (VOID)
{
    UINT8 i;
    UINT8 playing;

    playing = AcqData.current_prog;

    for (i = 0; i < 8; i++)
    {
        if (playing & (1 << i))
        {
            break;
        }
    }

    return AcqData.Program_Type[i];
}

/******************************************************************
 *
 *      NumberOfPrograms
 *
 *      Description: Return number of available multicast programs.
 *
 *      Input: 	None
 *      Output: UINT8
 *
 ******************************************************************/
UINT8 NumberOfPrograms (VOID)
{
    return AcqData.num_programs;
}

/******************************************************************
 *
 *      CAProgram
 *
 *      Description: Return the Conditional access status of current
 *					 program
 *
 *      Input: 	None
 *      Output: BOOL TRUE if current program is conditionally accessed
 *
 ******************************************************************/
BOOL CAProgram (VOID)
{
	return (AcqData.CA_Audio_Programs & AcqData.current_prog);
}

/******************************************************************
 *
 *      CAProgramsAvailable
 *
 *      Description: Return the Conditional access status of all
 *					 programs
 *
 *      Input: 	None
 *      Output: UINT8
 *
 ******************************************************************/
UINT8 CAProgramsAvailable (VOID)
{
	return AcqData.CA_Audio_Programs;
}

/******************************************************************
 *
 *      DigitalAudioMode
 *
 *      Description: Return the digital audio mode (MP1, MA1, etc)
 *					 along with the band
 *
 *      Input: 	None
 *      Output: UINT8
 *
 ******************************************************************/
UINT8 DigitalAudioMode (VOID)
{
	return AcqData.Mode | (AcqData.AM_FM<<4);
}

/******************************************************************
 *
 *      isDigitalAudioAcquired
 *
 *      Description: Checks whether digital audio acquired or not
 *
 *      Input: 	None
 *      Output: BOOL
 *
 ******************************************************************/

BOOL isDigitalAudioAcquired (VOID)
{
    return (AcqData.Dig_Audio_Acq == 1) ? TRUE : FALSE;
}


/******************************************************************
 *
 *      isDigitalSignalAcquired
 *
 *      Description: Checks whether digital signal (IBOC) acquired or not
 *
 *      Input: 	None
 *      Output: BOOL
 *
 ******************************************************************/
BOOL isDigitalSignalAcquired (VOID)
{
    return (AcqData.Acq_Stat == 1) ? TRUE : FALSE;
}

/******************************************************************
 *
 *      isSISCRCvalid
 *
 *      Description: SIS CRC Status
 *
 *      Input: 	None
 *      Output: BOOL
 *
 ******************************************************************/
#ifdef DRI352
BOOL isSISCRCvalid (VOID)
{
    return AcqData.SIS_CRC_status;
}
#endif

/******************************************************************
 *
 *      SNR
 *
 *      Description: Return Signal-To-Noise Ratio
 *
 *      Input: 	None
 *      Output: UINT32
 *
 ******************************************************************/
UINT32 SNR (VOID)
{
    UINT32 ival = 0;

    ival += (UINT32)(snr[0]);
    ival += (UINT32)(snr[1]) << 8;
    ival += (UINT32)(snr[2]) << 16;
    ival += (UINT32)(snr[3]) << 24;

    return ival;
}

/******************************************************************
 *
 *      pSNR
 *
 *      Description: Return the base address of array which stores Signal-To-Noise Ratio
 *
 *      Input: 	None
 *      Output: UINT8 *
 *
 ******************************************************************/
UINT8 * pSNR (VOID)
{
    return &snr[0];
}

/******************************************************************
 *
 *      SNRSave
 *
 *      Description: Saves Signal-To-Noise Ratio from IDM response
 *
 *      Input: 	None
 *      Output: UINT32
 *
 ******************************************************************/
VOID SNRSave (UINT8 * pData)
{

	UINT8 * p;
	UINT8 i;

	p = pData;

	for (i = 0; i < 4; i++)
	{
		snr[i] = *p++;
	}
}

/******************************************************************
 *
 *      BERSave
 *
 *      Description: Saves BER from IDM response
 *
 *      Input: 	None
 *      Output: None
 *
 ******************************************************************/
VOID BERSave(UINT8 * pData)
{
	UINT32 *p;
	UINT8 i;

	p = (UINT32*)pData;

	for(i=0;i<10;i++)
	{
		BER[i] = *p++;
	}
}

/******************************************************************
 *
 *      GetBER
 *
 *      Description: Return the base address of array which stores BER
 *
 *      Input: 	None
 *      Output: UINT32 *
 *
 ******************************************************************/
UINT32 * GetBER (VOID)
{
    return &BER[0];
}

#ifdef DRI352
BOOL IsCAProgram (VOID)
{
    return (AcqData.CA_Audio_Programs & (1 << AcqData.current_prog));
}
#endif
