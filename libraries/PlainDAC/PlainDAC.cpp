/*

	PlainDAC Library, optimized Digital to Analog Conversion
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
#include <Arduino.h>

/* Constants */
const int16_t maxDeltaValue = 127;
/* Volatile variables */
volatile uint8_t _dataAcqStatus = DAC_DAT_ACQ_IDLE;
volatile uint8_t _adcMSBs = 0;
volatile uint8_t _adcLSBs = 0;
/* Global variables */
uint8_t _acqEngStatus = DAC_ACQ_ENG_STOPPED;
uint8_t _acqMode;
uint8_t _vRef;
uint16_t _samples;
/* Test pins from PORTB */
//const uint8_t cycleSyncPin = PINB0; /* TODO: Comment in normal operation mode */
//const uint8_t adcSyncPin = PINB1; /* TODO: Comment in normal operation mode */

PlainDAC::PlainDAC(void)
{
/* Constructor */
//	DDRB |=  ((1 << cycleSyncPin) | (1 << adcSyncPin));  /* TODO: Comment in normal operation mode */
};

PlainDAC::~PlainDAC(void)
{
/* Destructor */
};

/* Public functions */
uint8_t PlainDAC::Revision(void)
{
	return(DAC_LIB_REV);
};

uint16_t PlainDAC::InitDataVector(uint8_t **vData)
{
/* Initialize vector of data and returns the size of the vector expressed in bytes */
	uint16_t size;
	switch(_acqMode) {
	case DAC_ACQ_MOD_UINT8:
		size = _samples * sizeof(uint8_t);
		break;
	case DAC_ACQ_MOD_UINT16:
		size = _samples * sizeof(uint16_t);
		break;
	case DAC_ACQ_MOD_DEL_ENCODING:
		size = (_samples * sizeof(uint8_t)) + 1;
		break;
	case DAC_ACQ_MOD_DOUBLE:
		size = _samples * sizeof(double);
		break;
	}
	*vData = (uint8_t*)malloc(size);
	/* Return the size of the vector in bytes */
	return(size);
};

void PlainDAC::SetAcquisitionParameters(uint16_t frequency, uint16_t samples, uint8_t acqMode, uint8_t vRef)
{
/* Set acquisition parameters */
	/* Stop acquisition engine if necessary */
	if (_acqEngStatus != DAC_ACQ_ENG_STOPPED) {
		StopAcquisitionEngine();
	}
	_acqMode = acqMode;
	_vRef = vRef;
	_samples = samples;
	uint16_t timeInterval = (1000000UL / frequency); /* In us from 50000 (20 hz) to 16 (64 kHz) */
	cli();
	/* set ADC */
	/* Clear control registers */
	ADMUX  = 0x00;
	ADCSRA = 0x00;
	ADCSRB = 0x00;
	if (_acqMode == DAC_ACQ_MOD_UINT8) {
		ADMUX |= (1 << ADLAR); /* Left aligned data */
	}
	else {
		ADMUX &= ~(1 << ADLAR); /* Right aligned data */
	}
	/* Reference voltage */
	ADMUX |= (_vRef << 6);
	/* Compute adc prescaler value automatically, allowed value are 2^1 to 2^7 */
	uint8_t adcPrescaler = (1 << 7); /* Allowed values are ranging from 2^1 to 2^7  */
	/* Decrease prescaler down to 16; for some reasons, decreasing below 16 drives to unaccurate timing */
	while ((adcPrescaler > uint16_t((timeInterval << 4) / 13)) && (adcPrescaler > 16)) {
		adcPrescaler >>= 1;
	}
	/* Set prescaler */
	uint8_t adcPrescalerExponent = Exponent(2, adcPrescaler);
	ADCSRA |= (adcPrescalerExponent >> 1);
	/* Enable ADC */
	ADCSRA |= (1 << ADEN);
	/* ADC Auto Trigger Enable */
	ADCSRA |= (1 << ADATE);
	/* ADC interrupt enable */
	ADCSRA |= (1 << ADIE);
	/* ADC Auto Trigger Source Selection */
	ADCSRB |= ((1 << ADTS0) | (1 << ADTS2));
	/* Set Timer1 */
	/* Reset Timer/Counter1 control registers and Interrupt Mask Register */
	TCCR1A = 0;
	TCCR1B = 0;
	TIMSK1 = 0;
	/* Set Clear Timer on Compare Match (CTC) Mode */
	TCCR1B |=  (1 << WGM12);
	/* Compute timer prescaler, allowed values are 1, 8 or 64 */
	uint8_t timPrescalerExponent = 0; /* Exponent of mantissa 8 */
	if (timeInterval >= 32768) {
		timPrescalerExponent = 2;
	}
	else if (timeInterval >= 4096) {
		timPrescalerExponent = 1;
	}
	/* Set prescaler  */
	TCCR1B |= (timPrescalerExponent + 1);
	/* Set upper count value: (F_CPU * Interval) / Prescaler */
	OCR1A = (16000000UL / (frequency * (1UL << (timPrescalerExponent * 8)))) - 1;
	/* Reset Timer/Counter2 */
	TIMSK2 = 0;
	/**/
	sei();
	StartAcquisitionEngine();
};

void PlainDAC::StartAcquisitionEngine(void)
{
/* Initialize and set Timer/Counter1  */
	TIMSK1 |=  (1 << OCIE1B); /* Enable timer interrupt  */
	_acqEngStatus == DAC_ACQ_ENG_STARTED;
};

void PlainDAC::StopAcquisitionEngine(void)
{
/* Reset Timer/Counter1  */
	TIMSK1 &= ~(1 << OCIE1B); /* Disable timer interrupt */
	_acqEngStatus == DAC_ACQ_ENG_STOPPED;
};

void PlainDAC::AcquireData(uint8_t *vData, uint8_t channel)
{
/* Acquire data from one specific channel */
	if (vData == NULL) {
		return;
	}
	if (_acqEngStatus == DAC_ACQ_ENG_STOPPED) {
		StartAcquisitionEngine();
	}
	/* Set channel */
	ADMUX &= ~0x0F; /* Clear all channel bits */
	ADMUX |= channel; /* Write channel bits */
	uint8_t originalTIMSK0 = TIMSK0; /* Record timer/counter0 mask */
	TIMSK0 = 0x00; /* Disable timer/counter0 */
	/* Reset number of acquired samples */
	uint16_t acquiredSamples = 0;
	int16_t lastAdcValue = 0; /* This variable is used for delta encoding mode */
	_dataAcqStatus = DAC_DAT_ACQ_WAITING;
	//PORTB &= ~(1 << cycleSyncPin); /* TODO: Comment in normal operation mode */
	/* Set acquisition status */
	do {
		if (_dataAcqStatus == DAC_DAT_ACQ_TRIGGERED) {
//			PORTB |=  (1 << adcSyncPin); /* TODO: Comment in normal operation mode */
			int16_t adcValue = ((_adcMSBs << 8) | _adcLSBs);
			/* Store data */
			switch(_acqMode) {
			case DAC_ACQ_MOD_UINT8: /* 8 bits mode */
				vData[acquiredSamples] = _adcMSBs;
				break;
			case DAC_ACQ_MOD_UINT16: /* 16 bits mode: data occupies only 10 bits */
				vData[(acquiredSamples << 1)] = _adcLSBs;
				vData[(acquiredSamples << 1) + 1] = _adcMSBs;
				break;
			case DAC_ACQ_MOD_DEL_ENCODING: /* Delta encoding mode */
				if (acquiredSamples == 0) {
					/* Store unsigned integer as a reference value */
					vData[0] = _adcLSBs;
					vData[1] = _adcMSBs;
				}
				else {
					int16_t delta = (adcValue - lastAdcValue);
					if ((delta < -maxDeltaValue) || (delta > maxDeltaValue)) {
						delta = (maxDeltaValue + 1); /* Record some sort of an error code */
					}
					vData[acquiredSamples + 1] = ((delta + maxDeltaValue) & 0xFF); /* Store unsigned byte, with offset half Byte value */
				}
				break;
			case DAC_ACQ_MOD_DOUBLE: /* 32 bits double mode */
				variant.dValue = (adcValue * 1.0); /* same as casting integer */
				memcpy((uint8_t*)&vData[(acquiredSamples << 2)], (uint8_t*)variant.vBuffer, sizeof(double)); /* Copy data */
				break;
			}
			lastAdcValue = adcValue;
			acquiredSamples++; /* Increment sample counts */
			_dataAcqStatus = DAC_DAT_ACQ_WAITING; /* Toggle status */
		}
	} while (acquiredSamples != _samples);
//	PORTB |=  (1 << cycleSyncPin); /* TODO: Comment in normal operation mode */
	_dataAcqStatus = DAC_DAT_ACQ_IDLE; /* Reset status */
	TIMSK0 = originalTIMSK0; /* Restore timer/counter0 operation */
};

int16_t PlainDAC::ReadIntegerData(uint8_t *vData, boolean firstOne)
{
/* Reads data from vector in sequential manner (no random access) all modes but Doubles */
	static uint16_t index;
	static int16_t lastAdcValue;
	int16_t adcValue = 0;
	if (firstOne) {
		index = 0; /* Reset index*/
	}
	else{
		index += 1; /* Increment index*/
	}
	switch (_acqMode) {
	case DAC_ACQ_MOD_UINT8:
		adcValue = vData[index];
		break;
	case DAC_ACQ_MOD_UINT16:
		adcValue = ((vData[(index << 1) + 1] << 8) | vData[(index << 1)]);
		break;
	case DAC_ACQ_MOD_DEL_ENCODING:
		if (firstOne) {
			/* Compute reference value */
			adcValue = ((vData[index + 1] << 8) | vData[index]);
		}
		else {
			/* Compute relative value */
			int16_t delta = UnsignedByteToSignedInt(vData[index + 1]);
			if (delta == 128) {
				adcValue = 0xFFFFFFFF; /* Some sort of error code */
			}
			else {
				adcValue = (lastAdcValue + delta);
			}
		}
		lastAdcValue = adcValue;
		break;
	}
	return(adcValue);
};

int16_t PlainDAC::ReadData(uint8_t *vData, uint16_t index)
{
/* Reads data from vector in random access mode all modes but Doubles */
	uint16_t adcValue = 0;
	switch(_acqMode) {
	case DAC_ACQ_MOD_UINT8:
		adcValue = vData[index];
		break;
	case DAC_ACQ_MOD_UINT16:
		adcValue = ((vData[(index << 1) + 1] << 8) | vData[(index << 1)]);
		break;
	case DAC_ACQ_MOD_DEL_ENCODING:
		int16_t lastAdcValue;
		for (uint16_t j = 0; j <= index; j++) {
			if (j == 0) {
				adcValue = ((vData[1] << 8) | vData[0]);
			}
			else {
				int16_t delta = (int16_t(vData[j + 1]) - maxDeltaValue);
				if (delta == 128) {
					adcValue = 0xFFFFFFFF;
				}
				else {
					adcValue = (lastAdcValue + delta);
				}
			}
			lastAdcValue = adcValue;
		}
		break;
	}
	return(adcValue);
};

double PlainDAC::ReadDoubleData(uint8_t *vData, uint16_t index)
{
/* Reads data from vector in random access mode */
	double adcValue = 0;
	if (_acqMode == DAC_ACQ_MOD_DOUBLE) {
		for (uint8_t j = 0; j < sizeof(double); j++) {
			variant.vBuffer[j] = vData[(index << 2) + j];
		}
		adcValue = variant.dValue;
	}
	return(adcValue);
};

uint8_t PlainDAC::SignedIntToUnsignedByte(int16_t signedIntValue)
{
/* Converts a signed 2 bytes integer into an unsigned byte */
	uint8_t result = 0;
	if ((signedIntValue < -maxDeltaValue) || (signedIntValue > maxDeltaValue)) {
		result = maxDeltaValue + 1; /* Set some sort of an error code */
	}
	else {
		result = (signedIntValue + maxDeltaValue);
	}
	return(result);
};

int16_t PlainDAC::UnsignedByteToSignedInt(uint8_t unsignedByteValue)
{
/* Converts an unsigned byte in a signed 2 bytes integer */
	int16_t result = int16_t(unsignedByteValue) - maxDeltaValue;
	return(result);
};

/* Private functions */

ISR(TIMER1_COMPB_vect)
{
/* Invoked on completion of counting (Compare mode) */
	/* Do not ask me why, the presence of this function is mandatory!!! */
};

ISR(ADC_vect)
{
/* Invoked on completion of conversions */
//	PORTB &= ~(1 << adcSyncPin); /* TODO: Comment in normal operation mode */
	if (_dataAcqStatus == DAC_DAT_ACQ_WAITING) {
		/* When both LSBs and MSBs are to be read, LSBs MUST be read first */
		if (_acqMode != DAC_ACQ_MOD_UINT8) {
			_adcLSBs = ADCL;
		}
		_adcMSBs = ADCH;
		_dataAcqStatus = DAC_DAT_ACQ_TRIGGERED;
	}
};

uint8_t PlainDAC::Exponent(uint8_t mantissa, uint16_t x)
{
/* Compute the Exponent of a powered value  */
/* Same as Exponent = Ln(x)/Ln(mantissa) but quick and dirty */
	uint8_t exp = 0;
	uint16_t result = 1;
	while (x != result) {
		result *= mantissa;
		exp++;
	}
	/* Returned value */
	return (exp);
};
