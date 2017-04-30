/*
  MeinEnigma Example

  Demonstrates controlling the 4 alphanumeric LEDs on the main board.

  Uses code from the MeinEnigma software.

  Jeff Tranter <tranter@pobox.com.

*/

// External library for HT16K33 chip.
#include "ht16k33.h"

// LED font table
#include "asciifont-pinout12.h"

HT16K33 HT;

// Display a character on one of the displays. Leftmost display is 0,
// rightmost 3.
void displayLetter(char letter, uint8_t dispeno) {
  uint8_t led;
  int8_t i;
  uint16_t val;

  led = (dispeno) * 16;
  if (letter > '`')
    letter -= ('a' - 'A');
  val = pgm_read_word(fontTable + letter - ' ');

  // No lookup table needed, all LEDs are at offset 64
  HT.setDisplayRaw(dispeno * 2 + 64 / 8, val & 0xFF);
  HT.setDisplayRaw(dispeno *2 + 64 / 8 + 1, val>>8);
  HT.sendLed();
}

void setup() {
  // Need to initialize the chip in order for displays to
  // work. This also clears all display segments and LEDs.
  HT.begin(0x00);

  // Clear the RAM to make sure we read data but don't send it.
  for (int i = 0; i < sizeof(HT.displayRam); i++)
    HT.displayRam[i] = 0;
}

void loop() {
  int i;

  // All segments on
  for (i = 64; i <= 127; i++) {
    HT.setLedNow(i);
    delay(50);
  }

  // All segments off
  for (i = 64; i <= 127; i++) {
    HT.clearLedNow(i);
    delay(50);
  }

  // Display some characters
  displayLetter('A', 0);
  displayLetter('B', 1);
  displayLetter('C', 2);
  displayLetter('D', 3);
  delay(1000);

  // Display full character set.
  int j = 0;
  for (char c = ' '; c <= '`'; c++) {
    displayLetter(c, j % 4);
    j++;
    delay(200);
  }
}
