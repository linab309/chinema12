/* @file 		Tuner.c
 * @brief 		HD_Radio Driver File
 *
 * @author 	jian
 * @date 	  	2011-08-15
 * @version 	1.0
 * @note         该HD_Radio 模块的调试就是按照该模块的串口协议
                     把相应的串口命令发送给该模块就可以实现
                     相应的功能。(只要串口初始化好后,把相应的
                     命令发送给该模块就可以控制这个模块了)
 */

#include "..\driver\uart.h"
#include "..\main\Macro.h"
#include "..\main\vfd_disp.h"
#include "..\main\Radio_task.h"
#include "..\main\queue.h"
#include "..\main\Debug.h"
#include "..\main\queue.h"
#include "Tuner.h"

#ifdef _DEBUG  
#define _DEBUG_TUNER 
#endif


//BANDS
typedef enum
{
    BAND_AM,
    BAND_FM,
    BANDS
} BAND;


uchar debug_flag = 0;
#define CMD_BUFFER_SIZE 256 //1+9+245+1
//uchar psd_vaild;//jian add in 11-08-18
uint8 currBand = BAND_FM;   // jian  add in 11-08-17
extern pSTRUCT_QUEUE tuner_queue_pointer;
extern uchar da_status ;
#define MP1	0x01
#define MP2	0x02
#define MP3	0x03
#define MP5	0x05
#define MP6	0x06

#define MA1	0x01
#define MA3	0x03
//#define CMD_BUFFER_SIZE 30
uchar idmdata[CMD_BUFFER_SIZE];


/***************
2                        1               1                    2                 1                  1                                    2
HEADER
0XA5 0XA5     lm count     instance     LM Length     Opcode     Abort/Status       Number of Data Segments


              2                           0 -2035              1
Data Segment Number                              check sum
    

******/
extern void DelayMs(int delay_time);

/*
* @brief This command is used to set monitoring or request the SIS and PSD data.
*
* @author jian
* @data    201-08-15
* @Para :
*	      SIS_Flag=1--可以返回HD Radio 中CMD_IND_SIS_CHANGE 信息包
*	      PSD_Flag=1--可以返回HD Radio 中CMD_IND_PSD_CHANGE 信息包
* @note:
*           对于checksum 的计算,Datasheet有说明。
*/
#define IDM_SERIAL_COMMUNICATION


#ifdef IDM_SERIAL_COMMUNICATION

#define IDM_HDR                 0xA5
#define MIN_LM_LEN              13      // 13 byte overhead with no data
#define MIN_LM_BYTES_TO_PARSE   7
#define SERIAL_TIMEOUT			15000

typedef enum
{
	hdr_1_search,
	hdr_2_search,
	seq_search,
	instance_search,
	lm_lsb_len_search,
	lm_msb_len_search,
	opcode_search,
	parse_complete
} PARSE_STATE;

BOOL IdmSerialWrite(uint8 WriteLen, uint8 *WriteBuffer)
{
 	uint8 idx;
//	SCI_buffer_wr(&sci2, WriteBuffer, WriteLen);
    // debug_printf("radio commad :");
        for(idx = 0;idx<WriteLen;idx++)
        	{
    
		        Uart1Send(WriteBuffer[idx]);
	 	//       debug_printf("%x ",WriteBuffer[idx]);
		}
//       debug_printf("\n");
	return TRUE;
}

/*********************************************************************************/
/** function: IdmSerialRead                                                      **/
/** description: read a logical message from the IDM using UART                 **/
/** input: [IN]uint8 opcode - LM parameter                                      **/
/**        [IN]INT32 buffsize - buffer size of *buffer                          **/
/**        [IN,OUT]uint8* buffer - pointer to memory used for returned UART		**/
/**								   data											**/
/** returns: 1-success,0-error                                                  **/
/*********************************************************************************/

BOOL IdmSerialRead (uint8 opcode, uint16 buffsize, uint8 * buffer)
{
	int buffidx,bytes_read;
	int lm_byte_length;
	uint8 msb,lsb;
	uint8 currbyte;
	PARSE_STATE parse_state;
	BOOL valid;
	uint16 size = 1;
	uint16 timeout = 0;
	
	bytes_read = 0;
	buffidx = 0;
	lm_byte_length = 0;
	lsb = 0;
	msb = 0;
	valid = FALSE;
	
	parse_state = hdr_1_search;
	if(debug_flag == 1)
		debug_printf("Read: \n");
	do
	{
		timeout = 0;

		/* See if there is a byte in the RX buffer */
		while((IsQueueEmpty(tuner_queue_pointer) == QUEUE_EMPTY) && timeout < SERIAL_TIMEOUT)			//timeout empirically derived	/*队列有数据*/
		{
			//MOD_ResetWDT();
			timeout++;
		}

		if(timeout >= SERIAL_TIMEOUT)
		{
			/* If you are here often, there is some serious error with UART */
			return FALSE;
		}
		
		//currbyte = SCI_getch(&sci2);
		DeQueue(tuner_queue_pointer,&currbyte);
if(debug_flag == 1)
		debug_printf("%x ",currbyte);
		
		switch (parse_state)
		{
			case hdr_1_search:
				if (currbyte == IDM_HDR)
				{
					buffidx = 0;
					/** start parsing **/
					buffer[buffidx] = currbyte;
					buffidx++;
					parse_state = hdr_2_search;
				}
				break;

			case hdr_2_search:
				if (currbyte == IDM_HDR)
				{
					buffer[buffidx] = currbyte;
					buffidx++;
					parse_state = seq_search;
				}
				else
				{
					parse_state = parse_complete;
				}
				break;

			case seq_search:
				buffer[buffidx] = currbyte;
				buffidx++;
				parse_state = instance_search;
				break;

			case instance_search:
				buffer[buffidx] = currbyte;;
				buffidx++;
				if (currbyte == 1)
				{
					parse_state = lm_lsb_len_search;
				}
				else
				{
					parse_state = parse_complete;
				}
				break;

			case lm_lsb_len_search:
				buffer[buffidx] = currbyte;
				buffidx++;
				lsb = currbyte;
				parse_state = lm_msb_len_search;
				break;

			case lm_msb_len_search:
				buffer[buffidx] = currbyte;
				buffidx++;
				msb = currbyte;
				lm_byte_length = (msb << 8) & 0x0000ff00;
				lm_byte_length |= lsb;
				if (lm_byte_length < MIN_LM_LEN)
				{
					parse_state = parse_complete;
				}
				else
				{
					parse_state = opcode_search;
				}
				/* Make sure the packet isn't too big */
				if (lm_byte_length > CMD_BUFFER_SIZE)
					lm_byte_length = CMD_BUFFER_SIZE;

				break;

			case opcode_search:
				if (currbyte == opcode)
				{
					buffer[buffidx] = currbyte;
					buffidx++;
					valid = TRUE;
				}
				parse_state = parse_complete;
				break;

			case parse_complete:
				break;

		} //end switch

		bytes_read++;

	} while ((bytes_read < (buffsize - MIN_LM_LEN)) && (parse_state != parse_complete));

#if 1
	if (valid)
	{
		/* Got here if we received the first 7 bytes of a logical message  */
		/* Now read the rest of the packet */
		timeout = 0;
		size = lm_byte_length - MIN_LM_BYTES_TO_PARSE;
		bytes_read = 0;
		
		do
		{
			/* See if there is a byte in the RX buffer */
		while((IsQueueEmpty(tuner_queue_pointer) == QUEUE_EMPTY) && timeout < SERIAL_TIMEOUT)			//timeout empirically derived	/*队列有数据*/
			{
				timeout++;
			}		

			if(timeout >= SERIAL_TIMEOUT)
			{
				/* If here, you didn't wait long enough for the complete packet to arrive */
				/* This may indicate an issue with UART itself */
				return FALSE;
			}

			DeQueue(tuner_queue_pointer,&currbyte);
if(debug_flag == 1)			
			debug_printf("%x " ,currbyte);
			buffer[buffidx] = currbyte;
			buffidx++;
			bytes_read++;
			
		} while(bytes_read < size);
	}
if(debug_flag == 1)			
		debug_printf("\n");

debug_flag  = 0;

	return valid;	
#else
	if (valid)
	{
		/* Got here if we received the first 7 bytes of a logical message  */
		/* Now read the rest of the packet */
		size = lm_byte_length - MIN_LM_BYTES_TO_PARSE;
		
		/* See if there is a byte in the RX buffer */
		while((SCI_rcvbuf_length(&sci2) < size) && timeout < SERIAL_TIMEOUT)
		{
			MOD_ResetWDT();
			//ServiceKeyPress();
			//ServiceRotaryEncoder();
			timeout++;
		}

		if(timeout >= SERIAL_TIMEOUT)
		{
			/* If here, you didn't wait long enough for the complete packet to arrive */
			/* This may indicate an issue with UART itself */
			SCI_flush(&sci2);
			return FALSE;
		}

		SCI_buffer_rd(&sci2, &buffer[buffidx], &size);
	}
	return valid;
#endif
}

/******************************************************************
 *
 *      CalcCheckSum
 *
 *      Description:Calculates the checksum for the Build_IDM_write_message Function.
 *
 *      Input:  int16 BuffLen,uint8 *Buffer
 *      Output: uint8
 *
 ******************************************************************/
uint8  CalcCheckSum(int16 BuffLen,
                            uint8 *Buffer)
{
    int16 i;
    uint8 ChkSum;
    int16 NumElements;
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
 *      Build_IDM_write_message
 *
 *      Description:Builds the IDM's command packet
 *
 *      Input:  uint8 opcode,
 *              uint8 msg_length,
 *              uint8 data_length,
 *              uint8 *cmd_data
 *      Output: None
 *
 ******************************************************************/

void Build_IDM_write_message (uint8 opcode, //opcode for the command
                              uint8 msg_length, //length of the completed packet.
                              uint8 data_length, //length of the cmd_data buffer
                              uint8 *cmd_data) //command specific data
{
    static uint8 LM_count = 0;
    int16 i;
    idmdata[0] = 0xA5;
    idmdata[1] = 0xA5;
    idmdata[2] = LM_count;
    if (LM_count == 9)
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

#if 1//def IDM_SERIAL_COMMUNICATION
	//Timer_UsDelay(150);
    valid = IdmSerialWrite(idmdata[4], &idmdata[0]);
//    DelayMs(10);
    if (valid)
	{
		valid = IdmSerialRead (idmdata[6], sizeof(idmdata), &idmdata[0]);
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


#endif // IDM_SERIAL_COMMUNICATION


#if 0
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
    uint16 numberOfSegments;
    uint16 segmentNumber;
    uint8 SessionType;
    uint8 data_buffer[2];
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
		idmdata[11] = (uint8)((segmentNumber >> 8) & 0xFF);
		idmdata[10] = (uint8)(segmentNumber & 0xFF);
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
        segmentNumber    = ((uint16)idmdata[11] << 8) + (uint16)idmdata[10];
        numberOfSegments = ((uint16)idmdata[9] << 8) + (uint16)idmdata[8];
        segmentNumber++;

		SessionType = idmdata[17];
#ifdef ENABLE_STORE_N_PLAY
		if (SessionType == GetCurrProgram() || SP_isPlaying())
#else
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
		psd_vaild = 1;
	else
		psd_vaild = 0;
}
#endif
void Tune_Select(BAND band, uint16 freq, uint8 multicast)
{
	uint8 data_buffer[5];

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

void Tune_Step(uint8 stepMode, uint8 dir, BOOL bMulticastScan)
{
	uint8 data_buffer[4];

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



void Tune_GetStatus(uint8 type)
{
	uint8 data_buffer[2];
	data_buffer[0] = TUNE_GET_STATUS;
	data_buffer[1] = type;

	Build_IDM_write_message(SYS_TUNE_OPCODE,
							15,
							2,
							&data_buffer[0]);


//	debug_flag  =1;
	if(IDMtransfer())
	{
		SaveRadioStatus(&idmdata[12]);
	}
//	debug_flag  = 0;
}


/******************************************************************
 *
 *      Sys_Audio_Control
 *
 *      Description:Provides access to audio properties
 *
 *      Input:  uint8 * pData, uint8 length
 *      Output: None
 *
 ******************************************************************/
void Sys_Audio_Control (uint8 * pData, uint8 length)
{
	uint8 i;
	uint8 data_buffer[2];

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




/*
* @brief This command is used to tune up frequency
*
* @author chenjianming
* @data  2008-3-31
* @para 
*            False: Tune to the next frequency
*                      (FM-200KHZ step  AM-10KHz step)
*            True: Tune to the next available sub channel
*                     If none ,tune to the next frequency
*/
void Tuner_Req_Tune_Up(unsigned char mode)
{

	Tune_Step(0x01, 0x01,  1);
}

/*
* @brief This command is used to tune down frequency
*
* @author chenjianming
* @data  2008-3-31
* @para 
*            False: Tune to the previous frequency
*                      (FM-200KHZ step  AM-10KHz step)
*            True: Tune to the previous available sub channel
*                     If none ,tune to the previous frequency
*/
void Tuner_Req_Tune_Down(unsigned char mode)
{
	Tune_Step(0x01, 0x00, 1);
}


/*
* @brief This command is used to seek up or down
*
* @author chenjianming
* @data  2008-3-31
*/
#define fm_seek_threshold  4623//1600l
#define  am_seek_threshold  50l
#define nFM_SEEK_DELAY 160

extern uint32 currSeekLevel ;

BOOL Radio_IsTuned(void)
{

	if((current_radio_mode==_FM_MODE)||(current_radio_mode==_HD_FM_MODE))
		{
			currBand = BAND_FM;	
		}
	else 
		{
			currBand = BAND_AM;
		}	
	
	if (currBand == BAND_FM)
	{
		if (currSeekLevel < fm_seek_threshold)
		{

			return TRUE;
		}
	}
	else
	{
		if (currSeekLevel > am_seek_threshold)
		{	

			return TRUE;
		}
	}
	
	return FALSE;
}



 uchar Tuner_Seek_Command(unsigned char direction)
{

	BOOL done, isTuned;
	uint8 seek_delay;

	done = FALSE;
	if((current_radio_mode==_FM_MODE)||(current_radio_mode==_HD_FM_MODE))
		{
			currBand = BAND_FM;	
		}
	else 
		{
			currBand = BAND_AM;
		}		
	
	if (currBand == BAND_FM)
		seek_delay = nFM_SEEK_DELAY;//FM_SEEK_DELAY;
	else
		seek_delay = AM_SEEK_DELAY;

	isTuned = Radio_IsTuned();


	Tune_Step(1, direction, isTuned);

	DelayMs(seek_delay);

	Tune_GetStatus(GET_STATUS_ANALOG_ONLY);

	if(Radio_IsTuned())
		done = TRUE;

	return done;
}


#define NO_DIGITAL				0x00
#define HD_ACQUIRED				0x01
#define SIS_ACQUIRED				0x02
#define DIGITAL_AUDIO_ACQUIRED	0x04

//11

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
        AcqStatusSave(&idmdata[12]);
    }
}


void Radio_GetStatus(void)
{
	//MOD_TimerSleep(100);
	Tune_GetStatus(GET_STATUS_ALL);		
	/* Get audio programs available and CA programs */
	get_acq_status();

//	idm_get_FM_status ();	
	
	 if(da_status & SIS_ACQUIRED)	
	 	{
	 	debug_printf("get_psd\n");
	//	get_psd();
	 	}
	 else if(da_status & DIGITAL_AUDIO_ACQUIRED)
	 	{
	 	debug_printf("DIGITAL_AUDIO_ACQUIRED\n");
	 	}
	else if(da_status & HD_ACQUIRED)
		{
		debug_printf("HD_ACQUIRED\n");
		}	 
	else
		{
		if (currBand == BAND_FM)
		{
			idm_get_FM_status();
			RBDS_Decode();
		}
		else
		{
		//	idm_get_AM_status();
		}

//		debug_printf("Radio_GetStatus\n");
		}
}

/*
*/

BOOL IsSISAcquired(void)
{
	return (da_status & SIS_ACQUIRED);
}
/**/
/*
* @brief This command is used to seek frequency with HD Radio signal
*
* @author chenjianming
* @data  2008-3-31
*/
uchar Tuner_HDSEEK_Command(unsigned char direction)
{
	BOOL done, isTuned;
	uint8 seek_delay;
	uint8 loop_delay;
	uint8 lcv;
	uint8 i;
	
	done = FALSE;
	if((current_radio_mode==_FM_MODE)||(current_radio_mode==_HD_FM_MODE))
		{
			currBand = BAND_FM;	
		}
	else 
		{
			currBand = BAND_AM;
		}	
	if (currBand == BAND_FM)
	{
		seek_delay = nFM_SEEK_DELAY;
		lcv = 20;
		loop_delay = FM_LOOP_DELAY;
	}
	else
	{
		seek_delay = AM_SEEK_DELAY;
		lcv = 65;
		loop_delay = AM_LOOP_DELAY;
	}

	isTuned = 1;//Radio_IsTuned();


	Tune_Step(1, direction, isTuned);

	DelayMs(seek_delay);

	Tune_GetStatus(GET_STATUS_DIGITAL_ONLY);

	if (Radio_IsTuned())
	{
		for (i = 0 ; i < lcv; i++)
		{
			DelayMs(loop_delay);
			Radio_GetStatus();
			if(IsSISAcquired())
			{
				done = TRUE;
				break;
			}
		}
	}


	return done;;
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

    uint8 data_buffer[1];

    data_buffer[0] = FM_GET_STATUS;

    Build_IDM_write_message(FM_DEMOD_CTRL_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);
//	debug_flag  =1;
    if (IDMtransfer())
    {
 //       SetStereoStatus(idmdata[13]);
    }
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

BOOL get_rbds_group(uint8* buf)
{
#if 1//def DRI352
	 uint8 data_buffer[1];
	 data_buffer[0] = 0x01;

     Build_IDM_write_message(RBDS_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

	//debug_flag  =1;
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
 *      idm_set_FM_stereo_mode
 *
 *      Description: Set FM stereo mode
 *
 *      Input:  mode
 *      Output: None
 *
 ******************************************************************/
void idm_set_FM_stereo_mode (uint8 mode)
{
	uint8 data_buffer[5];
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
		idm_get_FM_status();
	}
}

/*
* @brief mute control command
*
* @author chenjianming
* @data    2008-3-31
*/
void Tuner_Mute_Command(unsigned char mute)
{
	if(mute == Tune_Unmute)
		idm_set_FM_stereo_mode(0x03);
	else	 
		idm_set_FM_stereo_mode(0x00);
}

/*
* @brief Volume Change command
*
* @author chenjianming
* @data    2008-3-31
* @note 初始化为0DB
*/
void Tuner_Volume_Init(void)
{

	uint8 buf[3];
	buf[0] = VOLUME_CONTROL_FC;
	buf[1] = ROUTE_DAC_0;
	buf[2] = 0x80;
	Sys_Audio_Control(&buf[0],3);



}

/*
* @brief This command is used to setting tuner frequency
*
* @author chenjianming
* @data    2008-3-31
*/
void Tuner_Write_Freq_Command(unsigned char mode,unsigned int Tuner_Freq)
{
unsigned long Freq;
	
	if((mode==_FM_MODE)||(mode==_HD_FM_MODE))
		{
		Freq=Tuner_Freq/100;	
	        Tune_Select(BAND_FM, Freq, 0);
		}
	else 
		{
		Freq=Tuner_Freq;	
		debug_printf("Freq :%d\n",Tuner_Freq);
	        Tune_Select(BAND_AM, Freq, 0);
		}

}

/*  
* @brief HD Radio Init
*
* @author chenjianming
* @data    2008-3-31
*/
extern unsigned char Tuner_Signal_Quality;
extern unsigned int Tuner_Freq_H;
extern unsigned int Tuner_Freq_L;
void InitTuner(unsigned char mode)
{
#ifdef _DEBUG_TUNER
	//debug_printf("\nInit Tuner");
#endif
	Tuner_Write_Freq_Command(mode,system_freq);
}



/******************************************************************
 *
 *      select_program
 *
 *      Description: selects multicast program.
 *
 *      Input:  uint8 program
 *      Output: None
 *
 ******************************************************************/
void select_program(uint8 program)
{
    uint8 data_buffer[2];

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
    uint8 data_buffer[1];

    //to do --check what data services are enabled
    data_buffer[0] = 0x05; //port ID must be 5 for basic SIS


    Build_IDM_write_message(GET_SIS_DATA_OPCODE,
                            14,
                            1,
                            &data_buffer[0]);

    if (IDMtransfer())
    {
     //   SISbasicSave(&idmdata[12]);
    }
}









/*****************************_*************************************
 *
 *      FlushPSD
 *
 *      Description: Flush PSD Queue for given program
 *
 *      Input:  uint8 program
 *      Output: None
 *
 ******************************************************************/
void FlushPSD (uint8 program)
{
    uint8 data_buffer[3];

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
	uint8 data_buffer[2];

	data_buffer[0] = 0x04;	// port ID must be 4
	data_buffer[1] = 0x00;

	Build_IDM_write_message(FLUSH_PSD_QUEUE_OPCODE,
							13,
							0,
							&data_buffer[0]);

	IDMtransfer();

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
	uint8 data_buffer[1];

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
    uint8 data_buffer[1];
    uint8 msglength = 0;

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
        	//SIS_StationMessageSave(&idmdata[13], msglength);
		}
    }
}

#if 0

/******************************************************************
 *
 *      idm_get_surround_mode
 *
 *      Description: Get HD surround mode of a particular program
 *
 *      Input:  uint8 program number (0-7)
 *      Output: uint8 Surround mode:
 *								0x00	None
 *								0x01	SRS Circle Surround
 *								0x02	Dolby ProLogic2 Surround
 *								0x03	Neural Surround
 *								0x04	FHG HEMP3 Surround
 *								0x05	DTS Matrix Surround
 *								0x06	CT Surround
 *								0xFF	Program not available

 ******************************************************************/
uint8 idm_get_surround_mode (uint8 program)
{

	uint8 data_buffer[2];
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
    uint8 data_buffer[3];

    data_buffer[0] = 0x05; //port ID must be 5
    data_buffer[1] = 0x01; //one type of data to get
    data_buffer[2] = 0x01; //get short name for now.


    Build_IDM_write_message(SET_SIS_CNFG_OPCODE,
                            16,
                            3,
                            &data_buffer[0]);

    IDMtransfer();
}
#endif


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
	uint8 data_buffer;

	data_buffer = 0x00;

	Build_IDM_write_message(SYS_STATUS_OPCODE,
							14,
							1,
							&data_buffer);
	IDMtransfer();
}









/******************************************************************
 *
 *      ReadSoftwareVersion
 *
 *      Description:Reads Software Version
 *
 *      Input:  uint8 * p, uint8 len
 *      Output: None
 *
 ******************************************************************/
void ReadSoftwareVersion(uint8 * p, uint8 len)
{
#ifdef DRI352
    uint8 data_buffer[1];
    uint8 *pIDM = &idmdata[12];
    uint8 i;

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
#else //
    uint8 data_buffer[1];
    uint8 *pIDM = &idmdata[13];
    uint8 i;

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
 *      ChangeFMMode
 *
 *      Description: Enables/disables mono mode.
 *
 *      Input:  uint8 mono_stereo, uint8 dac
 *      Output: void
 *
 ******************************************************************/
void ChangeFMMode (uint8 mono_stereo, uint8 dac)
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


#if 0

/*********************************************************************
*
*       SetConfigParamFM
*
*       Description: Set FM configuration parameter
*       Input : uint8, uint32
*       Output: BOOL
*
*********************************************************************/

BOOL SetPreBWInterfMin (uint32 ival)
{
	return SetConfigParamFM(0x35, ival);
}

BOOL SetPostBWInterfMin (uint32 ival)
{
	return SetConfigParamFM(0x43, ival);
}

BOOL SetPostBWInterfMax (uint32 ival)
{
	return SetConfigParamFM(0x44, ival);
}

BOOL SetSLInterfMax (uint32 ival)
{
	return SetConfigParamFM(0x5A, ival);
}

//dabb
BOOL SetLeftRightScaling(uint32 ival)
{
	return SetConfigParamFM(0x01, ival);
}

//dabb 081118 softmute
BOOL SetAudioScalingMin(uint32 ival)
{
	return SetConfigParamFM(0x5D, ival);
}

//dabb 081118 softmute
BOOL SetAudioScalingMax(uint32 ival)
{
	return SetConfigParamFM(0x5E, ival);
}

static BOOL SetConfigParamFM (uint8 param, uint32 ival)
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

#endif

void Tuner_Req_Radio_Monitor_Command(unsigned char Radio_Service,unsigned char SIS_Flag,unsigned char PSD_Flag)
{
	if(SIS_Flag)
		{
		}
//	if(PSD_Flag)
//		get_psd();
		
	
       
}





