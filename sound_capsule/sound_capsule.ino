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
 
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorVal = 0;  // variable to store the value coming from the sensor

//int redPin = 8;
//int greenPin = 10;
//int bluePin = 11;
//int groundPin = 9;
int ledPin = 11;
int ledGround = 10;

void setup(){
  //start serial connection
  Serial.begin(9600);
  //configure pin2 as an input and enable the internal pull-up resistor
  //pinMode(2, INPUT_PULLUP);
  pinMode(3, OUTPUT); 
//  pinMode(redPin, OUTPUT);
//  pinMode(greenPin, OUTPUT);
//  pinMode(bluePin, OUTPUT);
//  pinMode(groundPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledGround, OUTPUT);

}

void loop(){
  
  digitalWrite(ledGround, LOW);  
  //read the pushbutton value into a variable
  sensorVal = analogRead(sensorPin);
  //print out the value of the pushbutton
  Serial.println(sensorVal);
  
  
  // Keep in mind the pullup means the pushbutton's
  // logic is inverted. It goes HIGH when it's open,
  // and LOW when it's pressed. Turn on pin 13 when the 
  // button's pressed, and off when it's not:
  if (sensorVal < 250) {
    digitalWrite(3, HIGH);
    digitalWrite(ledPin, LOW);
    //setColor(80, 0, 80); 
  } 
  else {
    digitalWrite(3, LOW);
    digitalWrite(ledPin, HIGH);
    //setColor(0, 255, 0); 
    //delay(1000);
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
