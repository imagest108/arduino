/*

	Example of use of the PlainDAC Library, optimized Digital to Analog Conversion
	Copyright (C) 2011 Didier Longueville

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

#include <PlainDAC.h>

PlainDAC DAC = PlainDAC(); /* Create DAC object */
const uint16_t samples = 64; /* Max value depends upon available memory space */
const uint16_t frequency = 8000; /* From 16 Hz to 64 kHz */
uint8_t *vData; /* 10 bits in 2 bytes data */
const uint8_t channel = 0; /* From 0 to 5 on Diecimila */
const uint8_t acqMode = DAC_ACQ_MOD_UINT16;
const uint8_t vRef = DAC_REF_VOL_DEFAULT;

void setup()
{  
	/* Initialize serial comm port */
	Serial.begin(115200); // 
	/* Set acquisition parameters */
	DAC.SetAcquisitionParameters(frequency, samples, acqMode, vRef);
	DAC.StartAcquisitionEngine();
	DAC.InitDataVector(&vData);
}

void loop() 
{
  DAC.AcquireData(vData, channel);
	printVector(); /* Print acquired data */
	while(1); /* Run Once */
	// delay(2000); /* Repeat after delay */
}

void printVector() 
{
	for (uint16_t i = 0; i < samples; i++) {
		double abscissa = ((i * 1.0) / frequency);
		/* First option */
		// uint16_t adcValue = ((vData[(i << 1) | 0x01] << 8) | vData[(i << 1)]);
		/* Second option */
		// memcpy((uint8_t*)&DAC.variant.vBuffer, (uint8_t*)&vData[(i << 1)], sizeof(uint16_t)); 
		// uint16_t adcValue = DAC.variant.iValue;		
		/* Third option */
		for (uint8_t j = 0; j < sizeof(uint16_t); j++) {
			DAC.variant.vBuffer[j] = vData[(i << 1) + j];
		}
		uint16_t adcValue = DAC.variant.iValue;
		/* Fourth option */
		// uint16_t adcValue = DAC.ReadData(vData, i == 0);
		/* Adjust formula to your taste */
		double ordinate = (((adcValue * 5.0) / 1024.0) - 2.5);
		/* Print */
		Serial.print(abscissa, 6);
		Serial.print(" ");
		Serial.print(ordinate , 4); 
		Serial.println();
	}
}

