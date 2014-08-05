/*
 Input Pullup Serial
 
 This example demonstrates the use of pinMode(INPUT_PULLUP). It reads a 
 digital input on pin 2 and prints the results to the serial monitor.
 
 The circuit: 
 * Momentary switch attached from pin 2 to ground 
 * Built-in LED on pin 13
 
 Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal 
 20K-ohm resistor is pulled to 5V. This configuration causes the input to 
 read HIGH when the switch is open, and LOW when it is closed. 
 
 created 14 March 2012
 by Scott Fitzgerald
 
 http://www.arduino.cc/en/Tutorial/InputPullupSerial
 
 This example code is in the public domain
 
 */
 
int playStatus = 0;

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorVal = 0;  // variable to store the value coming from the sensor

int powerPin = 2;
int photocellGroundPin = 5;
int photocellPowerPin = 6;
//int groundPin = 9;
//int ledPin = 11;
//int ledGround = 10;

//int recordingButton = 8;
//int playButton = 7;
int playPin = 10;
int playGround = 11;


void setup(){
  //start serial connection
  Serial.begin(9600);
  //configure pin2 as an input and enable the internal pull-up resistor
  //pinMode(2, INPUT_PULLUP);
  //pinMode(3, OUTPUT); 
  pinMode(powerPin, OUTPUT);
  pinMode(photocellGroundPin, OUTPUT);
  pinMode(photocellPowerPin, OUTPUT);

  pinMode(playPin, OUTPUT);
  pinMode(playGround, OUTPUT);
  
//  pinMode(playButton, OUTPUT);
//  pinMode(recordingButton, OUTPUT);
  
//  for(int i = 0; i < 14 ; i++){
//   digitalWrite(i, HIGH); 
//  }
  

}

void loop(){
  
  digitalWrite(powerPin, HIGH);  
  digitalWrite(photocellGroundPin, LOW);  
  digitalWrite(photocellPowerPin, HIGH);  
  //read the pushbutton value into a variable
  sensorVal = analogRead(sensorPin);
  //print out the value of the pushbutton
  
  //if(sensorVal > 250) digitalWrite(playButton, LOW);
  
  /*
  
  if(sensorVal > 250 && playStatus == 0){
//   Serial.print(sensorVal);
//   Serial.print(" : ");
    Serial.println("play"); 
   digitalWrite(playButton, LOW);
   playStatus = 1;
  }else if(playStatus != 0) {
    playStatus += 1;
    
    if(playStatus > 50){
      playStatus = 0;
    }else{
      digitalWrite(playButton, LOW);
    }
    digitalWrite(playButton, HIGH);
  } 
  
 //digitalWrite(playButton, LOW); 
 Serial.println(playStatus);
  delay(200);
  
  // Keep in mind the pullup means the pushbutton's
  // logic is inverted. It goes HIGH when it's open,
  // and LOW when it's pressed. Turn on pin 13 when the 
  // button's pressed, and off when it's not:
*/
  if (sensorVal > 250) {
      analogWrite(playPin, 255);
      analogWrite(playGround, 0);
  } 
  else {
      analogWrite(playPin, 0);
      analogWrite(playGround, 255);
  }
  
  
  
}

/*
void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
  digitalWrite(groundPin, HIGH); 
}*/
