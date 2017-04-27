/*
  MeinEnigma Example

  Demonstrates reading the keyboard.
  Uses code from the MeinEnigma software.

  Jeff Tranter <tranter@pobox.com.

*/

// External library for HT16K33 chip.
#include "ht16k33.h"

// Lookup table of ley codes to characters.
const char keys[] = { 'Q','W','E','R','T','Z','U','I','O','A','S','D','F','P','Y','X','C','V','B','N','M','L','G','H','J','K','1','2','3','4' };
                      
HT16K33 HT;

void setup() {
  Serial.begin(9600); // Initialize serial port for debug output.
  HT.begin(0x00);     // Need to initialize the chip in order for keyboard to work.
  Serial.println("Keyboard Demo");
}

void loop() {
  int k;

  k = HT.readKey();
  if (k != 0) {
    Serial.print("Key ");
    Serial.print(keys[abs(k)-1]);
    Serial.print(" ");
    if (k > 0) {
      Serial.println("pressed");
    } else
      Serial.println("released");
  }

  delay(100);
}


