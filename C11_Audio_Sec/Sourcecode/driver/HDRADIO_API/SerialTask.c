/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                serialtask.c											  			**
**																			  				**
** DESCRIPTION: 		Serial  functions													**
**********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <processor.h>
#include "globals.h"
#include "module.h"
//#include "tasks.h"
#ifdef IDM_SERIAL_COMMUNICATION
#include "sci_api.h"
#include "serialTask.h"

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

BOOL IdmSerialWrite(UINT8 WriteLen, UINT8 *WriteBuffer)
{
 	UINT8 idx;
	SCI_buffer_wr(&sci2, WriteBuffer, WriteLen);
/*	
	while (SCI_xmtbuf_length(&sci2))
	{
		MOD_ResetWDT();
	}
*/
	return TRUE;
}

/*********************************************************************************/
/** function: IdmSerialRead                                                      **/
/** description: read a logical message from the IDM using UART                 **/
/** input: [IN]UINT8 opcode - LM parameter                                      **/
/**        [IN]INT32 buffsize - buffer size of *buffer                          **/
/**        [IN,OUT]UINT8* buffer - pointer to memory used for returned UART		**/
/**								   data											**/
/** returns: 1-success,0-error                                                  **/
/*********************************************************************************/

BOOL IdmSerialRead (UINT8 opcode, UINT16 buffsize, UINT8 * buffer)
{
	int buffidx,bytes_read;
	int lm_byte_length;
	UINT8 msb,lsb;
	UINT8 currbyte;
	PARSE_STATE parse_state;
	BOOL valid;
	UINT16 size = 1;
	UINT16 timeout = 0;
	
	bytes_read = 0;
	buffidx = 0;
	lm_byte_length = 0;
	lsb = 0;
	msb = 0;
	valid = FALSE;
	
	parse_state = hdr_1_search;

	do
	{
		timeout = 0;

		/* See if there is a byte in the RX buffer */
		while((SCI_rcvbuf_length(&sci2) == 0) && timeout < SERIAL_TIMEOUT)			//timeout empirically derived
		{
			MOD_ResetWDT();
			timeout++;
		}

		if(timeout >= SERIAL_TIMEOUT)
		{
			/* If you are here often, there is some serious error with UART */
			SCI_flush(&sci2);
			return FALSE;
		}
		
		currbyte = SCI_getch(&sci2);
		
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
			while((SCI_rcvbuf_length(&sci2) == 0) && timeout < SERIAL_TIMEOUT)			//timeout empirically derived
			{
				MOD_ResetWDT();
				timeout++;
			}

			buffer[buffidx] = SCI_getch(&sci2);
			buffidx++;
			bytes_read++;

			if(timeout >= SERIAL_TIMEOUT)
			{
				/* If here, you didn't wait long enough for the complete packet to arrive */
				/* This may indicate an issue with UART itself */
				SCI_flush(&sci2);
				return FALSE;
			}
			MOD_ResetWDT();
		} while(bytes_read < size);
	}
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

#endif // IDM_SERIAL_COMMUNICATION





































