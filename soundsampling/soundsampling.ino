  
  #include "PlainDAC.h"
  #include "PlainFFT.h"
  
  PlainFFT FFT = PlainFFT(); /* Create FFT object */
  PlainDAC DAC = PlainDAC(); /* Create DAC object */
  
  const uint16_t samplingFrequency = 8000; /* Set default sampling frequency value */
  const uint8_t acqMode = DAC_ACQ_MOD_DOUBLE; /* Set default acquisition mode value */
  const uint16_t samples = 128; /* Set default samples value */
  const uint8_t channel = 0; /* From 0 to 5 on Diecimila */
  const uint8_t threshold = 0; //need to change 
  
  const uint8_t vRef = DAC_REF_VOL_DEFAULT; /* Set default voltage reference value */
  
  //int ledPin = 9;                 // LED connected to digital pin 13
  
  uint8_t vRefSpectrum[(samples >> 1)];
  uint8_t vActSpectrum[(samples >> 1)];
  
  void setup() {
    
    Serial.begin(115200);
    Serial.println("Ready");
    
    /* Set acquisition parameters */  
    DAC.SetAcquisitionParameters(samplingFrequency, samples, acqMode, vRef);
    DAC.StartAcquisitionEngine();
    
    //pinMode(ledPin, OUTPUT);
  }
  
  void loop() {
    
    /* Initialize data vector */
    uint8_t *vData = (uint8_t*)malloc(samples * sizeof(double)); 
    /* Acquire data in bytes array */
    DAC.AcquireData(vData, channel);
    /* Reallocate memory space to doubles array for real values */
    double *vReal = (double*)realloc(vData, samples * sizeof(double));
    /* Adjust signal level */
    for (uint16_t i = 0; i < samples; i++) {
    	vReal[i] = (((vReal[i] * 5.0) / 1024.0) - 2.5);
    }
    
    /* Allocate memory space to doubles array for imaginary values (all 0ed) */
    double *vImag = (double*)calloc(samples, sizeof(double)); 
    /* Weigh data */
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_RECTANGLE, FFT_FORWARD); 
    /* Compute FFT */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); 
    /* Compute magnitudes */
    FFT.ComplexToMagnitude(vReal, vImag, samples);
    

    
    /* Find max value */
    double max = 0;
    for (uint16_t i = 1; i < ((samples >> 1) - 1); i++) {
    	if (vReal[i] > max) max = vReal[i];
    }
    
    /* Clean, normalize and store data in actual spectrum */
    for (uint16_t i = 1; i < ((samples >> 1) - 1); i++) {
    	if ((vReal[i-1] < vReal[i]) && (vReal[i] > vReal[i+1]) && (uint8_t(vReal[i] * 100.0 / max) > threshold)) {
    		vActSpectrum[i] = uint8_t((vReal[i] * 255.0) / max);
    	}
    	else {
    		vActSpectrum[i] = 0;
    	}
    }
    
    if (matchSpectra() > 0x00) {
    	/* Turn status led on for some milliseconds */
    	PORTB |= (1 << PINB5);
    	//printSpectra(vRefSpectrum, vActSpectrum);
    
        	for (uint16_t i = 0; i < vRefSpectrum; i++) {
		double abscissa;
		/* Print abscissa value */
		switch (vActSpectrum) {
		case SCL_INDEX:
			abscissa = (i * 1.0);
			break;
		case SCL_TIME:
			abscissa = ((i * 1.0) / samplingFrequency);
			break;
		case SCL_FREQUENCY:
			abscissa = ((i * 1.0 * samplingFrequency) / samples);
			break;
		}
		Serial.print(abscissa, 6);
		Serial.print(" ");
		Serial.print(vData[i], 4);
		Serial.println();
	}
    
//    
//        	Serial.print(vRefSpectrum, 6);
//		Serial.print(" ");
//		Serial.print(vActSpectrum, 4);
//		Serial.println();
    	delay(5000);
    	PORTB &= ~(1 << PINB5);
    }
  
  }
  
  uint8_t matchSpectra(void) {
/* Compute the match criteria between the reference spectrum and the actual spectrum 
	 The result is expressed in percent and ranges strictly from 0% to 100% */
	uint16_t sumOfRefOrAct = 0;
	uint16_t sumOfAbsDiff = 0;
	for (uint16_t i = 1; i < ((samples >> 1) - 1); i++) {
		/* Compute absolute differences between reference and actual spectra */
		uint8_t diff;
		if (vRefSpectrum[i] > vActSpectrum[i]){
			diff = (vRefSpectrum[i] - vActSpectrum[i]);
			sumOfRefOrAct += vRefSpectrum[i];
		}
		else if (vActSpectrum[i] > vRefSpectrum[i]){
			diff = (vActSpectrum[i] - vRefSpectrum[i]);
			sumOfRefOrAct += vActSpectrum[i];			
		} 
		else {
			diff = 0;
			sumOfRefOrAct += vRefSpectrum[i];
		}
		sumOfAbsDiff += diff;
	}
	if (sumOfRefOrAct != 0x00) {
		/* Returns the matching value in pct */
		return(uint8_t(((sumOfRefOrAct - sumOfAbsDiff) * 100.0) / sumOfRefOrAct));
	}
	else {
		/* Reference spectrum not set */
		return(0x00);
	}
}


