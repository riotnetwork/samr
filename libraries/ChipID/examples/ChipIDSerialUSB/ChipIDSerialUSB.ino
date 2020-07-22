//
// ChipIDSerialUSB.ino
//
// Example shows the UniqueID on the SerialUSB Monitor.
//

#include <ChipID.h>

void setup()
{
	SerialUSB.begin(115200);
  while (!SerialUSB);
	ChipIDdump(SerialUSB);
	ChipIDdump(SerialUSB);
}

void loop()
{
}