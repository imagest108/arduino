int ledPin[] ={11,9,6,5}; 

int currentStatus = 4;


bool sensingMode = false;

void setup() {

  Serial.begin(57600);
  ledBegining();
    
 /*  
  if(sensingMode) {
    pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
    pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
    //interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
  }
  */
}

void loop() {
/*  
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
*/  
//  // put your main code here, to run repeatedly:
//  if(Serial.available()){
//    currentStatus = Serial.parseInt();
//  } 
  
  ledAnimation(currentStatus);  
  
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
