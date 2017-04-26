/*
  MeinEnigma Example

  Demonstrates controlling the 26 discrete LEDs on the lamp and key
  board.

  Jeff Tranter <tranter@pobox.com.

*/

// External library for HT16K33 chip.
#include "ht16k33.h"

HT16K33 HT;

// Lookup table of LEDs.
byte ledTable[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 28, 29, 30, 31, 19, 20, 21, 22, 23, 24, 25, 26, 27 };

void setup() {
  // Need to initialize the chip in order for displays to
  // work. This also clears all display segments and LEDs.
  HT.begin(0x00);
}

void loop() {
  int i;

  // All on
  for (i = 0; i < sizeof(ledTable); i++) {
    HT.setLed(ledTable[i]);
  }
  HT.sendLed();
  delay(1000);
   
  // All off
  for (i = 0; i < sizeof(ledTable); i++) {
    HT.clearLed(ledTable[i]);
  }
  HT.sendLed();
  delay(1000);

  // Walk all LEDs on
  for (i = 0; i < sizeof(ledTable); i++) {
    HT.setLedNow(ledTable[i]);
    delay(100);
  }
  
  // Walk all LEDs off;
  for (i = 0; i < sizeof(ledTable); i++) {
    HT.clearLedNow(ledTable[i]);
    delay(100);
  }
  
  // Walk one LED on
  for (i = 0; i < sizeof(ledTable); i++) {
    HT.setLedNow(ledTable[i]);
    delay(100);
    HT.clearLedNow(ledTable[i]);
  }

  // All on
  for (i = 0; i < sizeof(ledTable); i++) {
    HT.setLed(ledTable[i]);
  }
  HT.sendLed();
  
  // Walk one LED off
  for (i = 0; i < sizeof(ledTable); i++) {
    HT.clearLedNow(ledTable[i]);
    delay(100);
    HT.setLedNow(ledTable[i]);
  }
 
   // All off
  for (i = 0; i < sizeof(ledTable); i++) {
    HT.clearLed(ledTable[i]);
  }
  HT.sendLed();
  
  // Random
  for (int j=0; j < 500; j++ ) {
    i = random(sizeof(ledTable));
    if (random(2) == 1) {
      HT.setLedNow(ledTable[i]);
    } else{
      HT.clearLedNow(ledTable[i]);
    }
    delay(20);
  }
}

