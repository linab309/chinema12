/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                id3parse.c										  					**
**																			  				**
** DESCRIPTION: 		This file Contains data and function definitions used 				**
**						for ID3 parsing														**
**********************************************************************************************/

//#include "Processor.h"
#include <string.h>
#include "..\main\Macro.h"
#include "ID3parse.h"
//#include "idmcom.h"

#define ID3_HDR_LEN 10
#define ID3_TAG_ID_LEN 3
#define MEM_ALLOC_FAILED 20

#define TITLE_TAG		 "TIT2"
#define TRACK_TAG		 "TRCK"
#define ARTIST_TAG		 "TPE1"
#define COMMENT_TAG		 "COMM"
#define ALBUM_TAG		 "TALB"
#define GENRE_TAG		 "TCON"
#define UFID_TAG		 "UFID"
#define COMMERCIAL_TAG	 "COMR"


#define ID3_TAG_LEN 65

//ID3 header
typedef struct _ID3_HDR_Type
{
    uint8 Tag[3];
    uint8 Version;
    uint8 Revision;
    uint8 Flag;
    uint8 Length[4];
} ID3_HDR_Type;

uint8 Artist[ID3_TAG_LEN];
uint8 Title[ID3_TAG_LEN];

uint8 Artist_len = 0;
uint8 Title_len = 0;
#if 0
extern uint8 Album[ID3_TAG_LEN];
extern uint8 Genre[ID3_GENRE_LEN];
extern uint8 UFIDOwner[ID3_UFID_URL_LEN];
extern uint8 UFIDData[ID3_UFID_DATA_LEN];
#endif



uint8 Artist_Text_Encoding;
uint8 Title_Text_Encoding;

void TrimTag(uint8 *strVal)
{
	uint8 *p1, *p2;
	uint8 i;

	if(strVal == 0) return ;
	if(strlen(strVal) == 0) return ;

	p1 = strVal;
	while(p1 != 0 && *p1 == ' ')
	{
	 	for(i=0; i<strlen(strVal); i++)
			*(strVal+i) = *(strVal+i+1);
	}
	p2 = p1 + strlen(p1) -1;
	while(*p2 == ' ' && p1 != p2)
		p2--;

	*(p2+1) = 0;
}

/******************************************************************************
 *
 *     Swap_Long_4
 *
 *	Description : Swap long4
 *
 *	Input   	: int32 *
 *	Output      : None
 *
 ******************************************************************************/
void Swap_Long_4(int32 *tni4)
/* 4 byte signed long integers */
{

    *tni4=(((*tni4>>24)&0xff) | ((*tni4&0xff)<<24) |
           ((*tni4>>8)&0xff00) | ((*tni4&0xff00)<<8));
}


//functions

//uint32 Parse_ID3(uint8 * InBuff, ID3DataType * ID3_Data)
/******************************************************************************
 *
 *     Parse_ID3
 *
 *	Description : Parse ID3
 *
 *	Input   	: uint8 * InBuff, uint8 * pArtist, uint8 * pTitle, uint8 * pComment
 *	Output      : uint32
 *
 ******************************************************************************/
uint32 Parse_ID3(uint8 * InBuff)
{
//*******************************************************************************************
//Function: Parse_ID3
//Purpose:           Function to parse a buffer containing ID3 formatted data.
//                   The frame id's supported are TIT2, TPE1, TALB, TCON, COMM, COMR, and UFID
//                   The maximum ID3 buffer len is limited to 1024 bytes.
//Arguments:         [in]InBuff()        - raw ID3 input data
//                   [in,out]ID3_Data    - Output structure containing parsed ID3 data
//Returns:   1   - success
//           0   - unknown error
//           2  - ID3 length = 0//
//           3  - parse_comr failed
//           4  - parse_ufid failed
//**************************************************************************************************
//                                             ID3 Frame Format
// -------------------------------------------------------------------
//|  Frame ID     |       Size    |  Flags | Txt-Dcsr| Text String        |
// -------------------------------------------------------------------
//|     0 |     1 |     2 |     3 | 4 | 5 |     6 | 7 | 8 |     9  |    10       |      11 |  Size -1 |
// -------------------------------------------------------------------
	ID3_HDR_Type ID3_Header;
	uint32 ID3_Len;               //length of ID3 data not including 10 byte TAG field
	int32 Buff_Index;            //index pointer
	int32 StrLen;                //number of chars in tag
	int32  StrLen2;               //number of chars in tag

	uint8 Frame_ID[5];
	int16 i;
	BOOL retVal = TRUE;

	//get header info
	memcpy((void *)&ID3_Header, (void *)&InBuff[0], ID3_HDR_LEN);
	//validate tag field for 'ID3'
	//add code
	//find length of data
	//msb bit 7 is ignored in 4 byte length specifier
	ID3_Len = ( (ID3_Header.Length[0] * 2097152) +
		(ID3_Header.Length[1] * 16384) +
		(ID3_Header.Length[2] * 128) +
		(ID3_Header.Length[3]) );

	if((ID3_Header.Tag[0] != 'I')||(ID3_Header.Tag[1] != 'D')||(ID3_Header.Tag[2] != '3') || (ID3_Len == 0)  )
		return 0;

	if (ID3_Len > 512)
		ID3_Len = 512;

	/* Flush all PSD data */
	Artist[0] = '\0';
	Title[0] = '\0';

	//initialize index pointer to end of header
    Buff_Index = ID3_HDR_LEN;
	debug_printf("Parse_ID3\n");
    do
    {
		//get Frame ID - 4 bytes
		memcpy((void *)Frame_ID, (void *)&InBuff[Buff_Index], 4);
//		memcpy((void *)&Frame_ID[0], (void *)&InBuff[Buff_Index], 4);
		//add null
		Frame_ID[4] = 0x00;
		//increment pointer - just read Frame ID - 4 bytes
		Buff_Index += 4;
		//get length of title
		memcpy((void *)&StrLen, (void *)&InBuff[Buff_Index], 4);
		//note: except for the UFID frame, string length includes the
		//"text encoding description" byte
		//copy StrLen-1 bytes to temp buffer
		Swap_Long_4(&StrLen);

		StrLen2 = StrLen;
		//just read frame length - 4 bytes
		Buff_Index += 4;
		//skip 2 byte flag
		Buff_Index += 2;
		//get ascii text
		//what did we read?
		if (memcmp((void*)(void *)ARTIST_TAG, (void *)Frame_ID, 4) == 0)	//"TPE1"
		{
			//Artist_Text_Encoding = InBuff[Buff_Index];
			Buff_Index += 1;	//text encoding byte

			if (StrLen)
			{
				if (StrLen > ID3_TAG_LEN)
				{
					StrLen = ID3_TAG_LEN;
				}
				
				memcpy((void *)Artist, (void *)&InBuff[Buff_Index], StrLen - 1);
				Artist[StrLen - 1] = '\0';
				//Artist_len = StrLen;
				TrimTag(Artist);
				Artist_len  = strlen(Artist);
			}
			else
			{
				Artist[0] = '\0';
			}
		}
      		  else if (memcmp((void*)(void*)TITLE_TAG, (void*)Frame_ID, 4) == 0)	//"TIT2"
      		  {
		//	Title_Text_Encoding = InBuff[Buff_Index];
			Buff_Index += 1;	//text encoding byte


			if (StrLen)
			{
				if (StrLen > ID3_TAG_LEN)
				{
					StrLen = ID3_TAG_LEN;
				}
				memcpy((void *)Title, (void *)&InBuff[Buff_Index], StrLen - 1);
				Title[StrLen - 1] = '\0';
				TrimTag(Title);
				Title_len  = strlen(Title);
			}
			else
			{
				Title[0] = '\0';
			}
		  }

#if 0		
		else if(memcmp((void*)(void*)COMMENT_TAG, (void*)Frame_ID, 4) == 0)	//"COMM"
		{
			Comment_Text_Encoding = InBuff[Buff_Index];
			Buff_Index += 1;		//text encoding byte

#ifdef USE_COMMENT
			if (StrLen)
			{

				if (StrLen > ID3_COMMENT_LEN)
				{
					StrLen = ID3_COMMENT_LEN;
				}
				memcpy((void *)Comment, (void *)&InBuff[Buff_Index], StrLen - 1);
				Comment[StrLen - 1] = '\0';
				//if there is a null in between the string, then we will make it as a space
				for(i=0; i<StrLen - 1; i++)
				{
					if(Comment[i] == 0)
						Comment[i] = 0x20; // space
				}
				TrimTag(Comment);
			}
			else
			{
				Comment[0] = '\0';
			}
#endif
		}
		else if(memcmp((void*)ALBUM_TAG, (void*)Frame_ID, 4) == 0)	//"TALB"
		{
			Album_Text_Encoding = InBuff[Buff_Index];
			Buff_Index += 1;	//text encoding byte

			if (StrLen)
			{
				if (StrLen > ID3_TAG_LEN)
				{
					StrLen = ID3_TAG_LEN;
				}
				memcpy((void *)Album, (void *)&InBuff[Buff_Index], StrLen - 1);
				Album[StrLen - 1] = '\0';
				TrimTag(Album);
			}
			else
			{
				Album[0] = '\0';
			}
		}
		else if(memcmp((void*)GENRE_TAG, (void*)Frame_ID, 4) == 0)	//"TCON"
		{
			Genre_Text_Encoding = InBuff[Buff_Index];
			Buff_Index += 1;	//text encoding byte

			if (StrLen)
			{
				if (StrLen > ID3_GENRE_LEN)
				{
					StrLen = ID3_GENRE_LEN;
				}
				memcpy((void *)Genre, (void *)&InBuff[Buff_Index], StrLen - 1);
				Genre[StrLen - 1] = '\0';
				TrimTag(Genre);
			}
			else
			{
				Genre[0] = '\0';
			}
		}
		else if(memcmp((void*)UFID_TAG, (void*)Frame_ID, 4) == 0)	//"UFID"
		{
			// Vector does not have text encoding for UFID
			// UFID_Text_Encoding = InBuff[Buff_Index];
			// Buff_Index += 1;	//text encoding byte
			StrLen2 = StrLen2 + 1;
			if(strlen(UFIDOwner) == 0 || strlen(UFIDData) == 0)//Multiple APC UFIDs not allowed to be
																		//broadcast if encountered, prefer to just store first one.
																		//Second one should be ignored
			{
				UFID_Text_Encoding = 1;
				if (StrLen)
				{
					if (StrLen > (ID3_UFID_URL_LEN + ID3_UFID_DATA_LEN))
					{
						StrLen = ID3_UFID_URL_LEN + ID3_UFID_DATA_LEN;
					}
					Parse_UFID(&InBuff[Buff_Index], StrLen);
					UFIDOwner[ID3_UFID_URL_LEN - 1] = '\0';
					UFIDData[ID3_UFID_DATA_LEN - 1] = '\0';
				}
				else
				{
					UFIDOwner[0] = '\0';
					UFIDData[0] = '\0';
				}
			}
		}
		else if(memcmp((void*)TRACK_TAG, (void*)Frame_ID, 4) == 0)
		{
			Track_Text_Encoding = InBuff[Buff_Index];
			Buff_Index += 1;	//text encoding byte

#ifdef USE_TRACK		
			if (StrLen)
			{
				if (StrLen > ID3_TRACK_LEN)
				{
					StrLen = ID3_TRACK_LEN;
				}
				memcpy((void *)Track, (void *)&InBuff[Buff_Index], StrLen - 1);
				Track[StrLen - 1] = '\0';
			}
			else
			{
				Track[0] = '\0';
			}
#endif
		}
		else if(memcmp((void*)COMMERCIAL_TAG, (void*)Frame_ID, 4) == 0)
		{
			Commercial_Text_Encoding = InBuff[Buff_Index];
			Buff_Index += 1;	//text encoding byte

#ifdef USE_COMMERCIAL		
			if (StrLen)
			{
				Parse_Commercial(&InBuff[Buff_Index],StrLen,&Commercial_info);
			}
			else
			{
				memset(&Commercial_info,0,sizeof(COMMERCIAL_INFO));
			}
#endif
		}
#endif		
		else
		{
			//Skip text encoding byte
			Buff_Index += 1;
			//unknown frame field
			//return 0;
		}
		//'just read ASCII text
		Buff_Index += (StrLen2 - 1);
	}
	while ( Buff_Index < (ID3_TAG_ID_LEN + ID3_Len) );

	//got here if no errors occurred
	return 1;
} //end function Parse_ID3

