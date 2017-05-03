/*
  MeinEnigma Example

  Demonstrates controlling the decimal points on the 4-digit
  alphanumeric LED display. These are controlled differently from the
  other display segments.

  Jeff Tranter <tranter@pobox.com.

*/

// External library for HT16K33 chip.
#include "ht16k33.h"

void setup() {
  HT16K33 HT;

  // Set the pins used for output. These happen to be analog outputs
  // but are only used at digital levels.
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  // Need to initialize the chip in order for the decimal points to
  // work. This also clears all display segments.
  HT.begin(0x00);
}

// A0 controls the leftmost digit decimal point. A3 controls the
// rightmost. Setting an output to 1 (high) turns the associated
// decimal point off. Setting an output to 0 (low) turns the decimal
// point on.
void loop() {

#if 1
  // Make the decimal points count from 0 to 15 in binary.
  for (int i = 0; i < 16; i++) {
    digitalWrite(A0, !(i & 8));
    digitalWrite(A1, !(i & 4));
    digitalWrite(A2, !(i & 2));
    digitalWrite(A3, !(i & 1));
    delay(200);
  }
#endif

#if 0
  // Alternative pattern. Walk decimal points from left to right.
  for (int i = 0; i <= 4; i++) {
    digitalWrite(A0, !(i == 1));
    digitalWrite(A1, !(i == 2));
    digitalWrite(A2, !(i == 3));
    digitalWrite(A3, !(i == 4));
    delay(200);
  }
#endif
}
