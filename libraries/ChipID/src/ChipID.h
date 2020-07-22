// Copyright © Luiz Henrique Cassettari. All rights reserved.
// Ported by Daniel de Kock for the SAMR series
// Licensed under the MIT license.

#ifndef _CHIP_ID_H_
#define _CHIP_ID_H_

#include <Arduino.h>

#if defined(ARDUINO_ARCH_SAMD)
#elif defined(ARDUINO_ARCH_SAMR)
#elif defined(ARDUINO_ARCH_SAML)
#else
#error "ChipID only works on SAMD, SAML or SAMR Architecture"
#endif


#if defined(ARDUINO_ARCH_SAMD)
#define UniqueIDsize 16
#define UniqueIDbuffer 16
#elif defined(ARDUINO_ARCH_SAMR)
#define UniqueIDsize 16
#define UniqueIDbuffer 16
#elif defined(ARDUINO_ARCH_SAML)
#define UniqueIDsize 16
#define UniqueIDbuffer 16
#endif

#define UniqueID8 (_UniqueID.id + UniqueIDbuffer - 8)
#define UniqueID (_UniqueID.id + UniqueIDbuffer - UniqueIDsize)

#define ChipIDdump(stream)                      \
	{                                             \
		stream.print("ChipID: ");       \
		for (size_t i = 0; i < UniqueIDsize; i++) \
		{                                         \
			if (UniqueID[i] < 0x10)               \
				stream.print("0");                \
			stream.print(UniqueID[i], HEX);       \
			stream.print(" ");                    \
		}                                         \
		stream.println();                         \
	}

#define ChipID8dump(stream)                \
	{                                        \
		stream.print("ChipID: ");  \
		for (size_t i = 0; i < 8; i++)       \
		{                                    \
			if (UniqueID8[i] < 0x10)         \
				stream.print("0");           \
			stream.print(UniqueID8[i], HEX); \
			stream.print(" ");               \
		}                                    \
		stream.println();                    \
	}

class ChipID
{
  public:
	ChipID();
	uint8_t id[UniqueIDbuffer];
};

extern ChipID _UniqueID;

#endif
