#ifndef ACS37800_LIBRARY_H
#define ACS37800_LIBRARY_H

#include <stdint.h>
#include <stdbool.h>
#include "LL_spi.h"

struct ACS37800_t
{
	uint8_t ChipID;
	// this is the ratio of RSense over RIso+RSense (e.g. 1K / (2M + 1K))
	float VoltageDivider;
	//points to a function to handle the SPI read/writes
	SPI_ReadWriteMethod_t SPIFunction;
};

float ACS37800_getRMSVoltage(struct ACS37800_t* instance);
float ACS37800_getRMSPower(struct ACS37800_t* instance);

#endif //ACS37800_LIBRARY_H
