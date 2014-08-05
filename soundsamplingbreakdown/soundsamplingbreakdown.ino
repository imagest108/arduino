    
    #include "PlainDAC.h" 
    
    PlainDAC DAC = PlainDAC(); /* Create DAC object */
    
    const uint16_t samplingFrequency = 8000; /* Set default sampling frequency value */
    const uint8_t acqMode = DAC_ACQ_MOD_DOUBLE; /* Set default acquisition mode value */
    const uint16_t samples = 128; /* Set default samples value */
    const uint8_t channel = 0; /* From 0 to 5 on Diecimila */
    const uint8_t threshold = 0; //need to change 
    
    const uint8_t vRef = DAC_REF_VOL_DEFAULT; /* Set default voltage reference value */
    
    void setup() {
      // put your setup code here, to run once:
      
      Serial.begin(115200);
      Serial.println("Ready");
      DAC.SetAcquisitionParameters(samplingFrequency, samples, acqMode, vRef);
      DAC.StartAcquisitionEngine();
      DAC.InitDataVector(&vData);
      
    }
    
    void loop() {
      // put your main code here, to run repeatedly:
      
    }
