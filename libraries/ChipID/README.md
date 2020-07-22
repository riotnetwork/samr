#ported and simplified from  ArduinoChipID ( https://github.com/ricaun/ArduinoChipID )

This Library gets the Unique ID / Manufacture Serial Number from the Atmel /Microchip   SAMD, SAML and SAMR series of Cortex M0 microcontrollers

# Microcontrollers

ChipID supports the [Microcontrollers](MCU.md).

* Atmel SAMR ARM
* Atmel SAMD ARM
* Atmel SAML ARM

# Installation

This is built as part of the SAMR core libraries and should be available like SPI or Wire
## Examples

The library comes with [examples](examples). After installing the library you need to restart the Arduino IDE before they can be found under **File > Examples > ChipID**.

---

# Reference

## Include Library

```c
#include <ChipID.h>
```

### Variable: UniqueID & UniqueIDsize

UniqueID has UniqueIDsize bytes array of the Unique Serial ID.

```c
for(size_t i = 0; i < UniqueIDsize; i++)
  Serial.println(UniqueID[i], HEX);
```

### Method: ChipIDdump

Print the hexadecimal bytes of the Unique Serial ID on the Stream.

```c
void ChipIDdump(Stream);
```

### Variable: UniqueID8

UniqueID8 has the last 8 bytes array of the Unique Serial ID.

```c
for(size_t i = 0; i < 8; i++)
  Serial.println(UniqueID8[i], HEX);
```

### Method: ChipID8dump

Print the last eight hexadecimal bytes of the Unique Serial ID on the Stream.

```c
void ChipID8dump(Stream);
```

Do you like this library? Please [star the origonal project on GitHub](https://github.com/ricaun/ArduinoChipID/stargazers)!
