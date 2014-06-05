/*************************************************************************************************
** Copyright ?2005 iBiquity Digital Inc.
** All rights reserved.
**
** PROJECT:             Atmel HD Radio
**
** FILE:                rbds.c
**
** Description:        	RDBS(Radio Broadcast Data System) Decoder
**
*************************************************************************************************/

//#include "processor.h"

//#include "psd.h"
#include "rbds.h"
#include <string.h>
#include "Tuner.h"
#include "../main/debug.h"

#define ID3_TAG_LEN 65
extern uint8 Artist[ID3_TAG_LEN];
extern uint8 Title[ID3_TAG_LEN];
//extern uint8 short_station_name[8];

#define DISPLAY_PS_STRING
#define DISPLAY_RADIO_TEXT

#define strPS_Buffer	Title
#define strRadioText	Artist

//#define callingSign short_station_name

//uint8 PINumber = 0;

static uint8 RBDS_timer = 0;

//Group information words
uint16 iInfoWord1,iInfoWord2,
	   iInfoWord3,iInfoWord4;

// Block 2 common data vars
uint8 btGroupCode, btAB, btTP_Enabled, btPTY;

//Block 2 Type 0 group vars
uint8 btTA_Flag,btMS_Flag,btDI_Flag,btSegAddr;

//Block 2 Type 2 group vars Radio Text
uint8 btText_AB_Flag=0,btTextSegAddr=0;

BOOL bRBDSDataAvail = FALSE;

BOOL bRBDSEnabled = TRUE;

//extern uint16 microTimerCount;

/*********************************************************************
* Function: RBDS_Blk2_Decode
* Purpose:  Description: Extracts Block to data common to all groups.
* Input:    [IN] Info - 2 byte information word
*		    [IN,OUT] GroupCode - identifies group type
*		    [IN,OUT] AB_Type - 0 = group A, 1 = Group B
*		    [IN,OUT] TP_Enabled - traffic program flag, 0 = disabled, 1 = enabled
*		    [IN,OUT] PTY - program id code, 0 to 31.
* Returns:  None
*********************************************************************/

void RBDS_Blk2_Decode(uint32 Info,uint8 *GroupCode,uint8 *AB_Type, uint8 *TP_Enabled, uint8 *PTY)
{

	uint32 ltemp;
	ltemp = Info;

	//bit 15 - bit 12: 4 bits
	*GroupCode = (uint8)(ltemp>>12) & 0x0F;	// Group Type Code. 4 bits

	//bit 11:  1 bit
	*AB_Type =(uint8) (ltemp>>11) & 0x01;		// 0 = Version A or 1 = Version B. 1 bit

	//bit 10: 1 bit
	*TP_Enabled = (uint8)(ltemp>>10) & 0x01;	// Traffic Progam code 1 bit

	//bit 9 - bit 5 :5 bits
	*PTY = (uint8) (ltemp>>5) & 0x1F;			// Program Type code 5 bits

	return;
}

/*********************************************************************
* Function: RBDS_GetCallSign
* Purpose:  To get the call letter string from PI.
*           in block 2 type 0 groups.
* Input:    [IN] PI
*		        [IN,OUT] szCallSign - call letter
*			[IN,OUT] - SegAddr, denotes character position in PS string
* Returns:  None
*********************************************************************/

void RBDS_GetCallSign(uint32 PI, uint8 *szCallSign)
{

	uint32 lPI;
	uint32 lVal1, lVal2, lVal3;

	uint8 cFirstChar;
	uint8 c3rdPos;
	uint8 c2ndPos;
	uint8 c1stPos;
	uint8 len=0;
	lPI = PI;

	if(lPI < 0x54AB)
	{
		cFirstChar = 'K';
		lPI = lPI - 4096;    // in the doc page no 86
		szCallSign[len]=cFirstChar;
	}
	else
	{
		cFirstChar = 'W';
		lPI = lPI - 21672;   // in the doc page no 86
		szCallSign[len]=cFirstChar;
	}

    lVal1 = (lPI - (lPI % 676)) / 676;
    if (lVal1 <= 25)
    {
		c1stPos = (uint8)(lVal1 + 0x41);
		len++;
		szCallSign[len]=c1stPos;
	}

    lVal2 = (lPI - (lVal1 * 676));
    lVal2 = (lVal2 - (lVal2 % 26)) / 26;
    if (lVal2 <= 25)
    {
		c2ndPos =(uint8)(lVal2 + 0x41);
		len++;
		szCallSign[len]=c2ndPos;

	}

    lVal3 = (lPI - (lVal1 * 676) - (lVal2 * 26));
    if (lVal3 <= 25)
	{
		c3rdPos = (uint8)(lVal3 + 0x41);
		len++;
		szCallSign[len]=c3rdPos;
	}
	szCallSign[len+1]=0;
 	return;
}

/*********************************************************************
* Function: RBDS_B2_0_Decode
* Purpose:  Decodes basic tuning and switching information found
*           in block 2 type 0 groups.
* Input:    [IN,OUT] - Info, block 2 information word
*			[IN,OUT] - TA_Flag, traffic anouncement flag. 0 = disabled, 1 = enabled
*           [IN,OUT] - MS_Flag, music/speech flag. 0 = speech, 1 = music.
*			[IN,OUT] - DI_Flag, decoder information bit. This bit's weight depends
*                      on the segment address value.
*			[IN,OUT] - SegAddr, denotes character position in PS string
* Returns:  None
*********************************************************************/

void RBDS_B2_0_Decode(uint32 Info,uint8 *TA_Flag, uint8 *MS_Flag,
					uint8 *DI_Flag,
                    uint8 *SegAddr)
{
	//if msb bit is set, then Info will go negative
	uint32 lTemp = 0;
	lTemp = Info;
    //get TA bit (shift-right 4)
    *TA_Flag = (uint8)((lTemp >> 4) & 0x1);
    //get MS flag (shift-right 3)
    *MS_Flag = (uint8)((lTemp >> 3) & 0x1);
    //get DI flag (shift-right 2)
    *DI_Flag = (uint8)((lTemp >> 2) & 0x1);
    //get segment address (lower 2 bits)
    *SegAddr =(uint8)(lTemp & 0x3);
}

/*******************************************************************************************
* Function: RBDS_B2_2_Decode(uint32 Info, uint8* AB_Flag, uint8* Txt_SegAddr)
* Purpose:  Decodes radio text found in block 2 type 2A or 2B groups
* Input:    [IN,OUT] - Info, block 2 information word
*			[IN,OUT] - AB_Flag, type of radio text. 0 = A (64 chars), 1 = B (32 chars)
*					   If 2A type then block 3 and 4 contain radio text
*					   If 2B type then block 3 contains PI code, type 4 contains radio text
*			[IN,OUT] - segment address, denotes character position in PS string
* Returns:  None
********************************************************************************************/
void RBDS_B2_2_Decode(uint32 Info, uint8* AB_Flag, uint8* Txt_SegAddr)
{
 	uint32 lTemp;
    //if msb bit is set, then Info will go negative
    lTemp = Info;
    //get text A/B bit (shift-right 4)
    *AB_Flag = (uint8)((lTemp >> 4) & 0x1);
    //get text segment address (mask off lower 4 bits)
    *Txt_SegAddr = (uint8)(lTemp & 0xF);
}

void RBDS_GetPSDesc(uint32 blockbuf_4, uint8 *szAltFreq)
{
	uint8 ntemp;

	ntemp = (int8)((blockbuf_4 >> 8) & 0xFF);
	if((btSegAddr*2) < ID3_TAG_LEN-1) {
		szAltFreq[(btSegAddr*2)] = ntemp;
	}
	ntemp = (int8)(blockbuf_4 & 0xFF);
	if(((btSegAddr*2) + 1) < ID3_TAG_LEN-1) {
		szAltFreq[(btSegAddr * 2)+1]= ntemp;
	}
}

void RBDS_GetRadioText(uint32 blockbuf_3,uint32 blockbuf_4, uint8* szRadioText)
{
	int8 nbit;

	if ( btAB == 0)
	{
		nbit = (int8)((blockbuf_3 >> 8) & 0xFF);
		if(( btTextSegAddr*4) < ID3_TAG_LEN-1){
			szRadioText[btTextSegAddr*4] = nbit;
		}
		nbit = (int8)(blockbuf_3 & 0xFF);
		if((( btTextSegAddr*4) + 1) < ID3_TAG_LEN-1){
			szRadioText[(btTextSegAddr*4)+1]= nbit;
		}

		nbit =(int8) ((blockbuf_4 >> 8) & 0xFF);
		if((( btTextSegAddr*4) + 2) < ID3_TAG_LEN-1){
			szRadioText[(btTextSegAddr*4)+2] = nbit;
		}
		nbit =(int8)(blockbuf_4 & 0xFF);
		if((( btTextSegAddr*4) + 3) < ID3_TAG_LEN-1){
			szRadioText[(btTextSegAddr*4)+3]= nbit;
		}
	}
	else if ( btAB == 1)
	{
		nbit = (int8)((blockbuf_4 >> 8) & 0xFF);
		if(( btTextSegAddr*2) < ID3_TAG_LEN-1){
			szRadioText[btTextSegAddr*2] = nbit;
		}
		nbit =(int8)(blockbuf_4 & 0xFF);
		if((( btTextSegAddr*2) + 1) < ID3_TAG_LEN-1){
			szRadioText[(btTextSegAddr*2)+1]= nbit;
		}
	}

}

/****************************************************
* Function: void RBDS_Init(void)
* Purpose:  Initialize the RBDS related values.
* Input:    None
* Returns:  None
*******************************************************/
void RBDS_Init(void)
{
	memset(strRadioText,0,sizeof(strRadioText));
	memset(strPS_Buffer,0,sizeof(strPS_Buffer));
//	memset(callingSign,0,sizeof(callingSign));
	EnableRBDS(TRUE);
}

/****************************************************
* Function: void RBDS_Decode(void)
* Purpose:  RBDS Main routine.
* Input:    None
* Returns:  None
*******************************************************/
#define RBDS_SIGNAL_TIMEOUT		15 // 15 seconds

void RBDS_Decode(void)
{
	static uint16 lastRBDSSignalAcqCnt = 0;
//	uint16 diff;
 	uint8 buf[8];

	if(!bRBDSEnabled) return;

    // Delay before displaying RDS info.
    // This fixes the problem of displaying RDS momentarily before HD data.
    if (RBDS_timer < 50)
    {
        RBDS_timer++;
       return;
    }

	
	
	if(!get_rbds_group(buf))
	{
		//We will wait for n seconds to change the state
	//     	diff = microTimerCount - lastRBDSSignalAcqCnt;
		if(lastRBDSSignalAcqCnt >= RBDS_SIGNAL_TIMEOUT)
			{
			lastRBDSSignalAcqCnt = 0   ;
			bRBDSDataAvail = FALSE;
			}
		else
			lastRBDSSignalAcqCnt++;
		return;
	}
	
	debug_printf("RBDS_DECODE\n");
	
	lastRBDSSignalAcqCnt = 0;
	bRBDSDataAvail = TRUE;

	memcpy(&iInfoWord1, &buf[0], 2);
	memcpy(&iInfoWord2, &buf[2], 2);
	memcpy(&iInfoWord3, &buf[4], 2);
	memcpy(&iInfoWord4, &buf[6], 2);

	//Decode the buffer.
//	RBDS_GetCallSign(iInfoWord1,callingSign);

	RBDS_B2_0_Decode(iInfoWord2, &btTA_Flag, &btMS_Flag, &btDI_Flag, &btSegAddr);
	RBDS_Blk2_Decode(iInfoWord2, &btGroupCode, &btAB, &btTP_Enabled, &btPTY);

	switch(btGroupCode)
	{
	case 0:
		#ifdef DISPLAY_PS_STRING
		RBDS_B2_0_Decode(iInfoWord2, &btTA_Flag, &btMS_Flag, &btDI_Flag, &btSegAddr);
		RBDS_GetPSDesc(iInfoWord4,strPS_Buffer);
		#endif
		break;
	case 2:
		RBDS_B2_2_Decode(iInfoWord2,&btText_AB_Flag,&btTextSegAddr);
		RBDS_GetRadioText(iInfoWord3,iInfoWord4,strRadioText);
		break;
	}
}

/****************************************************
* Function: BOOL RBDS_IsValid(void)
* Purpose:  Check if RBDS data available
* Input:    None
* Returns:  BOOL
*******************************************************/
BOOL RBDS_IsValid(void)
{
	return bRBDSDataAvail;
}

/****************************************************
* Function: BOOL RBDS_SetInvalid(void)
* Purpose:  Invlaidate RBDS data availablity
* Input:    None
* Returns:  None
*******************************************************/
void RBDS_SetInvalid(void)
{
	bRBDSDataAvail = FALSE;
	memset(strRadioText,0,sizeof(strRadioText));
	memset(strPS_Buffer,0,sizeof(strPS_Buffer));
//	memset(callingSign,0,sizeof(callingSign));
	//rbds_flush();
        RBDS_timer = 0;
}


#if 0
/****************************************************
* Function: uint8 *RBDS_GetCallSignStr(void)
* Purpose:  Used to get the call sign of the current frequency.
* Input:    None
* Returns:  uint8*
*******************************************************/
uint8 *RBDS_GetCallSignStr(void)
{
//	return callingSign;
}

/****************************************************
* Function: uint8 RBDS_GetPTYCodeStr(void)
* Purpose:  Used to get the PTY Code of the current frequency.
* Input:    None
* Returns:  uint8
*******************************************************/
uint8 RBDS_GetPTYCode(void)
{
	return btPTY;
}
#endif
/********************************************************************************
* Function: uint8 *RBDS_GetPSDescriptionStr(void)
* Purpose:  Used to get the Program Service Description of the current frequency.
* Input:    None
* Returns:  uint8*
*********************************************************************************/
uint8 *RBDS_GetPSDescriptionStr(void)
{
	return strPS_Buffer;
}

/**********************************************************************
* Function: uint8 *RBDS_GetRadioTextStr(void)
* Purpose:  Used to get the Radio text string of the current frequency.
* Input:    None
* Returns:  uint8*
***********************************************************************/

uint8 *RBDS_GetRadioTextStr(void)
{
	return strRadioText;
}

void RBDS_SetEnable(BOOL bEnable)
{
	bRBDSEnabled = bEnable;
	EnableRBDS(bEnable);
	if(!bEnable)
	{
		memset(strRadioText,0,sizeof(strRadioText));
		memset(strPS_Buffer,0,sizeof(strPS_Buffer));
//		memset(callingSign,0,sizeof(callingSign));
	}
}

BOOL RBDS_IsEnabled(void)
{
	return bRBDSEnabled;
}
