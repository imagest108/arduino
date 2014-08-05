const int analogPin= 0;
const int ledPin = 9;
int brightness = 0; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int mn = 1024;
  int mx = 0;
  
  for(int i = 0; i <1000; ++i){
     int val = analogRead(analogPin);
     //Serial.println(val);
      mn = min(mn, val);
      mx = max(mx, val); 
      brightness = val /4;       //divide by 4 to fit in a byte
     analogWrite(ledPin, brightness);   // PWM the LED with the brightness value 
  }
  Serial.print("m=");
  Serial.print(mn);
  Serial.print(" M=");
  Serial.print(mx);
  Serial.print(" D=");
  Serial.print(mx-mn);
  Serial.println();
    //delay(300);
}


