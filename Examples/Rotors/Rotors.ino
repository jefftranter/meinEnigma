/*
  MeinEnigma Example

  Demonstrates reading the rotors.

  This uses the encoder library and was adapted from the basic example
  at http://www.pjrc.com/teensy/td_libs_Encoder.html

  Jeff Tranter <tranter@pobox.com.

*/

#define ENCODER_USE_INTERRUPTS
//#define ENCODER_DO_NOT_USE_INTERRUPTS

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
// Avoid using pins with LEDs attached

Encoder rotor1(3, 2);
Encoder rotor2(4, 5);
Encoder rotor3(6, 7);
Encoder rotor4(10, 11);

void setup() {
  Serial.begin(9600);
  Serial.println("Rotor Demonstration");
}

long oldPosition1  = -999;
long oldPosition2  = -999;
long oldPosition3  = -999;
long oldPosition4  = -999;

void loop() {
  long newPosition1 = rotor1.read();
  long newPosition2 = rotor2.read();
  long newPosition3 = rotor3.read();
  long newPosition4 = rotor4.read();
      
  if (newPosition1 != oldPosition1) {
    oldPosition1 = newPosition1;
    Serial.print("Position1 = ");
    Serial.println(newPosition1);
  }
  if (newPosition2 != oldPosition2) {
    oldPosition2 = newPosition2;
    Serial.print("Position2 = ");
    Serial.println(newPosition2);
  }
  if (newPosition3 != oldPosition3) {
    oldPosition3 = newPosition3;
    Serial.print("Position3 = ");
    Serial.println(newPosition3);
  }
  if (newPosition4 != oldPosition4) {
    oldPosition4 = newPosition4;
    Serial.print("Position4 = ");
    Serial.println(newPosition4);
  }
  delay(20);
}
