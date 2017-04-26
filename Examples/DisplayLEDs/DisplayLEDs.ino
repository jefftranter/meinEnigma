/*
  MeinEnigma Example

  Demonstrates controlling the 4 alphanumeric LEDs on the main board.

  Jeff Tranter <tranter@pobox.com.

*/

// External library for HT16K33 chip.
#include "ht16k33.h"

HT16K33 HT;

void setup() {
  // Need to initialize the chip in order for displays to
  // work. This also clears all display segments and LEDs.
  HT.begin(0x00);
}

void loop() {
  int i;

  // All on
  for (i = 64; i <= 127; i++) {
    HT.setLedNow(i);
    delay(100);
  }
  delay(1000);

  // All off
  for (i = 64; i <= 127; i++) {
    HT.clearLedNow(i);
    delay(100);
  }

}

