int testPin = 10;
int testGround = 11;

int recordTestPin = 9;
int recordTestGround = 8;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(testPin, OUTPUT);
  pinMode(testGround, OUTPUT);
  pinMode(recordTestPin, OUTPUT);
  pinMode(recordTestGround, OUTPUT);

}

void loop() {
  digitalWrite(recordTestGround, LOW);
  digitalWrite(recordTestPin, HIGH);
  //digitalWrite(testGround, LOW);
//  analogWrite(testPin, 255);
//  // put your main code here, to run repeatedly:
////  digitalWrite(testPin, HIGH);
////  digitalWrite(testPin, LOW);
////  delay(2000);
//  int value = digitalRead(testPin);
//  Serial.println(value);

if (Serial.available() > 0) {
    int inByte = Serial.read();
    // do something different depending on the character received.  
    // The switch statement expects single number values for each case;
    // in this exmaple, though, you're using single quotes to tell
    // the controller to get the ASCII value for the character.  For 
    // example 'a' = 97, 'b' = 98, and so forth:

    switch (inByte) {
    case 'a':    
      analogWrite(testPin, 255);
      analogWrite(testGround, 0);
      Serial.println("play");
      break;
      
     case 's':
      //for(int i = 0 ; i < 500 ; i++){    
        digitalWrite(recordTestPin, HIGH);
        digitalWrite(recordTestPin, HIGH);
        digitalWrite(recordTestPin, HIGH);        
        digitalWrite(recordTestPin, HIGH);
        digitalWrite(recordTestPin, HIGH);
        
        //analogWrite(recordTestGround, 0);
        //Serial.print(i);
        //Serial.print(" : ");
        Serial.println("record");
      //};
      break;
    
     case 'd':    
      analogWrite(recordTestPin, 0);
      analogWrite(recordTestGround, 255);
      Serial.println("record stop");
      break; 
      

    default:
      // turn all the LEDs off:
      for (int thisPin = 2; thisPin < 7; thisPin++) {
        //digitalWrite(thisPin, LOW);
      analogWrite(testPin, 0);
      analogWrite(testGround, 255);
      analogWrite(recordTestPin, 0);
      analogWrite(recordTestGround, 0);
      }
    } 
  }


}
