/*

	PlainDAC Library, optimized Digital to Analog Conversion
	Copyright (C) 2010 Didier Longueville

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PlainDAC_h /* Prevent loading library twice */
#define PlainDAC_h

#include <Arduino.h>

#define DAC_LIB_REV 0x04
/* Data acquisition engine status */
#define DAC_ACQ_ENG_STOPPED 0
#define DAC_ACQ_ENG_STARTED 1
/* Data acquisition status */
#define DAC_DAT_ACQ_IDLE 0 /* Idle, no acquisition */
#define DAC_DAT_ACQ_WAITING 1 /* Ready to acquire, waiting for an ADC sample to be taken */
#define DAC_DAT_ACQ_TRIGGERED 2 /* An ADC sample has been taken, ready to be recorded */
/* Data acquisition modes */
#define DAC_ACQ_MOD_UINT8 0 /* 8 bits mode */
#define DAC_ACQ_MOD_UINT16 1 /* 10 bits mode, 2 bytes */
#define DAC_ACQ_MOD_DEL_ENCODING 2 /* 10 bits mode, delta encoding */
#define DAC_ACQ_MOD_DOUBLE 3 /* 32 bits double format */
/* Voltage references */
#define DAC_REF_VOL_EXTERNAL 0x00 /* As per AREF pin voltage */
#define DAC_REF_VOL_DEFAULT	0x01 /* VCC */
#define DAC_REF_VOL_INTERNAL 0x03 /* 1V1 on ATMEGA 168/328 */

class PlainDAC {
public:
	/* Constructor */
	PlainDAC(void);
	~PlainDAC(void);
	/* Functions */
	void AcquireData(uint8_t *vData, uint8_t channel);
	uint16_t InitDataVector(uint8_t **vData);
	int16_t ReadIntegerData(uint8_t *vData, boolean firstOne);
	int16_t ReadData(uint8_t *vData, uint16_t index);
	double ReadDoubleData(uint8_t *vData, uint16_t index);
	uint8_t Revision(void);
	void SetAcquisitionParameters(uint16_t frequency, uint16_t samples, uint8_t acqMode, uint8_t vRef);
	uint8_t SignedIntToUnsignedByte(int16_t signedIntValue);
	void StartAcquisitionEngine(void);
	void StopAcquisitionEngine(void);
	int16_t UnsignedByteToSignedInt(uint8_t signedByteValue);

	union variantTyp {
		double dValue;
		uint8_t bValue;
		uint16_t iValue;
		uint8_t vBuffer[4];
	} variant;

private:
	/* Functions */
	uint8_t Exponent(uint8_t mantissa, uint16_t x);

};

#endif
