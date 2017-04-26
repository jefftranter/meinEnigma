/*
  MeinEnigma Example

  Demonstrates controlling the 4 alphanumeric LEDs on the main board.

  Jeff Tranter <tranter@pobox.com.

*/

// External library for HT16K33 chip.
#include "ht16k33.h"

// LED font table
#include "asciifont-pinout12.h"

HT16K33 HT;

// Display something on one of the "wheels".
// Rightmost wheel is 3
// Leftmost is 0.
void displayLetter(char letter, uint8_t walzeno) {
  uint8_t led;
  int8_t i;
  uint16_t val;

  led = (walzeno) * 16;
  if (letter>'`')
    letter-=('a'-'A');
  val = pgm_read_word(fontTable+letter-' '); // A= 0b1111001111000000 - 0xF3C0

  // No lookup table needed, all LEDs are at offset 64
  HT.setDisplayRaw(walzeno*2+64/8,val & 0xFF);
  HT.setDisplayRaw(walzeno*2+64/8+1,val>>8);
  HT.sendLed();
}

void setup() {
  // Need to initialize the chip in order for displays to
  // work. This also clears all display segments and LEDs.
  HT.begin(0x00);

  // Clear the ram to make sure we read data but don't send it!
  for (int i = 0; i < sizeof(HT.displayRam); i++)
    HT.displayRam[i] = 0;
}

void loop() {
  int i;

  // All on
  for (i = 64; i <= 127; i++) {
    HT.setLedNow(i);
    delay(50);
  }

  // All off
  for (i = 64; i <= 127; i++) {
    HT.clearLedNow(i);
    delay(50);
  }

  displayLetter('A', 0);
  displayLetter('B', 1);
  displayLetter('C', 2);
  displayLetter('D', 3);
  delay(1000);

  int j = 0;
  for (char c = ' '; c <= '`'; c++) {
    displayLetter(c, j % 4);
    j++;
    delay(200);
  }
}

