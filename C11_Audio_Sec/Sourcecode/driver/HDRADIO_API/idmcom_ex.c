/*********************************************************************************************
* iBiquity Digital Proprietary                                                              **
* Copyright 2005 iBiquity Digital Corporation                                               **
**                                                                                          **
** PROJECT:             HD Radio SDK                                                        **
**                                                                                          **
** FILE:                idmcom.C                                                            **
**                                                                                          **
** DESCRIPTION:                 contains the functions necessary to control the DRI350 IDM  **
**********************************************************************************************/

#include <string.h>
#include <math.h>
//#include "Processor.h"
//#include "idmcom.h"
//#include "avr_timer_drv.h"
#ifdef ENABLE_STORE_N_PLAY
#include "storeplay_api.h"
#endif
//#include "TDGA2.h"
#include "ID3parse.h"
//#include "psd.h"
//#include "idmstat.h"
//#include "sis.h"
//#include "../radio_task.h"
//#ifdef IDM_SERIAL_COMMUNICATION
//#include "SerialTask.h"
//#endif
#define _ENABLE_EXTENDED_IDM_SUPPORT
#ifdef _ENABLE_EXTENDED_IDM_SUPPORT

#define DRI350_I2C_READ_WRITE   0x01
#define BAND_SELECT             0x02
#define ADD_SERVICE             0x51

#ifndef _TUNER_ATMEL
#ifdef KSE_FM_PGA_GAIN
#define FM_PGA_GAIN             0x02   //070915 1.6->1.33 //dabb. 07.07.13 change to 1.6 for DSP S/W DRI-352-001751EF-CMRL-V016 //0x01    // 2.00
#else
#define FM_PGA_GAIN             0x01    // 2.00
#endif
#ifdef KSE_AM_PGA_GAIN
#define AM_PGA_GAIN             0x01    //dabb 07.07.16 change to 2.0 for DSP S/W DRI-352-001751EF-CMRL-V016 //0x05  //2.67 
#else
#define AM_PGA_GAIN             0x05    // 2.67
#endif
#else	//_TUNER_ATMEL
#define FM_PGA_GAIN             0x00    // 1.00
#define AM_PGA_GAIN             0x00    // 1.00
#endif

#define MP1	0x01
#define MP2	0x02
#define MP3	0x03
#define MP5	0x05
#define MP6	0x06

#define MA1	0x01
#define MA3	0x03

UINT8 idmdata[CMD_BUFFER_SIZE];
BOOL bSNRPalying = FALSE;
UINT32 g_nFMLeftRightScaling = 0;


/******************************************************************
 *
 *      get_seek_value
 *
 *      Description: Reads the PLL error of the DSP.
 *
 *      Input:  UINT8 band
 *      Output: UINT32
 *
 ******************************************************************/
UINT32 get_seek_value (UINT8 band)
{
    UINT32 seek;

    UINT8 data_buffer[1];

    if (band == BAND_FM)
    {
        data_buffer[0] = GET_FM_SEEK_STRENGTH;
        Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                                14,
                                1,
                                &data_buffer[0]);
    }
    else
	{
#ifdef DRI352
        data_buffer[0] = GET_AM_SEEK_STRENGTH;
        Build_IDM_write_message(AM_DEMOD_CTRL_OPCODE,
                                14,
                                1,
								&data_buffer[0]);
#endif
    }


    IDMtransfer();

    seek = 0l;
    seek += (UINT32)(idmdata[13]);
    seek += (UINT32)(idmdata[14]) << 8;
    seek += (UINT32)(idmdata[15]) << 16;
    seek += (UINT32)(idmdata[16]) << 24;

    return seek;
}

/******************************************************************
 *
 *      iboc_acquire
 *
 *      Description:Reacqurie command (used when tuning).
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void iboc_acquire(void)
{
    UINT8 data_buffer[1];

    Build_IDM_write_message(ACQUIRE_OPCODE,
                            13,
                            0,
                            &data_buffer[0]);

    IDMtransfer();
}

/******************************************************************
 *
 *      select_program
 *
 *      Description: selects multicast program.
 *
 *      Input:  UINT8 program
 *      Output: None
 *
 ******************************************************************/
void select_program(UINT8 program)
{
    UINT8 data_buffer[2];

    data_buffer[0] = 0x00;
    data_buffer[1] = 1 << program;

    Build_IDM_write_message(SEL_PROG_OPCODE,
                            15,
                            2,
                            &data_buffer[0]);

    IDMtransfer();
}


/******************************************************************
 *
 *      get_acq_status
 *
 *      Description:Gets acquire status
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void get_acq_status (void)
{
    Build_IDM_write_message(ACQ_STATUS_OPCODE,
                            13,
                            0,
                            (void *)0);

    if (IDMtransfer())
    {
    #ifdef OTHER_DEFINE
        AcqStatusSave(&idmdata[12]);
   #endif	
    }
}

/******************************************************************
 *
 *      get_basic_sis_data
 *
 *      Description:Gets the SIS data from the IDM
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void get_basic_sis_data(void)
{
    UINT8 data_buffer[1];

    //to do --check what data services are enabled
    data_buffer[0] = 0x05; //port ID must be 5 for basic SIS


    Build_IDM_write_message(GET_SIS_DATA_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
    #ifdef OTHER_DEFINE
        SISbasicSave(&idmdata[12]);
    #endif	
    }
}


/******************************************************************
 *
 *      GetSISuniversalShortName
 *
 *      Description:Gets the SIS Universal Short Name
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void GetSISuniversalShortName (void)
{
    UINT8 data_buffer[1];

    data_buffer[0] = 0x04; //port ID must be 4


    Build_IDM_write_message(GET_SIS_DATA_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
    #ifdef OTHER_DEFINE
        SISuniversalShortNameSave(&idmdata[12]);
    #endif	
    }
}

/******************************************************************
 *
 *      GetSISuniversalLongName
 *
 *      Description:Gets the SIS Universal Long Name
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void GetSISuniversalLongName (void)
{
    UINT8 data_buffer[1];

    data_buffer[0] = 0x06; //port ID must be 6 - Get_Slogan. Long station is different from this. 


    Build_IDM_write_message(GET_SIS_DATA_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
        #ifdef OTHER_DEFINE
	        SISuniversalLongNameSave(&idmdata[12]);
	#endif	
    }
}

/******************************************************************
 *
 *      GetSIStimeZone
 *
 *      Description:Gets the SIS Time Zone information
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void GetSIStimeZone (void)
{
    UINT8 data_buffer[1];

    data_buffer[0] = 0x02; //port ID must be 2


    Build_IDM_write_message(GET_SIS_DATA_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
        #ifdef OTHER_DEFINE
	        SIStimeZoneSave(&idmdata[12]);
	#endif		
    }
}


/******************************************************************
 *
 *      GetSISLeapSeconds
 *
 *      Description:Gets the SIS Leap Seconds information
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void GetSISLeapSeconds (void)
{
    UINT8 data_buffer[1];

    data_buffer[0] = 0x03; //port ID must be 3


    Build_IDM_write_message(GET_SIS_DATA_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
        #ifdef OTHER_DEFINE    
	        SISLeapSecondsSave(&idmdata[12]);
	#endif	
    }
}

/******************************************************************
 *
 *      get_psd
 *
 *      Description:Gets the program service data from the IDM
 *
 *      Input:  None
 *      Output: None
 *  
 ******************************************************************/
void get_psd (void)
{
    UINT16 numberOfSegments;
    UINT16 segmentNumber;
    UINT8 SessionType;
    UINT8 data_buffer[2];
	BOOL valid_transfer, valid_psd;

    data_buffer[0] = 0x04; // port ID must be 4
    data_buffer[1] = 0xFE;

	// As the max size of a PSD packet is 1Kbytes,
	// read data in segments, starting with 0.
	segmentNumber = 0;
	valid_psd = FALSE;
	do
	{
		Build_IDM_write_message(GET_PSD_OPCODE,
								15,
								2,
								&data_buffer[0]);
		// Setup segment number.
		idmdata[11] = (UINT8)((segmentNumber >> 8) & 0xFF);
		idmdata[10] = (UINT8)(segmentNumber & 0xFF);
		idmdata[14] = CalcCheckSum(15, &idmdata[0]);

#ifdef IDM_SERIAL_COMMUNICATION
		IdmSerialWrite(idmdata[4], &idmdata[0]);
		//Timer_UsDelay(150);
		valid_transfer = IdmSerialRead (idmdata[6], sizeof(idmdata), &idmdata[0]);
#else
		IdmI2CWrite(BBP_SLAVE_ADDRESS,
					idmdata[4],
					&idmdata[0]);

		Timer_UsDelay(150);
		valid_transfer = IdmI2CExRead(BBP_SLAVE_ADDRESS, &idmdata[0]);
#endif
        if (!valid_transfer || (idmdata[4] == 0))
        {
            break;
        }

        // Read returned segment number then increment for next time.
        segmentNumber    = ((UINT16)idmdata[11] << 8) + (UINT16)idmdata[10];
        numberOfSegments = ((UINT16)idmdata[9] << 8) + (UINT16)idmdata[8];
        segmentNumber++;

		SessionType = idmdata[17];
#ifdef ENABLE_STORE_N_PLAY
		if (SessionType == GetCurrProgram() || SP_isPlaying())
//#else
		if (SessionType == GetCurrProgram())
#endif
        {
			if(segmentNumber == 1)
			{
				if (Parse_ID3((char *)&idmdata[22]) == 1)
				{
				   valid_psd = TRUE;
				}
			}
		}
    }
    while (segmentNumber < numberOfSegments);

	if (valid_psd)
		setPSDvalid();
//	else
//		setPSDinvalid();
}

/*****************************_*************************************
 *
 *      FlushPSD
 *
 *      Description: Flush PSD Queue for given program
 *
 *      Input:  UINT8 program
 *      Output: None
 *
 ******************************************************************/
void FlushPSD (UINT8 program)
{
    UINT8 data_buffer[3];

    data_buffer[0] = 0x04; // port ID must be 4
    data_buffer[1] = 0xFE;
    data_buffer[2] = program;

    Build_IDM_write_message(FLUSH_PSD_QUEUE_OPCODE,
                            16,
                            3,
                            &data_buffer[0]);

    IDMtransfer();
}

/******************************************************************
 *
 *      FlushAllPSD
 *
 *      Description:Flush All PSD
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void FlushAllPSD (void)
{
	UINT8 data_buffer[2];

	data_buffer[0] = 0x04;	// port ID must be 4
	data_buffer[1] = 0x00;

	Build_IDM_write_message(FLUSH_PSD_QUEUE_OPCODE,
							13,
							0,
							&data_buffer[0]);

	IDMtransfer();
        #ifdef OTHER_DEFINE	
		setPSDinvalid();
	#endif	
}

/******************************************************************
 *
 *      FlushSIS
 *
 *      Description:Flush SIS
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void FlushSIS (void)
{
	UINT8 data_buffer[1];

	data_buffer[0] = 0x05; // function code must be 5

	Build_IDM_write_message(FLUSH_SIS_DATA_OPCODE,
							14,
							3,
							&data_buffer[0]);

	IDMtransfer();
}

/******************************************************************
 *
 *      GetSISstationMessage
 *
 *      Description: Reads the SIS Station Message from the IDM and
 *                   stores values SIS variables.
 *      Input : None
 *      Output: None
 ******************************************************************/
void GetSISstationMessage (void)
{
    UINT8 data_buffer[1];
    UINT8 msglength = 0;

    // Function code for Extended SIS Get_Station_Message
    data_buffer[0] = 0x01;

    Build_IDM_write_message(GET_SIS_DATA_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

	// If transfer was successful, check validity of the message by
	// verifying returned function code (0x01) and making sure IDM
	// was able to write data (length != 0x0B).

    if (IDMtransfer())
    {
		if(idmdata[12] == 0x01 && idmdata[4] != 0x0E)
		{
			msglength = idmdata[4] - 16;// Subract out length of header and other data bytes.
		   #ifdef OTHER_DEFINE		
	        	SIS_StationMessageSave(&idmdata[13], msglength);
		   #endif
		}
    }
}

/******************************************************************
 *
 *      add_service_aux1
 *
 *      Description:Routes the HD audio to primary DAC.The Values are Hard coded for Now.
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void add_service_aux1 (void)
{
#ifdef DRI352
    UINT8 data_buffer[4];

    data_buffer[0] = 0x00;  // Add Audio Proc Services
    data_buffer[1] = 0x10;      // Add AUX 1
    data_buffer[2] = 0x00;
    data_buffer[3] = 0x00;

    Build_IDM_write_message(ADD_SERVICE,
                            17,
                            4,
                            &data_buffer[0]);

	IDMtransfer();
#endif
}

/******************************************************************
 *
 *      add_service_audproc
 *
 *      Description:Add service audio processing
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void add_service_audproc (void)
{
#ifdef DRI352	
    UINT8 data_buffer[4];

    data_buffer[0] = 0x40;  // Add Audio Proc Services
    data_buffer[1] = 0x00;      // Add AUX 1
    data_buffer[2] = 0x00;
    data_buffer[3] = 0x00;

    Build_IDM_write_message(ADD_SERVICE,
                            17,
                            4,
                            &data_buffer[0]);

	IDMtransfer();
#endif
}

/******************************************************************
 *
 *      idm_route_dac
 *
 *      Description:Routes the HD audio to primary DAC
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void idm_route_dac(UINT8 source)
{
#ifdef DRI352
    UINT8 data_buffer[5];

        switch(source)
        {
        case 0:  // HD Radio (AM/FM)
			data_buffer[0] = 0x0F;	//Source:	bit 0 - Digital FM
									//			bit 1 - Digital AM
									//			bit 2 - Analog  FM
									//			bit 3 - Analog  AM
			data_buffer[1] = 0x00;
			data_buffer[2] = 0x00;
			data_buffer[3] = 0x00;	//reserved
			break;

        case 1:  // Aux In
			data_buffer[0] = 0x00;
			data_buffer[1] = 0x10;	//Source:	bit 4 - AUX 1
			data_buffer[2] = 0x00;	//Source:	bit 1 - AUX 2
			data_buffer[3] = 0x00;	//reserved
			break;
        }
		data_buffer[4] = 0x03;	//Destination:	bit 0 - DAC 1
								//				bit 1 - DAC 2

    Build_IDM_write_message(ROUTE_AUDIO,
                            18,
                            5,
                            &data_buffer[0]);

	IDMtransfer();
#endif
}

/******************************************************************
 *
 *      idm_get_surround_mode
 *
 *      Description: Get HD surround mode of a particular program
 *
 *      Input:  UINT8 program number (0-7)
 *      Output: UINT8 Surround mode:
 *								0x00	None
 *								0x01	SRS Circle Surround
 *								0x02	Dolby ProLogic2 Surround
 *								0x03	Neural Surround
 *								0x04	FHG HEMP3 Surround
 *								0x05	DTS Matrix Surround
 *								0x06	CT Surround
 *								0xFF	Program not available

 ******************************************************************/
UINT8 idm_get_surround_mode (UINT8 program)
{

	UINT8 data_buffer[2];
	data_buffer[0] = 0x00;	//get_specific_audio_program_info
	data_buffer[1] = program;

	Build_IDM_write_message(GET_SERVICE_INFO_OPCODE,
							15,
							2,
							&data_buffer[0]);

	if (IDMtransfer())
	{
		if (idmdata[5]==0x11)
			return idmdata[17];
		else
			return 0x00;
	}
	return 0x00;
}

/******************************************************************
 *
 *      set_mode
 *
 *      Description:Set the IDM to the mode (AM or FM) specified in the HD_Data structure
 *
 *      Input:  UINT8 band
 *      Output: None
 *
 ******************************************************************/
void set_mode (UINT8 band)
{
    UINT8 data_buffer[1];

    data_buffer[0] = (band == BAND_FM) ? 1 : 0;


    Build_IDM_write_message(AM_FM_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);


    IDMtransfer();
}


/******************************************************************
 *
 *      get_blend_mode
 *
 *      Description: Get blend mode for specified service mode
 *
 *					0x00	No blending - always output analog audio
 *					0x01	Blend to analog the earliest
 *					0x02	Blend to analog the second earliest
 *					0x03	Blend to analog the third earliest
 *					0x04	Stay in digital the longest
 *					0x05	N/A
 *					0x06	N/A
 *					0x07	No blending -  always output digital audio
 *
 *      Input:  UINT8 service mode number
 *      Output: UINT8 blend mode
 *
 ******************************************************************/
UINT8 get_blend_mode (UINT8 service)
{
#ifdef DRI352
	UINT8 data_buffer;

	data_buffer = 0;

	Build_IDM_write_message(GET_CNFG_OPCODE,
							14,
							1,
							&data_buffer);

	IDMtransfer();

	return idmdata[service+12];
#else
	service = FALSE;		//redundant - just to avoid warning at compile time
	return service;
#endif
}

/******************************************************************
 *
 *      set_blend_mode
 *
 *      Description: Select blend mode: Force Audio to Analog, auto-blend, or force digital
 *				0x00	No blending - always output analog audio
 *				0x01	Blend to analog the earliest
 *				0x02	Blend to analog the second earliest
 *			  0x03	Blend to analog the third earliest
 *			  0x04	Stay in digital the longest
 *			  0x05	N/A
 *			  0x06	N/A
 *			  0x07	No blending -  always output digital audio
 *				0xFF	invalid: this is indication to revert to original values
 *      Input:  UINT8 blend
 *      Output: None
 *
 ******************************************************************/
void set_blend_mode (UINT8 blend)
{
#ifdef DRI352
	UINT8 data_buffer;
	static UINT8 original_blend[6];
	static BOOL	called = FALSE;

	data_buffer = 0;

	Build_IDM_write_message(GET_CNFG_OPCODE,
							14,
							1,
							&data_buffer);

	IDMtransfer();

	if (!called)
	{
		original_blend[0] = idmdata[0x12];
		original_blend[1] = idmdata[0x19];
		original_blend[2] = idmdata[0x20];
		original_blend[3] = idmdata[0x27];
		original_blend[4] = idmdata[0x58];
		original_blend[5] = idmdata[0x6D];
		called = TRUE;
	}

	idmdata[11] = 0x01;		//Set config parameters

	if (blend==0xFF)
	{
		idmdata[0x12] = original_blend[0];	//Codec Mode 0
		idmdata[0x19] = original_blend[1];	//Codec Mode 1
		idmdata[0x20] = original_blend[2];	//Codec Mode 2
		idmdata[0x27] = original_blend[3];	//Codec Mode 3
		idmdata[0x58] = original_blend[4];	//Codec Mode 10
		idmdata[0x6D] = original_blend[5];	//Codec Mode 13
	}
	else
	{
		idmdata[0x12] = blend;	//Codec Mode 0
		idmdata[0x19] = blend;	//Codec Mode 1
		idmdata[0x20] = blend;	//Codec Mode 2
		idmdata[0x27] = blend;	//Codec Mode 3
		idmdata[0x58] = blend;	//Codec Mode 10
		idmdata[0x6D] = blend;	//Codec Mode 13
	}

	Build_IDM_write_message(SET_CNFG_OPCODE,
							145,
							132,
							&idmdata[11]);

	IDMtransfer();
#endif
}

/******************************************************************
 *
 *      set_sis_config
 *
 *      Description:Tells the IDM what type of SIS data to send to the host
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void set_sis_config(void)
{
    UINT8 data_buffer[3];

    data_buffer[0] = 0x05; //port ID must be 5
    data_buffer[1] = 0x01; //one type of data to get
    data_buffer[2] = 0x01; //get short name for now.


    Build_IDM_write_message(SET_SIS_CNFG_OPCODE,
                            16,
                            3,
                            &data_buffer[0]);

    IDMtransfer();
}

/******************************************************************
 *
 *      set_aux_mode
 *
 *      Description:Set Auxiliary Mode
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void set_aux_mode (void)
{
#ifdef DRI352
    UINT8 data_buffer[1];

    data_buffer[0] = 0x03;  // 32-bit ADC slave mode


    Build_IDM_write_message(AUX_SET_MODE,
                            14,
                            1,
                            &data_buffer[0]);

	IDMtransfer();
#endif
}

/******************************************************************
 *
 *      idm_set_FM_stereo_mode
 *
 *      Description: Set FM stereo mode
 *
 *      Input:  mode
 *      Output: None
 *
 ******************************************************************/
void idm_set_FM_stereo_mode (UINT8 mode)
{
	UINT8 data_buffer[5];
	data_buffer[0] = ANALOG_FM_STEREO_CTRL_OPCODE;
	data_buffer[1] = mode;	//0x00 - mute
							//0x01 - mono
							//0x02 - stereo
							//0x03 - automatic
	data_buffer[2] = 0;
	data_buffer[3] = 0;
	data_buffer[4] = 0;

	Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
							18,
							5,
							&data_buffer[0]);

	if (IDMtransfer())
	{
		//idm_get_FM_status();
	}
}

/******************************************************************
 *
 *      GetSysStatus
 *
 *      Description: Get System Status
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void GetSysStatus(void)
{
	UINT8 data_buffer;

	data_buffer = 0x00;

	Build_IDM_write_message(SYS_STATUS_OPCODE,
							14,
							1,
							&data_buffer);
	IDMtransfer();
}

/******************************************************************
 *
 *      idm_get_AM_status
 *
 *      Description: Get AM status
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/

void idm_get_AM_status (void)
{
#ifdef DRI352
	UINT8 data_buffer[1];

	data_buffer[0] = AM_GET_STATUS;

	Build_IDM_write_message(AM_DEMOD_CTRL_OPCODE,
							14,
							1,
							&data_buffer[0]);

	if (IDMtransfer())
	{
#ifdef OTHER_DEFINE 
		SetStereoStatus(idmdata[49]);
#endif
	}
#endif
}

/******************************************************************
 *
 *      idm_get_FM_status
 *
 *      Description: Get FM status
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void idm_get_FM_status (void)
{

    UINT8 data_buffer[1];

    data_buffer[0] = FM_GET_STATUS;

    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
    #ifdef OTHER_DEFINE 
        SetStereoStatus(idmdata[13]);
    #endif	
    }
}


/******************************************************************
 *
 *      set_audio_processing
 *
 *      Description:Provides access to audio properties
 *
 *      Input:  UINT8 * pData, UINT8 length
 *      Output: None
 *
 ******************************************************************/
void set_audio_processing (UINT8 * pData, UINT8 length)
{
#ifdef DRI352	
    UINT8 i;
    UINT8 data_buffer[20];

    for (i = 0; i < length; i++)
    {
        data_buffer[i] = *pData++;
    }

    Build_IDM_write_message(SET_AUDIO_PROCESSING,
                            length + 13,
                            length,
                            &data_buffer[0]);


	IDMtransfer();
#endif
}

/******************************************************************
 *
 *      get_apc_data
 *
 *      Description: Gets current audio properties
 *
 *      Input:  const UINT8 * pData, UINT8 length
 *      Output: None
 *
 ******************************************************************/
void get_apc_data(UINT8* pData, UINT8 length)
{
#ifdef DRI352
    UINT8 data_buffer;

    data_buffer = 0x10;

    Build_IDM_write_message(SET_AUDIO_PROCESSING,
                            14,
                            1,
                            &data_buffer);


    if(IDMtransfer())
    {
		memcpy(pData, &idmdata[14], length);
	}
#endif
}

/******************************************************************
 *
 *      get_SAVU_data
 *
 *      Description: Gets current APC SAVU Properties
 *
 *      Input:  const UINT8 * pData, UINT8 length
 *      Output: None
 *
 ******************************************************************/
void get_SAVU_data(UINT8* pData, UINT8 length)
{
#ifdef DRI352
	UINT8 i;
	UINT8 data_buffer;
	UINT8 freq;

	data_buffer = 0x0D;

	Build_IDM_write_message(SET_AUDIO_PROCESSING,
							14,
							1,
							&data_buffer);


	if(IDMtransfer())
	{
		memcpy(pData, &idmdata[14], length);

		//Front L/R
		for(i=0;i<10;i++)
		{
			freq = pData[i];
			freq = (~freq + 1) & 0xFF;
			pData[i] = 100 + (freq *-1);
		}

		//Rear L/R
		for(i=10;i<20;i++)
		{
			freq = pData[i];
			freq = (~freq + 1) & 0xFF;
			pData[i] = 100 + (freq *-1);
		}
	}
#endif
}

/******************************************************************
 *
 *      ReadSoftwareVersion
 *
 *      Description:Reads Software Version
 *
 *      Input:  UINT8 * p, UINT8 len
 *      Output: None
 *
 ******************************************************************/
void ReadSoftwareVersion(UINT8 * p, UINT8 len)
{
#ifdef DRI352
    UINT8 data_buffer[1];
    UINT8 *pIDM = &idmdata[12];
    UINT8 i;

    data_buffer[0] = 0x00; // No forward data.


    Build_IDM_write_message(SW_VER_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
        for (i = 0; i < (len - 1) && *pIDM != 0; i++)
        {
            *p = *pIDM;
            p++;
            pIDM++;
        }
        *p = '\0';
    }
#else
    UINT8 data_buffer[1];
    UINT8 *pIDM = &idmdata[13];
    UINT8 i;

    data_buffer[0] = GET_SW_VERSION_FC;


    Build_IDM_write_message(SYS_VERSION_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
        for (i = 0; i < (len - 1) && *pIDM != 0; i++)
        {
            *p = *pIDM;
            p++;
            pIDM++;
        }
        *p = '\0';
	}
#endif
}


/******************************************************************
 *
 *      Build_IDM_write_message
 *
 *      Description:Builds the IDM's command packet
 *
 *      Input:  UINT8 opcode,
 *              UINT8 msg_length,
 *              UINT8 data_length,
 *              UINT8 *cmd_data
 *      Output: None
 *
 ******************************************************************/

void Build_IDM_write_message (UINT8 opcode, //opcode for the command
                              UINT8 msg_length, //length of the completed packet.
                              UINT8 data_length, //length of the cmd_data buffer
                              UINT8 *cmd_data) //command specific data
{
    static UINT8 LM_count = 0;
    INT16 i;
    idmdata[0] = 0xA5;
    idmdata[1] = 0xA5;
    idmdata[2] = LM_count;
    if (LM_count == 0xFF)
        LM_count = 0x00;
    else
        LM_count++;

	idmdata[3] = 1;
	idmdata[4] = msg_length;
    idmdata[5] = 0;
    idmdata[6] = opcode;
    idmdata[7] = 0x00 ;  // Resume (Hard code for now)
    idmdata[8] = 0x01;   // Number of Segments (Hard code for Now)
    idmdata[9] = 0x00;   // Number of Segments (Hard code for Now)
    idmdata[10] = 0x00;   // Segment Number (Hard code for Now)
    idmdata[11] = 0x00;   // Segment Number (Hard code for Now)

    for (i=0;i<data_length; i++)
    {
        idmdata[i+12] = cmd_data[i];
    }
    idmdata[msg_length-1] = CalcCheckSum(msg_length, &idmdata[0]);

}

/******************************************************************
 *
 *      CalcCheckSum
 *
 *      Description:Calculates the checksum for the Build_IDM_write_message Function.
 *
 *      Input:  INT16 BuffLen,UINT8 *Buffer
 *      Output: UINT8
 *
 ******************************************************************/
UINT8  CalcCheckSum(INT16 BuffLen,
                            UINT8 *Buffer)
{
    INT16 i;
    UINT8 ChkSum;
    INT16 NumElements;
    //ignore last element of buffer
    //this will hold the checksum
    NumElements = BuffLen - 1;
    ChkSum = 0;
    for (i=0; i<NumElements; i++)
    {
        ChkSum += Buffer[i];
    }
    ChkSum &= 0xFF; //Limit it to 1 byte
    return ChkSum;
}


/******************************************************************
 *
 *      IDMtransfer
 *
 *      Description:The write/read sequence to the IDM is common to many function so it is
 *                                      wrapped up here.
 *
 *      Input:  None
 *      Output: BOOL
 *
 ******************************************************************/
BOOL IDMtransfer(void)
{
	BOOL valid;

#ifdef IDM_SERIAL_COMMUNICATION
	//Timer_UsDelay(150);
    //valid = IdmSerialWrite(idmdata[4], &idmdata[0]);
    if (valid)
	{
	//	valid = IdmSerialRead (idmdata[6], sizeof(idmdata), &idmdata[0]);
	}
#else
	Timer_UsDelay(150);
    valid = IdmI2CWrite( BBP_SLAVE_ADDRESS, idmdata[4], &idmdata[0]);
    if (valid)
	{
        Timer_UsDelay(150);	
        valid = IdmI2CExRead(BBP_SLAVE_ADDRESS, &idmdata[0]);
	}
#endif
	if(!valid)
		return FALSE;
	return valid;
}

BOOL get_rbds_group(UINT8* buf)
{
#if 1//def DRI352
	 UINT8 data_buffer[1];
	 data_buffer[0] = 0x01;

     Build_IDM_write_message(RBDS_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);


    if (IDMtransfer())
    {
		memcpy(buf, &idmdata[13], 8);
		return ((idmdata[4] - 0x0E)); // && (idmdata[4] == RBDS_OPCODE));
    }

	return 0;
#else
	buf[0] = FALSE;			//redundant - just to avoid warning at compile time
	return buf[0];
#endif
}

/******************************************************************
 *
 *      set_diag
 *
 *      Description:Provides daig services
 *
 *      Input:  const UINT8 * pData, UINT8 length
 *      Output: None
 *
 ******************************************************************/
void set_diag (const UINT8 * pData, UINT8 length)
{
#ifdef DRI352	
    UINT8 i;
    UINT8 data_buffer[20];

    for (i = 0; i < length; i++)
    {
        data_buffer[i] = *pData++;
    }

    Build_IDM_write_message(DIAGNOSTICS,
                            length + 13,
                            length,
                            &data_buffer[0]);


	IDMtransfer();
#endif	
}

void GenerateTestTone(UINT8 PortID,UINT32 Freq,UINT32 Amp)
{
#ifdef DRI352
	UINT8 data_buffer[10];

	data_buffer[0] = PortID;
	memcpy(&data_buffer[1],&Freq,4);
	memcpy(&data_buffer[5],&Amp,4);

	Build_IDM_write_message(DIAGNOSTICS,
	                            22,
	                            9,
	                            &data_buffer[0]);

	IDMtransfer();
#else
	PortID = FALSE;			//redundant - just to avoid warning at compile time
#endif
}
/******************************************************************
 *
 *      DiagnosticsEnable
 *
 *      Description: Enables/disables diagnostics based on input.
 *
 *      Input:  BOOL enable
 *      Output: BOOL result
 *
 ******************************************************************/
BOOL DiagnosticsEnable (BOOL enable)
{
#ifdef DRI352
    UINT8 data_buffer[1];

    data_buffer[0] = (enable == TRUE) ? 1 : 0;
    Build_IDM_write_message(DIAGNOSTICS,
                            14,
                            1,
                            &data_buffer[0]);

	return IDMtransfer();
#else
	enable = FALSE;			//redundant - just to avoid warning at compile time
	return enable;
#endif
}

/***********************************************************************************************
 *
 *      SplitAudio
 *
 *      Description: Split audio as Digital to left and Analog to right for current FM station
 *
 *      Input:  UINT8 FM=1, AM=0
 *      Output: BOOL result
 *
 ************************************************************************************************/
BOOL SplitAudio (UINT8 band)
{
#ifdef DRI352
    UINT8 data_buffer[2];

	data_buffer[0] = 2;			//2 - split digital left/ analog right on DAC1,
								//5 - split digital front DAC1/ analog rear DAC2
	data_buffer[1] = band;		//0 - AM
								//1 - FM
    Build_IDM_write_message(DIAGNOSTICS,
                            15,
                            2,
                            &data_buffer[0]);

	return IDMtransfer();
#else
	band = FALSE;		//redundant - just to avoid warning at compile time	
	return band;
#endif
}

#ifdef UNUSED_CODE	//Unused code
/*
BOOL FreqCalAM (void)
{
#ifdef DRI352
    UINT8 data_buffer[1];

    data_buffer[0] = 0x0A;
    Build_IDM_write_message(DIAGNOSTICS,
                            14,
                            1,
                            &data_buffer[0]);

	return IDMtransfer();
#else
	return FALSE;
#endif
}

BOOL FreqCalFM (void)
{
#ifdef DRI352
    UINT8 data_buffer[1];

    data_buffer[0] = 0x0B;
    Build_IDM_write_message(DIAGNOSTICS,
                            14,
                            1,
                            &data_buffer[0]);

	return IDMtransfer();
#else
return FALSE;
#endif
}

BOOL GetCalibratedFreqError (UINT32 * p)
{
#ifdef DRI352
    BOOL valid;
    UINT8 data_buffer[1];

    data_buffer[0] = 0x0C;
    Build_IDM_write_message(DIAGNOSTICS,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[13]);
        *p += (UINT32)(idmdata[14]) << 8;
        *p += (UINT32)(idmdata[15]) << 16;
        *p += (UINT32)(idmdata[16]) << 24;
    }

	return valid;
#else
	*p = FALSE;			//redundant - just to avoid warning at compile time	
	return FALSE;
#endif
}

BOOL SetCalibratedFreqError (UINT32 am, UINT32 fm)
{
#ifdef DRI352
    UINT8 data_buffer[8];

    // Store AM cal freq error.
    data_buffer[0] = am;
    data_buffer[1] = am >> 8;
    data_buffer[2] = am >> 16;
    data_buffer[3] = am >> 24;

    // Store FM cal freq error.
    data_buffer[4] = fm;
    data_buffer[5] = fm >> 8;
    data_buffer[6] = fm >> 16;
    data_buffer[7] = fm >> 24;

    // Write values to IDM.
    Build_IDM_write_message(0x68,	//Freq_Err_Input 0x68
                            21,
                            8,
                            &data_buffer[0]);

	return IDMtransfer();
#else
	return FALSE;
#endif
}

BOOL GetIFlevelFM (UINT32 * p)
{
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = FM_GET_STATUS;

    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[61]);
        *p += (UINT32)(idmdata[62]) << 8;
        *p += (UINT32)(idmdata[63]) << 16;
        *p += (UINT32)(idmdata[64]) << 24;
    }

    return valid;
}

BOOL GetLeftAudioLevelFM(UINT32 *p)
{
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = FM_GET_STATUS;

    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[65]);
        *p += (UINT32)(idmdata[66]) << 8;
        *p += (UINT32)(idmdata[67]) << 16;
        *p += (UINT32)(idmdata[68]) << 24;
    }

    return valid;

}

BOOL GetRightAudioLevelFM(UINT32 *p)
{
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = FM_GET_STATUS;

    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[69]);
        *p += (UINT32)(idmdata[70]) << 8;
        *p += (UINT32)(idmdata[71]) << 16;
        *p += (UINT32)(idmdata[72]) << 24;
    }

    return valid;

}

BOOL GetIFlevelAM (UINT32 * p)
{
#ifdef DRI352
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = AM_GET_STATUS;

    Build_IDM_write_message(AM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[45]);
        *p += (UINT32)(idmdata[46]) << 8;
        *p += (UINT32)(idmdata[47]) << 16;
        *p += (UINT32)(idmdata[48]) << 24;
    }

	return valid;
#else
	return FALSE;
#endif
}

BOOL GetLeftAudioLevelAM (UINT32 * p)
{
#ifdef DRI352
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = AM_GET_STATUS;

    Build_IDM_write_message(AM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[53]);
        *p += (UINT32)(idmdata[54]) << 8;
        *p += (UINT32)(idmdata[55]) << 16;
        *p += (UINT32)(idmdata[56]) << 24;
    }

	return valid;
#else
	return FALSE;
#endif
}

BOOL GetRightAudioLevelAM (UINT32 * p)
{
#ifdef DRI352
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = AM_GET_STATUS;

    Build_IDM_write_message(AM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[57]);
        *p += (UINT32)(idmdata[58]) << 8;
        *p += (UINT32)(idmdata[59]) << 16;
        *p += (UINT32)(idmdata[60]) << 24;
    }

	return valid;
#else
	return FALSE;
#endif
}
*/
#endif	//UNUSED_CODE

void GetSystemStatus(UINT8* pData, UINT16 length)
{
#ifdef DRI352
        UINT8 data_buffer[1];

    Build_IDM_write_message(GET_SYS_STATUS_OPCODE,
                            13,
                            0,
                            &data_buffer[0]);


    if(IDMtransfer())
    {
        memcpy(pData, &idmdata[0], length);
	}
#endif
}

void StationChangeStartFM (void)
{
    unsigned char data_buffer[5];

    data_buffer[0] = 0x0A;  // Station change
    data_buffer[1] = 0x01;  // Start
    data_buffer[2] = 0x00;
    data_buffer[3] = 0x00;
    data_buffer[4] = 0x00;


    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            18,
                            5,
                            &data_buffer[0]);


    IDMtransfer();
}

void StationChangeStartAM (void)
{
#ifdef DRI352
    unsigned char data_buffer[5];

    data_buffer[0] = 0x04;  // Station change
    data_buffer[1] = 0x01;  // Start
    data_buffer[2] = 0x00;
    data_buffer[3] = 0x00;
    data_buffer[4] = 0x00;


    Build_IDM_write_message(AM_DEMOD_CTRL_OPCODE,
                            18,
                            5,
                            &data_buffer[0]);


	IDMtransfer();
#endif
}

/******************************************************************
 *
 *      ChangeFMMode
 *
 *      Description: Enables/disables mono mode.
 *
 *      Input:  UINT8 mono_stereo, UINT8 dac
 *      Output: void
 *
 ******************************************************************/
void ChangeFMMode (UINT8 mono_stereo, UINT8 dac)
{
    unsigned char data_buffer[3];

    data_buffer[0] = MONO_MODE_FC;  // Mono Stereo Control
	data_buffer[1] = dac;			// DAC_0		0x00
									// DAC_1		0x01
									// DAC_2		0x02
	data_buffer[2] = mono_stereo;	// 0 = Mono off, default (= stereo mode).
									// 1 = Mono on


    Build_IDM_write_message(SYS_AUDIO_OPCODE,
                            16,
                            3,
                            &data_buffer[0]);

    IDMtransfer();
}

/*********************************************************************
*
*       GetTHD_N
*
*       Description: Get THD+N at line out, in dB
*       Input : None
*       Output: UINT8
*
*********************************************************************/
UINT8 GetTHD_N(void)
{
        return 40;
}

void SetPGAgain (UINT8 band)
{
	if (band == BAND_FM)
	{
		pga_gain(FM_PGA_GAIN);
	}
	else
	{
		pga_gain(AM_PGA_GAIN);
	}
}

static BOOL pga_gain (UINT8 gain)
{
#ifdef DRI352
	//1.00  000
	//1.14  100
	//1.33  010
	//1.60  110
	//2.00  001
	//2.67  101
	//4.00  011
	UINT8 data_buffer[1];
	data_buffer[0] = gain << 1;

	Build_IDM_write_message(PGA_GAIN_OPCODE,
							14,
							1,
							&data_buffer[0]);


	return IDMtransfer();
#else
	gain = FALSE;		//redundant - just to avoid warning at compile time	
	return FALSE;
#endif
}

/*********************************************************************
*
*       SetConfigParamFM
*
*       Description: Set FM configuration parameter
*       Input : UINT8, UINT32
*       Output: BOOL
*
*********************************************************************/

BOOL SetPreBWInterfMin (UINT32 ival)
{
	return SetConfigParamFM(0x35, ival);
}

BOOL SetPostBWInterfMin (UINT32 ival)
{
	return SetConfigParamFM(0x43, ival);
}

BOOL SetPostBWInterfMax (UINT32 ival)
{
	return SetConfigParamFM(0x44, ival);
}

BOOL SetSLInterfMax (UINT32 ival)
{
	return SetConfigParamFM(0x5A, ival);
}

//dabb
BOOL SetLeftRightScaling(UINT32 ival)
{
	return SetConfigParamFM(0x01, ival);
}

//dabb 081118 softmute
BOOL SetAudioScalingMin(UINT32 ival)
{
	return SetConfigParamFM(0x5D, ival);
}

//dabb 081118 softmute
BOOL SetAudioScalingMax(UINT32 ival)
{
	return SetConfigParamFM(0x5E, ival);
}

static BOOL SetConfigParamFM (UINT8 param, UINT32 ival)
{
	unsigned char data_buffer[5];

	data_buffer[0] = param;  // configuration parameter

	// Store 32 integer value
	data_buffer[1] = ival;
	data_buffer[2] = ival >> 8;
	data_buffer[3] = ival >> 16;
	data_buffer[4] = ival >> 24;

	Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
							18,
							5,
							&data_buffer[0]);


	return IDMtransfer();
}


void EnableRBDS (BOOL enable)
{
#ifdef DRI352
    unsigned char data_buffer[5];

    data_buffer[0] = 0x50;  // RBDS Enable configuration parameter
	if (enable)
		data_buffer[1] = 0x01;  // Enable RBDS processing
	else
		data_buffer[1] = 0x00;  // Disable RBDS processing
    data_buffer[2] = 0x00;
    data_buffer[3] = 0x00;
    data_buffer[4] = 0x00;


    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            18,
                            5,
                            &data_buffer[0]);

	IDMtransfer();
#endif
}

BOOL rbds_flush(void)
{
#ifdef DRI352
	UINT8 data_buffer[1];
	data_buffer[0] = 0x02;

	Build_IDM_write_message(RBDS_OPCODE,
					14,
					1,
					&data_buffer[0]);


	if (IDMtransfer())
	{
		return 1;
	}

	return 0;
#else
	return FALSE;
#endif
}

void EnableBER(UINT8 bval)
{
#ifdef DRI352	
	UINT8 data_buffer;

	data_buffer = bval;

	Build_IDM_write_message(BER_OPCODE,
							14,
							1,
							&data_buffer);


	IDMtransfer();
#endif
}

BOOL ResetErrorCounters(void)
{
#ifdef DRI352
	UINT8 data_buffer;

	data_buffer = 0x01;

	Build_IDM_write_message(RST_ERR_OPCODE,
					14,
					1,
					&data_buffer);


	if (IDMtransfer())
	{
		return 1;
	}
#endif
	return 0;
}

/******************************************************************
 *
 *      CA_GetRadioID
 *
 *      Description: Reads the unique RadioID of
 *					 the conditional access chip
 *
 *      Input:  UINT8* pointer to 4-byte RadioID array
 *      Output: UINT8 Success/failure status
 *
 ******************************************************************/
UINT8 CA_GetRadioID(UINT8* id)
{

	UINT8 data_buffer[1];

	// Function code to get Radio ID from CA chip
	data_buffer[0] = CA_RADIO_ID_FC;

	Build_IDM_write_message(CA_OPCODE,
							14,
							1,
							&data_buffer[0]);

	if (IDMtransfer())
	{
		if (idmdata[13] == CA_FUNC_SUCCESS)
		{
			id[3] =	idmdata[14];
			id[2] =	idmdata[15];
			id[1] =	idmdata[16];
			id[0] =	idmdata[17];
			return 1;
		}
	}

	return 0;
}

/******************************************************************
 *
 *      CA_GetServiceMsg
 *
 *      Description: Get the conditional access service message for
 *					 a given audio program
 *
 *      Input:  UINT8 Program number (0-7)
 *      Output: UINT8* pointer to service message data, first byte is
 *						length
 *
 ******************************************************************/
UINT8* CA_GetServiceMsg(UINT8 program)
{

	UINT8 data_buffer[3];
	UINT8 length;

	// Function code to get CA service message
	data_buffer[0] = CA_SVC_MSG_FC;
	data_buffer[1] = 0;	// hard-code for audio programs for now
	data_buffer[2] = program;

	Build_IDM_write_message(CA_OPCODE,
							16,
							3,
							&data_buffer[0]);

	if (IDMtransfer())
	{
		if (idmdata[13] == CA_FUNC_SUCCESS)
		{
			length = idmdata[14];
			idmdata[15] += 1;		//Add 1 to the encoding byte always. If not added, when 0, it accidently terminates the string
			idmdata[15+length] = 0;	//NULL terminate string;
			return &idmdata[14];
		}
	}

	return 0;
}

/******************************************************************
 *
 *      CA_GetInfo
 *
 *      Description: Get the conditional access information for
 *					 a given audio program
 *
 *      Input:  UINT8 Program number (0-7)
 *      Output: UINT8* pointer to conditional access information
 *
 ******************************************************************/
UINT8* CA_GetInfo(UINT8 program)
{

	UINT8 data_buffer[4];

	// Function code to get CA service message
	data_buffer[0] = CA_INFO_FC;
	data_buffer[1] = 0;		// hard-code for audio programs for now
	data_buffer[2] = 0xFF;	// get info for indicated program, not all programs
	data_buffer[3] = program;

	Build_IDM_write_message(CA_OPCODE,
							17,
							4,
							&data_buffer[0]);

	if (IDMtransfer())
	{
		if (idmdata[13] == CA_FUNC_SUCCESS)
		{
			return &idmdata[15];
		}
	}

	return 0;
}

void Tune_Select(BAND band, UINT16 freq, UINT8 multicast)
{
	UINT8 data_buffer[5];

	data_buffer[0] = TUNE_SELECT;
	data_buffer[1] = band;

	if(band == BAND_FM)
		freq = freq * 10;

	data_buffer[2] = freq & 0xFF;
	data_buffer[3] = (freq>>8) & 0xFF;
	data_buffer[4] = multicast;


	Build_IDM_write_message(SYS_TUNE_OPCODE,
							18,
							5,
							&data_buffer[0]);

	IDMtransfer();

}

void Tune_Step(UINT8 stepMode, UINT8 dir, BOOL bMulticastScan)
{
	UINT8 data_buffer[4];

	data_buffer[0] = TUNE_STEP;
	data_buffer[1] = stepMode;
	data_buffer[2] = dir;
	data_buffer[3] = bMulticastScan;


	Build_IDM_write_message(SYS_TUNE_OPCODE,
							17,
							4,
							&data_buffer[0]);

	IDMtransfer();
}

void Tune_GetStatus(UINT8 type)
{
	UINT8 data_buffer[2];
	data_buffer[0] = TUNE_GET_STATUS;
	data_buffer[1] = type;

	Build_IDM_write_message(SYS_TUNE_OPCODE,
							15,
							2,
							&data_buffer[0]);

	if(IDMtransfer())
	{
#ifdef OTHER_DEFINE	
		SaveRadioStatus(&idmdata[12]);
#endif
	}
}

/******************************************************************
 *
 *      Sys_Audio_Control
 *
 *      Description:Provides access to audio properties
 *
 *      Input:  UINT8 * pData, UINT8 length
 *      Output: None
 *
 ******************************************************************/
void Sys_Audio_Control (UINT8 * pData, UINT8 length)
{
	UINT8 i;
	UINT8 data_buffer[20];

	for (i = 0; i < length; i++)
	{
		data_buffer[i] = *pData++;
	}

	Build_IDM_write_message(SYS_AUDIO_OPCODE,
							length + 13,
							length,
							&data_buffer[0]);


	IDMtransfer();
}

/******************************************************************
 *
 *      DAC_Service_Select
 *
 *      Description: Select Audio Service for a given DAC
 *
 *      Input:  UINT8 * pData, UINT8 length
 *      Output: None
 *
 ******************************************************************/
void DAC_Service_Select (UINT8 route_dac, UINT8 service_id)
{
	UINT8 data_buffer[3];
	
	data_buffer[0] = DAC_SERVICE_SELECT_FC;
	data_buffer[1] = route_dac;				//0 - DAC1, 1 - DAC2, 2 - DAC3
	data_buffer[2] = service_id;			//0 - Reserved
											//1 - Silence
											//2 - IBOC 
											//3 - Analog
											//4 - HD
											//5 - AUX 1
	

	Build_IDM_write_message(SYS_AUDIO_OPCODE,
							16,
							3,
							&data_buffer[0]);

	IDMtransfer();
}

BOOL GetCurrentConfig(void)
{
#ifdef DRI352
	UINT8 data_buffer[1];

	data_buffer[0] = 0;
	Build_IDM_write_message(GET_CNFG_OPCODE,
                            14,
                            1, 
                            &data_buffer[0]);

	return IDMtransfer();
#else
	return FALSE;
#endif
}

BOOL SetCurrentConfig(UINT8 *cfgData, UINT8 len)
{
#ifdef DRI352
 	//BOOL valid;
	UINT8 data_buffer[132];

	data_buffer[0] = 1;
	if(len>131) len = 131;
	memcpy(&data_buffer[1], cfgData, len);

	Build_IDM_write_message(SET_CNFG_OPCODE,
                            144,
                            132,
                            &data_buffer[0]);

	return IDMtransfer();
#else
	cfgData = &len;		//redundant - just to avoid warning at compile time
	return FALSE;
#endif
}

void Set_ExPacketSize(UINT32 size)
{
	UINT8* pData = &idmdata[12];
	if(GetCurrentConfig())
	{
	 	*(pData+120) = (size>>24)&0xFF;
		*(pData+121) = (size>>16)&0xFF;
		*(pData+122) = (size>>8)&0xFF;
		*(pData+123) = (size)&0xFF;
		SetCurrentConfig(pData, 131);
	}
}

#if defined(DRI352) && defined(READ_SLAVE_SDK_VERSION)
/******************************************************************
 *
 *      ReadSlaveSDKVersion
 *
 *      Description:Reads Slave Micro Version
 *
 *      Input:  UINT8 * p, UINT8 len
 *      Output: None
 *
 ******************************************************************/
void ReadSlaveSDKVersion(UINT8 * p, UINT8 len)
{
	UINT8 data_buffer[3];
	UINT8 *pIDM = &idmdata[15];
	UINT8 i;

	data_buffer[0] = 0x01;					// Vendor ID = iBiquity Digital
	data_buffer[1] = 0x01;					// Product ID = 1181-1281
	data_buffer[2] = GET_SLAVE_SDK_VERSION; // Function Code = Read Slave SDK Version

	Build_IDM_write_message(VENDOR_OPCODE,
							16,
							3,
							&data_buffer[0]);

	if (IDMtransfer())
	{
		for (i = 0; i < (len - 1) && *pIDM != 0; i++)
		{
			*p = *pIDM;
			p++;
			pIDM++;
		}
		*p = '\0';
	}
}

/******************************************************************
 *
 *      FloatSlaveMicro
 *
 *      Description:Floats the slave micro I2C & SPI buses
 *
 *      Input:  None
 *      Output: None
 *
 ******************************************************************/
void FloatSlaveMicro(void)
{
	UINT8 data_buffer[3];
	UINT8 i;

	data_buffer[0] = 0x01;					// Vendor ID = iBiquity Digital
	data_buffer[1] = 0x01;					// Product ID = 1181-1281
	data_buffer[2] = FLOAT_SLAVE_I2C_BUS;	// Function Code = Float Slave I2C, To enable CDM use

	Build_IDM_write_message(VENDOR_OPCODE,
							16,
							3,
							&data_buffer[0]);

	IDMtransfer();
}


#endif //READ_SLAVE_SDK_VERSION

/******************************************************************
 *
 *      force_analog
 *
 *      Description: Force Audio to Analog, or auto-blend
 *
 *      Input:  UINT8 blend
 *      Output: None
 *
 ******************************************************************/
void force_analog (UINT8 blend)
{
#ifdef DRI352
	UINT8 data_buffer[1];

	data_buffer[0] = blend;


	Build_IDM_write_message(BLND_OVR_OPCODE,
							14,
							1,
							&data_buffer[0]);


	IDMtransfer();
#endif
}

void get_sys_status(void)
{
    UINT8 data_buffer[1];

    data_buffer[0] = 0;
    
    Build_IDM_write_message(SYS_STATUS_OPCODE,
                            10,
                            0,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
        #ifdef OTHER_DEFINE
        AcqStatusSave(idmdata);
	#endif	
    }
}


BOOL GetIFlevelFM (UINT32 * p)
{
#ifdef DRI352
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = FM_GET_STATUS;

    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[61]);
        *p += (UINT32)(idmdata[62]) << 8;
        *p += (UINT32)(idmdata[63]) << 16;
        *p += (UINT32)(idmdata[64]) << 24;
    }

    return valid;
#else
	*p = 0x0f+1;
	return TRUE;
#endif
}


BOOL GetIFlevelAM (UINT32 * p)
{
#ifdef DRI352
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = AM_GET_STATUS;

    Build_IDM_write_message(AM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();
    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[45]);
        *p += (UINT32)(idmdata[46]) << 8;
        *p += (UINT32)(idmdata[47]) << 16;
        *p += (UINT32)(idmdata[48]) << 24;
    }

    return valid;
#else
	*p = 0xdf30+1;
	return TRUE;
#endif
}


#ifndef _TUNER_ATMEL
/******************************************************************
 *
 *      dri350_tune
 *
 *      Description:Tunes the TDGA2 through the secondary IIC
 *
 *      Input:  UINT8 band, UINT16 freq
 *      Output: None
 *
 ******************************************************************/
void dri350_tune (UINT8 band, UINT16 freq)
{
#ifdef DRI352
    UINT8 data_buffer[10];
    UINT8 *p_tuner_data;
    UINT8 i;

    data_buffer[0] = 0;                     //Function Code-->Send Tuner Command
    data_buffer[1] = 0x60;  //0x62;          //TDGA2 slave Address?
    data_buffer[2] = 6;                     //Data to send to tuner
    data_buffer[3] = 0;                     //??????????

    p_tuner_data = tdga2tune(band, freq); //Get the tuner Data

#ifdef IDM_TUNE

    for (i = 4 ; i<= 9; i++) //Fill the data buffer with the tuner data
    {
        data_buffer[i] = *(p_tuner_data + (i-4));
    }

    for (i = 0; i<=1; i++) //Write the tuner byte twice
    {
        if (i==0)
        {
            data_buffer[6] |= 0x80; //unlock the PLL
        }
        else
        {
            data_buffer[6] &= 0x7f; //Lock the PLL
        }

        Build_IDM_write_message(DDICE_TUNE_OPCODE,
                                20, //Length of IDM command packet
                                10, //Data Buffer Length
                                &data_buffer[0]);//Data buffer Address

        IDMtransfer();
    }
#endif //IDM_TUNE
#endif
}
#endif


void idm_set_FM_left_right_scaling (DIRECTION dir)
{
if(dir == UP)
	g_nFMLeftRightScaling += 0x21;
else
	g_nFMLeftRightScaling -= 0x21;

	SetLeftRightScaling(g_nFMLeftRightScaling);
}


void idm_get_FM_left_right_scaling (void)
{
	UINT8 data_buffer[2];

	data_buffer[0] = GET_ANALOG_FM_CNFG;
	data_buffer[1] = 0;

	Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
							15,
							2,
							&data_buffer[0]);

	if (IDMtransfer())
	{
		// 4bytes
		g_nFMLeftRightScaling = idmdata[0x12+0x04];
		g_nFMLeftRightScaling += idmdata[0x12+0x05] << 8;
		g_nFMLeftRightScaling += idmdata[0x12+0x06] << 16;
		g_nFMLeftRightScaling += idmdata[0x12+0x07] << 24;
	}
}


BOOL GetRFlevelFM (UINT32 * p)
{
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = FM_GET_STATUS;

    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();

    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[13+0x48]);
        *p += (UINT32)(idmdata[13+0x49]) << 8;
        *p += (UINT32)(idmdata[13+0x4A]) << 16;
        *p += (UINT32)(idmdata[13+0x4B]) << 24;
    }

    return valid;
}


BOOL GetRFlevelAM (UINT32 * p)
{
    BOOL valid;
    unsigned char data_buffer[1];

    data_buffer[0] = AM_GET_STATUS;

    Build_IDM_write_message(AM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    valid = IDMtransfer();

    if (valid)
    {
        *p = 0l;
        *p += (UINT32)(idmdata[13+0x18]);
        *p += (UINT32)(idmdata[13+0x19]) << 8;
        *p += (UINT32)(idmdata[13+0x1A]) << 16;
        *p += (UINT32)(idmdata[13+0x1B]) << 24;
    }

    return valid;
}
#endif

