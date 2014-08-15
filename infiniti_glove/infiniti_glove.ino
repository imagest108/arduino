int ledPin[] ={11,3,9,6}; 
int pulsePin = 1;
int blinkPin = 2;
int fadePin = 4;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

int currentStatus = 0;

// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, must be seeded! 
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

bool sensingMode = false;

void setup() {

  Serial.begin(57600);
  ledBegining();
    
   
  if(sensingMode) {
    pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
    pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
    interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
  }
}

void loop() {
  
 if(sensingMode){
    if (QS == true) { 
        fadeRate = 255;     
        Serial.print("BPM : ");
        Serial.println(BPM); 
        QS = false;
    }
    
    ledFadeToBeat();
    delay(20); 
  }
  
    // put your main code here, to run repeatedly:
    if(Serial.available() > 0){
      currentStatus = Serial.parseInt();
      ledAnimation(currentStatus);  
    } 
  
}

void ledAnimation(int statusNum){
 
  clearLed();
 
  for (int fadeValue = 0 ; fadeValue <= 150; fadeValue += 10) {
  // sets the value (range from 0 to 255):
    for(int i = 0; i < statusNum ; i++){
      analogWrite(ledPin[i], fadeValue);
      delay(30); 
//      Serial.print("go up: ");
//      Serial.println(fadeValue);
    }
  }
  for (int fadeValue = 150 ; fadeValue >= 0; fadeValue -= 10) {
  // sets the value (range from 0 to 255):
    for(int i = 0; i < statusNum ; i++){
      analogWrite(ledPin[i], fadeValue);
      delay(30); 
//      Serial.print("go down: ");
//      Serial.println(fadeValue);  
    }
  }
}

void clearLed(){
  for(int i = 0; i < sizeof(ledPin) ; i++){
       analogWrite(ledPin[i], 0);
  }
}

void ledBegining(){
  for(int i = 0; i < sizeof(ledPin) ; i++){
       analogWrite(ledPin[i], 255);
       delay(50);
       analogWrite(ledPin[i], 0);    
  }
  for(int i = 0; i < sizeof(ledPin) ; i++){
       analogWrite(ledPin[i], 255);
       delay(50);
       analogWrite(ledPin[i], 0);    
  }
    for(int i = 0; i < sizeof(ledPin) ; i++){
       analogWrite(ledPin[i], 255);
       delay(50);
       analogWrite(ledPin[i], 0);    
  }
}


void ledFadeToBeat(){
    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
}

