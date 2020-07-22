// Copyright © Luiz Henrique Cassettari. All rights reserved.
// Ported by Daniel de Kock for the SAMR series
// Licensed under the MIT license.

#include "ChipID.h"

ChipID::ChipID()
{

	// from section 9.3.3 of the datasheet ( SAMD ) or 11.6 of teh stasheet ( SAML and SAMR )
	#define SERIAL_NUMBER_WORD_0	*(volatile uint32_t*)(0x0080A00C)
	#define SERIAL_NUMBER_WORD_1	*(volatile uint32_t*)(0x0080A040)
	#define SERIAL_NUMBER_WORD_2	*(volatile uint32_t*)(0x0080A044)
	#define SERIAL_NUMBER_WORD_3	*(volatile uint32_t*)(0x0080A048)

	uint32_t pdwUniqueID[4];
	pdwUniqueID[0] = SERIAL_NUMBER_WORD_0;
	pdwUniqueID[1] = SERIAL_NUMBER_WORD_1;
	pdwUniqueID[2] = SERIAL_NUMBER_WORD_2;
	pdwUniqueID[3] = SERIAL_NUMBER_WORD_3;

	for (int i = 0; i < 4; i++)
	{
		id[i*4+0] = (uint8_t)(pdwUniqueID[i] >> 24);
		id[i*4+1] = (uint8_t)(pdwUniqueID[i] >> 16);
		id[i*4+2] = (uint8_t)(pdwUniqueID[i] >> 8);
		id[i*4+3] = (uint8_t)(pdwUniqueID[i] >> 0);
	}

}

ChipID _UniqueID;
