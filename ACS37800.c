#include "ACS37800.h"
#include <stdint.h>
#include <stdbool.h>
#include "LL_spi.h"
#include <stdio.h>

enum ACS37800_Registers_t
{
	ACS37800_Registers_B = 0x1B,
	ACS37800_Registers_C = 0x1C,
	ACS37800_Registers_D = 0x1D,
	ACS37800_Registers_E = 0x1E,
	ACS37800_Registers_F = 0x1F,
	ACS37800_Registers_RMS_V_I = 0x20,
	ACS37800_Registers_RMS_P = 0x21
};

const uint32_t ADDRESS_MASK = 0x7F;
const uint32_t WRITE = 0x00;
const uint32_t READ = 0x80;

/*
* Convert an unsigned bitfield which is right justified, into a floating point number
*
*    data        - the bitfield to be converted
*    binaryPoint - the binary point (the bit to the left of the binary point)
*    width       - the width of the bitfield
*    returns     - the floating point number
*/
float ConvertUnsignedFixedPoint(uint32_t inputValue, uint16_t binaryPoint, uint16_t width)
{
	uint32_t mask;

	if (width == 32)
	{
		mask = 0xFFFFFFFF;
	}
	else
	{
		mask = (1UL << width) - 1UL;
	}

	return (float)(inputValue & mask) / (float)(1L << binaryPoint);
}

/*
 * Sign extend a bitfield which if right justified
 *
 *    data        - the bitfield to be sign extended
 *    width       - the width of the bitfield
 *    returns     - the sign extended bitfield
 */
int32_t SignExtendBitfield(uint32_t data, uint16_t width)
{
	// If the bitfield is the width of the variable, don't bother trying to sign extend (it already is)
	if (width == 32)
	{
		return (int32_t)data;
	}

	int32_t x = (int32_t)data;
	int32_t mask = 1L << (width - 1);

	x = x & ((1 << width) - 1); // make sure the upper bits are zero

	return (int32_t)((x ^ mask) - mask);
}

/*
 * Convert a signed bitfield which is right justified, into a floating point number
 *
 *    data        - the bitfield to be sign extended then converted
 *    binaryPoint - the binary point (the bit to the left of the binary point)
 *    width       - the width of the bitfield
 *    returns     - the floating point number
 */
float ConvertSignedFixedPoint(uint32_t inputValue, uint16_t binaryPoint, uint16_t width)
{
	int32_t signedValue = SignExtendBitfield(inputValue, width);
	return (float)signedValue / (float)(1L << binaryPoint);
}

float ACS37800_getRMSVoltage(struct ACS37800_t* instance)
{
	uint8_t command = (ACS37800_Registers_RMS_V_I & ADDRESS_MASK) | READ;
	uint8_t buffer[5] = {command, 0, 0, 0, 0};
	instance->SPIFunction(&buffer, 5 , 0, spmMode0);
	buffer[0] = command;
	instance->SPIFunction(&buffer, 5 , 0, spmMode0); //read again because data becomes available the next read cycle
	uint32_t value = (uint32_t)buffer[1];
	value |= (uint32_t)buffer[2] << 8;
	value |= (uint32_t)buffer[3] << 16;
	value |= (uint32_t)buffer[4] << 24; // high byte
	uint16_t b = value & 0xFFFF;
	float volts = ConvertUnsignedFixedPoint(b, 16, 16);
	volts *= 250; //Convert to mV (Differential Input Range is +/- 250mV)
	volts /= 1000; //Convert to Volts
	volts *= instance->VoltageDivider;
	volts *= 1.19;
	return volts;
}

float ACS37800_getRMSPower(struct ACS37800_t* instance)
{
	uint8_t command = (ACS37800_Registers_RMS_P & ADDRESS_MASK) | READ;
	uint8_t buffer[5] = {command, 0, 0, 0, 0};
	instance->SPIFunction(&buffer, 5 , 0, spmMode0);
	buffer[0] = command;
	instance->SPIFunction(&buffer, 5 , 0, spmMode0); //read again because data becomes available the next read cycle
	uint32_t value = (uint32_t)buffer[1];
	value |= (uint32_t)buffer[2] << 8;
	value |= (uint32_t)buffer[3] << 16;
	value |= (uint32_t)buffer[4] << 24; // high byte
	uint16_t b = value & 0xFFFF;
	float power = ConvertSignedFixedPoint(b, 15, 16);
	power *= 5328;
	power *= instance->VoltageDivider;
	power /= 1000; // Convert from mW to W
	return power;
}


