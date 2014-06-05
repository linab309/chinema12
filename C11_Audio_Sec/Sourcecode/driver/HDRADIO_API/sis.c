/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                sis.c											  					**
**																			  				**
** DESCRIPTION: 		Station Information Service data  functions							**
**********************************************************************************************/

#include <processor.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "radio.h"
#define STATION_MESSAGE_LENGTH 190

static UINT8 station_ID[4];
static UINT8 station_location[4];
UINT8 short_station_name[8];
static UINT8 universal_short_station_name[15];
#if 0
static UINT8 long_station_name[57];
static UINT8 universal_long_station_name[65];
#endif
static UINT8 status;
static UINT8 sis_status;
static BOOL bStationLocAvail;
static BOOL bALFNAvail;
static BOOL bStationIDAvail;

static UINT8 stat_msg_priority;
static UINT8 stat_msg_encoding;
static BOOL stat_msg_valid = FALSE;
static UINT8 stat_msg[STATION_MESSAGE_LENGTH];			

static UINT8 alfn[4];
static UINT8 alfn_status;
static UINT8 time_lock_status;

static UINT8 Time_Zone[4];
static UINT8 Leap_Seconds[6];

static INT16 LeapSeconds = 0;

static UINT32 ALFNtoGPStime (UINT32 a);

/******************************************************************************
 *
 *     SISbasicSave
 *
 *	Description : Store SIS data retrieved from extended SIS operation.
 *	Input   	: Pointer to SIS data
 *	Output      : None
 *
 ******************************************************************************/
VOID SISbasicSave (UINT8 * pSISdata)
{
    UINT8 length;
	UINT8 temp_length;
	UINT8 i;
	UINT8 type;
	UINT8 numtypes;
    UINT8 * p;

	/*
	 ** Store ALFN data.
	 */
	p = pSISdata + 4;
	alfn_status = *p++;
	for (i = 0; i < 4; i++)
	{
		alfn[i] = *p++;
	}
	time_lock_status = *p;
	bALFNAvail = TRUE;

	p = pSISdata + 10;
	numtypes = *p++;

	while (numtypes)
	{
		type = *p++;
		status = *p++;
		length = *p++;

		if (status)	//Data is available
		{
			switch (type)
			{
				case 1:	//Station Short Name
					sis_status = status;
					temp_length = length;
					if (length > 7)
					{
						length = 7;
					}
					memcpy((VOID *)short_station_name , (VOID *)p, length);
					short_station_name[length] = '\0';
					*p+=temp_length;
					break;
				case 0:	//Station ID
					bStationIDAvail = TRUE;
					for (i=0;i<4;i++)
						station_ID[i] = *p++;
					break;
				case 2:	//Station Long Name
					sis_status = 4;
					temp_length = length;
					if (length > 56)
					{
						length = 56;
					}
#if 0
					memcpy((VOID *)long_station_name , (VOID *)p, length);
					long_station_name[length] = '\0';
#endif
					*p+=temp_length;
					break;
				case 4://Station Location
					bStationLocAvail = TRUE;
					for (i=0;i<4;i++)
						station_location[i] = *p++;
					break;
				default:
					*p+=length;
					break;
			}
		}
		numtypes--;
	}
}

/******************************************************************************
 *
 *     SISuniversalShortNameSave
 *		Description : Store SIS universal short station name retrieved from extended
 *            		: SIS operation.
 *		Input   	: Pointer to SIS data
 *		Output      : None
 *
 ******************************************************************************/

VOID SISuniversalShortNameSave (UINT8 * pSISdata)
{
    UINT8 * p = pSISdata + 3;
	UINT8 text_encoding = pSISdata[1];
	UINT8 append_byte = pSISdata[2];
	
	if (text_encoding == 0)		//0 = ISO/IEC 8859-1 encoding
	{
		memcpy((VOID *)universal_short_station_name , (VOID *)p, 12);

		/* Append current freq band, if needed */
		if (append_byte == 1)
		{
			if (GetCurrBand() == BAND_FM)
				strcat(universal_short_station_name, "-FM");
			else
				strcat(universal_short_station_name, "-AM");
		}
	}
	else
		universal_short_station_name[0] = '\0';
}

/******************************************************************************
 *
 *     SISuniversalLongNameSave
 *		Description : Store SIS universal long station name retrieved from extended
 *            		: SIS operation.
 *		Input   	: Pointer to SIS data
 *		Output      : None
 *
 ******************************************************************************/
VOID SISuniversalLongNameSave (UINT8 * pSISdata)
{
#if 0
	UINT8 * p = pSISdata + 2;
	UINT8 text_encoding = pSISdata[1];
//	UINT8 append_byte = pSISdata[2];
	
	if (text_encoding == 0)		//0 = ISO/IEC 8859-1 encoding
    memcpy((VOID *)universal_long_station_name , (VOID *)p, 64);
	else
		universal_long_station_name[0] = '\0';
#endif
}


/******************************************************************************
 *
 *     SIStimeZoneSave
 *	Description : Store SIS time zone informatoin retrieved from extended SIS
 *           	: operation.
 *	Input   	: Pointer to SIS data
 *	Output      : None
 *
 ******************************************************************************/

VOID SIStimeZoneSave (UINT8 * pSISdata)
{
	UINT8 * p = pSISdata + 1;
	UINT8 lsb = *p++;
	UINT8 msb = *p++;

	//TimeZoneOffset
	Time_Zone[0] = (msb << 8) + lsb;
	//DST_Schedule
	Time_Zone[1] = *p++;
	//DST_Local
	Time_Zone[2] = *p++;
	//DST_Regional
	Time_Zone[3] = *p;
}

/******************************************************************************
 *
 *     SISLeapSecondsSave
 *	Description : Store SIS Leap seconds informatoin retrieved from extended SIS
 *           	: operation.
 *	Input   	: Pointer to SIS data
 *	Output      : None
 *
 ******************************************************************************/
VOID SISLeapSecondsSave(UINT8 * pSISdata)
{
	UINT32 LeapGPSOffset;
	UINT8 * p = pSISdata + 1;

	//Leap Pending Offset
	Leap_Seconds[0] = *p++;
	//Leap Current Offset
	Leap_Seconds[1] = *p++;

	memcpy(&LeapGPSOffset,p,4);
	LeapGPSOffset = LeapGPSOffset * 10000;
	//Leap GPS Offset
	memcpy(&Leap_Seconds[2],&LeapGPSOffset,4);
}

/******************************************************************************
 *
 *     isSISdataUpdated
 *	Description : Determine if SIS data is available and updated since last read
 *	            : by host.  This is accomplished by checking the data available
 *	            : flag.
 *	Input 		: None
 *	Output      : TRUE if data available flag is set to 2, FALSE otherwise
 *
 ******************************************************************************/

BOOL isSISdataUpdated (VOID)
{
    return (status == 2) ? TRUE : FALSE;
}

/******************************************************************************
 *
 *     isSISdataAvailable
 *	Description : Determine if SIS data is available.  This is accomplished by
 *	            : checking the data available flag.
 *	Input   	: None
 *	Output      : TRUE if data available flag is 1 or 2, FALSE otherwise
 *
 ******************************************************************************/

BOOL isSISdataAvailable (VOID)
{
//    return (status == 1 || status == 2) ? TRUE : FALSE;
	return (sis_status == 1 || sis_status == 2) ? TRUE : FALSE;
}

/******************************************************************************
 *
 *     setSISinvalid
 *	Description : Set all SIS information to invalid and uninitialized state.
 *	Input   	: None
 *	Output      : None
 *
 ******************************************************************************/
VOID setSISinvalid (VOID)
{
	status = 0;
	sis_status = 0;
	memset(short_station_name, 0, 8);
	memset(universal_short_station_name, 0, sizeof(universal_short_station_name));
#if 0
	memset(long_station_name, 0, 57);
	memset(universal_long_station_name, 0, sizeof(universal_long_station_name));
#endif
	memset(station_ID, 0, 4);
	memset(station_location, 0, 4);
//	memset(stat_msg, 0, sizeof(stat_msg));
    alfn_status = 3;
    time_lock_status = 0;
    bStationIDAvail = FALSE;
	bStationLocAvail = FALSE;
	bALFNAvail = FALSE;
}

/******************************************************************************
 *
 *     SIS_ShortStationName
 *	Description : Return short station name to application.
 *	Input   	: None
 *	Output      : Pointer to short station name
 *
 ******************************************************************************/

UINT8 * SIS_ShortStationName (VOID)
{
    return short_station_name;
}

/******************************************************************************
 *
 *     SIS_UniversalShortStationName
 *	Description : Return universal short station name to application.
 *	Input 	    : None
 *	Output      : Pointer to short station name
 *
 ******************************************************************************/

UINT8 * SIS_UniversalShortStationName (VOID)
{
    return universal_short_station_name;
}

/******************************************************************************
 *
 *     isUniversalStationShortNameAvailable
 *	Description : Determine if universal short station name is available.
 *	Input   	: None
 *	Output      : TRUE if available, FALSE otherwise
 *
 ******************************************************************************/

BOOL isUniversalStationShortNameAvailable (VOID)
{
	UINT8 len = strlen(universal_short_station_name);
	return (len > 0) ? TRUE : FALSE;
}
#if 0
/******************************************************************************
 *
 *     SIS_LongStationName
 *	Description : Return long station name.
 *	Input   	: None
 *	Output      : Pointer to long station name
 *
 ******************************************************************************/

UINT8 * SIS_LongStationName (VOID)
{
    return long_station_name;
}

/******************************************************************************
 *
 *     SIS_UniversalLongStationName
 *	Description : Return universal long station name to application.
 *	Input 	    : None
 *	Output      : Pointer to long station name
 *
 ******************************************************************************/
UINT8 * SIS_UniversalLongStationName(VOID)
{
	return universal_long_station_name;
}
#endif

/******************************************************************************
*
*	SIS_IsValidStationMessage
*  Description 	: Return status of station message.
*  Input		: None
*  Output		: BOOL
*
*******************************************************************************/
BOOL SIS_IsValidStationMessage(VOID)
{
	return stat_msg_valid;
}

/******************************************************************************
*
*	SIS_GetStationMessagePriority
*  Description 	: Return prority of station message.
*  Input		: None
*  Output		: UINT8
*
*******************************************************************************/
UINT8 SIS_GetStationMessagePriority(VOID)
{
	return stat_msg_priority;
}

/******************************************************************************
*
*	SIS_GetStationMessageEncoding
*  Description 	: Return encoding method of station message.
*  Input		: None
*  Output		: UINT8
*
*******************************************************************************/
UINT8 SIS_GetStationMessageEncoding(VOID)
{
	return stat_msg_encoding;
}

/******************************************************************************
*
*	 SIS_GetStationMessage
*  Description 	: Return station message.
*  Input		: None
*  Output		: UINT8 *
*
*******************************************************************************/
UINT8 *SIS_GetStationMessage(VOID)
{
	stat_msg_valid = FALSE;			// Must make station message reads 'destructive' because
									// DSP will not return station messages unless new data
	return stat_msg;
}

/******************************************************************************
*
*	 SIS_StationMessageSave
*  Description 	: Store Station message retrieved from extended SIS operation.
*  Input		: UINT8 *
*  Output		: None
*
*******************************************************************************/
VOID SIS_StationMessageSave(UINT8 *pSISdata, UINT8 msg_length)
{
	/*      (Text Encoding Type)
			0x00      : IS0-646 repertoire E.1
			0x01      : IS0-646 repertoire E.2
			0x02      : IS0-646 repertoire E.3
			0x03-0xFF : Reserved
	*/

	/*		(Message Priority)
			0x00      : Regular
			0x01      : High
	*/
	stat_msg_encoding = pSISdata[0];
	stat_msg_priority = pSISdata[1];
	memcpy((void *)stat_msg, (void *)&pSISdata[2], STATION_MESSAGE_LENGTH);		
	if(msg_length > (STATION_MESSAGE_LENGTH - 1)) msg_length = (STATION_MESSAGE_LENGTH - 1);
	stat_msg[msg_length] = '\0';         // Terminate message with null.
	stat_msg_valid = TRUE;
}

#if 1
/******************************************************************************
 *
 *     ALFNstatus
 *	Description : Return ALFN status to application.
 *	Input   	: None
 *	Output      : ALFN status
 *
 ******************************************************************************/

UINT8 ALFNstatus (VOID)
{
    return alfn_status;
}

/******************************************************************************
 *
 *     isALFNvalid
 *	Description : Return ALFN validity to application.
 *	Input   	: None
 *	Output      : 1 if ALFN valid, 0 otherwise
 *
 ******************************************************************************/

BOOL isALFNvalid (VOID)
{
    return (alfn_status == 0) ? TRUE : FALSE;
}

/******************************************************************************
 *
 *     isALFNtimeLocked
 *	Description : Return ALFN time lock status to application.
 *	Input   	: None
 *	Output      : ALFN time lock status
 *
 ******************************************************************************/
BOOL isALFNtimeLocked (VOID)
{
    return (time_lock_status == 1) ? TRUE : FALSE;
}

/******************************************************************************
 *
 *     ALFN
 *	Description : Convert 4-byte ALFN to 32-bits and return 32-bit number.
 *	Input   	: None
 *	Output      : 32-bit ALFN number
 *
 ******************************************************************************/
UINT32 ALFN (VOID)
{
    UINT32 ival = 0;

    ival += (UINT32)(alfn[0]);
    ival += (UINT32)(alfn[1]) << 8;
    ival += (UINT32)(alfn[2]) << 16;
    ival += (UINT32)(alfn[3]) << 24;

    return ival;
}

/******************************************************************************
 *
 *     ALFNtime
 *
 *	Description : Calculate and save time
 *	Input   	: UINT16 *
 *	Output      : None
 *
 ******************************************************************************/
VOID ALFNtime (UINT16 * p)
{
    UINT32 alfn;
    UINT32 gps;
    UINT32 RemSeconds;
    UINT16 RemYears;
    UINT16 NumLeapYears;
    UINT16 CurYear;
    UINT16 CurDayOfYear;
//    UINT8 dt[RTC_PARAMS];
    UINT8 CurHour;
    UINT8 CurMinute;
    UINT8 CurSecond;
	UINT8 TimeZoneOffset = 0;

	TimeZoneOffset = Time_Zone[0];
    /*
    ** 1.  Convert ALFN to GPS time in seconds - this is equivalent to the number
    **     of elapsed seconds since Jan. 6, 1980 at 00:00:00 UTC.
    */
    alfn = ALFN();
    gps = ALFNtoGPStime(alfn);
    RemSeconds = gps;

    /*
    ** 2.  Correct for leap seconds.
    */
    RemSeconds -= (UINT32)LeapSeconds;

    /*
    ** 3.  Correct for the local time zone.
    */
    RemSeconds += (60l * (UINT32)TimeZoneOffset);

    /*
    ** 4.  Subract out the number of seconds in 1980 starting from Jan. 6 (GPS epoch).
    */
    RemSeconds -= 31190400l;

    /*
    ** 5.  Divide by number of seconds in a 4-year period (including a leap day) to
    **     compute number of leap years.
    */
    NumLeapYears = RemSeconds/126230400l;

    /*
    ** 6.  Subract the number of seconds in the integral number of 4-year periods
    **     calculated in step 5.
    */
    RemSeconds -= ((UINT32)NumLeapYears * 126230400l);

    /*
    ** 7.  Calculate the remaining number of complete years.
    */
    RemYears = RemSeconds/31536000l;

    /*
    ** 8.  If the remaining number of years is 4, this must mean that it is the last
    **     day of a leap year (year 3, day 365).
    */
    if (RemYears == 4)
    {
        RemYears = 3;
    }

    /*
    ** 9.  Calculate the current year, given that GPS time started in 1980.
    */
    CurYear = 1980 + 4*NumLeapYears + RemYears + 1;

    /*
    ** 10. Subract the number of seconds in the integral number of years computed in
    **     steps 7 and 8.
    */
    RemSeconds -= ((UINT32)RemYears * 31536000l);

    /*
    ** 11. Calculate the current day of year from the remaining number of seconds.
    */
    CurDayOfYear = RemSeconds/86400l;

    /*
    ** 12. Subract the number of seconds in the days computed in step 11.
    */
    RemSeconds -= (86400l*(UINT32)CurDayOfYear);

    /*
    ** 13. Calculate the current hour from the remaining number of seconds.
    */
    CurHour = RemSeconds/3600l;

    /*
    ** 14. Subract the number of seconds in the hours computed in step 13.
    */
    RemSeconds -= (3600l*CurHour);

    /*
    ** 15. Calculate the current minute from the remaining number of seconds.
    */
    CurMinute = RemSeconds/60l;

    /*
    ** 16. Subract the number of seconds in the minutes computed in step 15.
    **     This value is equal to the current second.
    */
    CurSecond = RemSeconds - 60l*CurMinute;

    /*
    ** Now Save.
    */
	*p++ = CurYear;
	*p++ = CurDayOfYear;
	*p++ = CurHour;
	*p++ = CurMinute;
	*p++ = CurSecond;
}


/******************************************************************************
 *
 *     ALFNtoGPStime
 *
 *	Description : Convert ALFN to GPS time in seconds.
 *	Input   	: UINT32
 *	Output      : static UINT32
 *
 ******************************************************************************/
static UINT32 ALFNtoGPStime (UINT32 a)
{
    UINT32 b = 0xBE37C63Bl; // 32-bit integer value for 65536/44100, lsb:2^-31
    UINT32 result;
    UINT32 temp;

    result = (((a & 0x0000FFFF) * (b & 0x0000FFFF)) + 0x40000000l) >> 31;
    result = result + ((((a >> 16) * (b & 0x0000FFFF)) + 0x4000) >> 15);
    result = result + ((((b >> 16) * (a & 0x0000FFFF)) + 0x4000) >> 15);
    temp = (a >> 16) * (b >> 16);
    result = result + (temp << 1);

    return result;
}

#endif

/******************************************************************************
 *
 *     SIS_IsALFNAvail
 *
 *	Description : Returns TRUE if ALFN is available.
 *	Input   	: None
 *	Output      : BOOL
 *
 ******************************************************************************/
BOOL SIS_IsALFNAvail(VOID)
{
	return bALFNAvail;
}

/******************************************************************************
 *
 *     SIS_IsStationIDAvail
 *
 *	Description : Returns TRUE if Station ID is available.
 *	Input   	: None
 *	Output      : BOOL
 *
 ******************************************************************************/
BOOL SIS_IsStationIDAvail(VOID)
{
	return bStationIDAvail;
}

/******************************************************************************
 *
 *     SIS_GetStationID
 *
 *	Description : Returns the Station ID.
 *	Input   	: None
 *	Output      : UINT8*
 *
 ******************************************************************************/
UINT8 *SIS_GetStationID(VOID)
{
	return station_ID;
}
/******************************************************************************
 *
 *     SIS_CountryCode
 *
 *	Description : Returns the Country Code.
 *	Input   	: None
 *	Output      : UINT16
 *
 ******************************************************************************/
UINT16 SIS_CountryCode(VOID)
{
	 UINT16 countryCode = (station_ID[0]<<2) | (station_ID[1]>>6);
 	 return countryCode;
}

/******************************************************************************
 *
 *     SIS_FacilityCode
 *
 *	Description : Returns the Facility Code.
 *	Input   	: None
 *	Output      : UINT32
 *
 ******************************************************************************/
UINT32 SIS_FacilityCode(VOID)
{
 	UINT32 facilityCode;
	facilityCode = station_ID[1] & 0x07; //Bits 13, 14, 15 of station ID
	facilityCode <<= 8;
	facilityCode |= station_ID[2];
	facilityCode <<= 8;
	facilityCode |= station_ID[3];
 	return facilityCode;
}

/******************************************************************************
 *
 *     SIS_IsStationLocationAvail
 *
 *	Description : Returns TRUE if Station Location is available.
 *	Input   	: None
 *	Output      : BOOL
 *
 ******************************************************************************/
BOOL SIS_IsStationLocationAvail(VOID)
{
	return bStationLocAvail;
}

/******************************************************************************
 *
 *     SIS_GetStationLocation
 *
 *	Description : Returns the Station Location
 *	Input   	: None
 *	Output      : UINT8*
 *
 ******************************************************************************/
UINT8 *SIS_GetStationLocation(VOID)
{
	return station_location;
}

/******************************************************************************
 *
 *     SIS_GetTimeZone
 *	Description : Return Time zone to application.
 *	Input   	: None
 *	Output      : UINT8 *
 *
 ******************************************************************************/
UINT8 *SIS_GetTimeZone(VOID)
{
	return Time_Zone;
}

/******************************************************************************
 *
 *     SIS_GetTimeZone
 *	Description : Return Leap Seconds to application.
 *	Input   	: None
 *	Output      : UINT8 *
 *
 ******************************************************************************/
UINT8 *SIS_GetLeapSeconds(VOID)
{
	return Leap_Seconds;
}
