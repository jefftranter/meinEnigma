/*
  MeinEnigma Example

  Basic clock application with alarm.

  Jeff Tranter <tranter@pobox.com.

Feature Description
-------------------

Time display, 12 hour mode: 100 through 1159 with rightmost decimal
point indicating pm.

Time Display, 24 hour mode: 0000 through 2359.

Second decimal point will toggle at a one second on/one second off
rate whenever the time is displayed.

Time set: pressing four keys below display will perform as follows
(left to right):
- advance one hour
- go back one hour
- advance one minute
- go back one minute

Chime: When enabled, will beep the time on the hour using the buzzer.
Will beep once for every hour, from 1 through 12 (independent of 12/24
hour mode).

Alarm: When enabled, buzzer will sound at one second on/one second off
rate until any key is pressed.

Leftmost decimal point indicates alarm on.

Keyboard keys:

D - Display date briefly, e.g "JA 1" "2017" or "DE24" "2018". Shows
month and day, then year, then goes back to time mode.

C - Toggle chime. Briefly Display "CH Y" or "CH N", then goes back to
time mode.

M - Toggle between 12 and 24 hour mode. Briefly display "12HR" or
"24HR", then go back to time mode.

A - Toggle alarm on/off. Briefly display "AL N" or AL Y", then go back
to time mode. Leftmost decimal point goes on when alarm is enabled.

S - Set alarm. Pressing keys under display will set alarm time,
similar to time set. Pressing S again will exit alarm set mode.
Leave alarm set mode if no key pressed for more than 5 seconds.

T - Set date. Pressing keys under display will set month and day,
similar to time set. Pressing T again will exit date set mode.
Leave date mode if no key pressed for more than 5 seconds.

*/

// Wire library for i2c.
#include <Wire.h>

// External library for HT16K33 chip.
#include "ht16k33.h"

// LED font table
#include "asciifont-pinout12.h"

// I2C address of DS32310 real-time clock.
#define DS3231_ADDR 0x68

// Lookup table of key codes to characters.
const char keys[] = { 'Q','W','E','R','T','Z','U','I','O','A','S','D','F','P','Y','X','C','V','B','N','M','L','G','H','J','K','1','2','3','4' };

// Lookup table of decimal point output pins.
int decimalPoint[4] = { A0, A1, A2, A3 };

// Lookup tables for names of the months of the year.
const char *monthName[] = { "JA", "FE", "MA", "AP", "MA", "JU", "JU", "AU", "SE", "OC", "NO", "DE" };

// Read a byte from a specific I2C address. Send one byte (address to read) and read a byte.
uint8_t i2c_read(uint8_t unitaddr, uint8_t addr) {
  i2c_write(unitaddr, addr);
  Wire.requestFrom(unitaddr, (uint8_t)1);
  return Wire.read();    // Read one byte
}

// Write a single byte to I2C.
uint8_t i2c_write(uint8_t unitaddr, uint8_t val) {
  Wire.beginTransmission(unitaddr);
  Wire.write(val);
  return Wire.endTransmission();
}


// Write two bytes to i2c.
uint8_t i2c_write2(uint8_t unitaddr, uint8_t val1, uint8_t val2) {
  Wire.beginTransmission(unitaddr);
  Wire.write(val1);
  Wire.write(val2);
  return Wire.endTransmission();
}


// Object instance for HT16K33 chip.
HT16K33 HT;

// Mode flags;
bool twentyFourHourMode = false;
bool alarmEnabled = false;
bool charmEnabled = false;


// State flags;
bool alarmOn = false;
bool chiming = false;
bool colon = false;
bool alarmSetMode = false; // These two are mutually exclusive.
bool dateSetMode = false;

// Current date/time
int hour, minute, second; // These are all stored in BCD.
int year, month, day;
int lastHour; // To determine when hour rolls over.

// Alarm time.
int alarmHours, alarmMinutes;


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


// Get current time from RTC.
void getTime()
{
  year   = i2c_read(DS3231_ADDR, 6);
  month  = i2c_read(DS3231_ADDR, 5);
  day    = i2c_read(DS3231_ADDR, 4);
  hour   = i2c_read(DS3231_ADDR, 2 );
  minute = i2c_read(DS3231_ADDR, 1);
  second = i2c_read(DS3231_ADDR, 0);
}


// Write current time to RTC.
void setTime()
{
  i2c_write2(DS3231_ADDR, 6, year);
  i2c_write2(DS3231_ADDR, 5, month);
  i2c_write2(DS3231_ADDR, 4, day);
  i2c_write2(DS3231_ADDR, 2, hour);
  i2c_write2(DS3231_ADDR, 1, minute);
  i2c_write2(DS3231_ADDR, 0, second);
}


// Set a decimal point on display (0-3) on or off.
void setDecimalPoint(int display, bool state=true)
{
  digitalWrite(decimalPoint[display], !state);
}


// Display time.
void displayTime()
{
  char str[4];

  if (twentyFourHourMode) {
    str[0] = '0' + hour / 16;
    str[1] = '0' + hour % 16;
    str[2] = '0' + minute / 16;
    str[3] = '0' + minute % 16;
    printDisplay(str);
    setDecimalPoint(3, false);
  } else {
    int tmpHour = hour % 12; // Calculate 12 hour time
    if (tmpHour == 0) {
      tmpHour = 12;
    }
    str[0] = '0' + tmpHour/ 16;
    if (str[0] == '0') {
      str[0] = ' '; // Suppress leading zero.
    }
    str[1] = '0' + tmpHour % 16;
    str[2] = '0' + minute / 16;
    str[3] = '0' + minute % 16;
    printDisplay(str);
    if (hour >= 12) { // am/pm indicator.
      setDecimalPoint(3, true);
    } else {
      setDecimalPoint(3, false);
    }
  }
}


// Display date.
void displayDate()
{
}


// Show four characters on display.
void printDisplay(const char str[4])
{
  for (int i = 0; i < 4; i++) {
    displayLetter(str[i], i);
  }
}


// Get a keyboard key. Does not block. Returns an ASCII character or
// \0 if no key pressed. Does not report key releases.
char getKey()
{
  int key = HT.readKey();

  if (key <= 0) {
    return 0; // Key released or no key pressed.
  } else{     
    return keys[abs(key) - 1];
  }
}

void setup() {
  // Set the pins used for decimal point output. These happen to be
  // analog outputs but are only used at digital levels.
  for (int i = 0; i < 4; i++) {
      pinMode(decimalPoint[i], OUTPUT);
  }

  // Initialize serial port for debug output.
    Serial.begin(9600);

  // Need to initialize the HT chip in order for displays to work.
  // This also clears all display segments and LEDs.
  HT.begin(0x00);

  // Clear the RAM to make sure we read data but don't send it.
  for (int i = 0; i < sizeof(HT.displayRam); i++) {
    HT.displayRam[i] = 0;
  }

  // Turn off decimal points.
  for (int i = 0; i < 4; i++) {
      setDecimalPoint(i, false);
  }
}


void loop() {

  // Get current time from RTC.
  getTime();

  // Display current time (12 or 24 hour mode).
  displayTime();
    
  // Toggle alarm beep if it is active.

  // Check if it is time to play the chime (hour rolled over).

  // If no keys pressed for 5 seconds, exit alarm or date set modes.

  // Check if key pressed.
  char key = getKey();
    
  // Handle time/alarm/date set keys 1-4.

  // Handle date key.

  // Handle toggle chime key.

  // Handle 12/24 hour mode key
  if (key == 'M') {
    twentyFourHourMode = !twentyFourHourMode;
    if (twentyFourHourMode) {
        printDisplay("24HR");
    } else {
      printDisplay("12HR");
    }
    delay(1000);
  }

  // Toggle alarm on/off key.

  // Handle alarm mode key.
    
  // Handle set date key.

  // Delay 1 second.
  delay(1000);

  // Toggle seconds decimal point.
  colon = !colon;
  setDecimalPoint(1, colon);
}