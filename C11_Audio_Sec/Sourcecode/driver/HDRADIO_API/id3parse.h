/*********************************************************************************************
* iBiquity Digital Proprietary																**
* Copyright 2005 iBiquity Digital Corporation								  				**
**																			  				**
** PROJECT:             HD Radio SDK   									  				    **
**																			  				**
** FILE:                id3parse.h										  					**
**																			  				**
** DESCRIPTION: 		This file Contains data and function declares used	 				**
**						for ID3 parsing														**
**********************************************************************************************/

#if     !defined(id3parse_H)
#define id3parse_H


/*!
 *      \file id3parse.h
 *      \brief ID3 parsing function handler
 */

/*! \fn UINT32 Parse_ID3(UINT8 * InBuff)
        *  \brief Parse ID3 data from the input buffer
        *  \param UINT8 * InBuff
*/
uint32 Parse_ID3(uint8 * InBuff);
void TrimTag(uint8 *strVal);
void Swap_Long_4(int32 *tni4);


#endif /* End of id3parse_H */
