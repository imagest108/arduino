  #include "pitches.h"

//void setup() { 
//  Serial.begin(9600);       // initialize serial communications
//} 


// notes in the melody:
 int melody[] = {
   NOTE_C4, NOTE_G3,NOTE_G3, NOTE_GS3, NOTE_G3,0, NOTE_B3, NOTE_C4};

 // note durations: 4 = quarter note, 8 = eighth note, etc.:
 int noteDurations[] = {
   4, 8, 8, 4,4,4,4,4 };

 void setup() {
   // iterate over the notes of the melody:
//   for (int thisNote = 0; thisNote < 8; thisNote++) {
//
//     // to calculate the note duration, take one second 
//     // divided by the note type.
//     //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//     int noteDuration = 1000/noteDurations[thisNote];
//     tone(8, melody[thisNote],noteDuration);
//
//     //pause for the note's duration plus 30 ms:
//     delay(noteDuration +30);

//   }
    Serial.begin(9600);       // initialize serial communications
 }


void loop() 
{ 
  int sensorReading = analogRead(A1); // read the analog input 
  Serial.println();      // print it
  float frequency = map(sensorReading, 200, 900, 100, 1000);
   // change the pitch, play for 10 ms:
  tone(8, frequency, 10);
  
  //Serial.begin(9600);       // initialize serial communications

}
