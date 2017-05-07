/*
  MeinEnigma Example

  Basic clock application with alarm.

  Jeff Tranter <tranter@pobox.com.

  Feature Description
  -------------------

  Time display, 12 hour mode: 100 through 1159 with rightmost decimal
  point indicating pm. 24 hour mode: 0000 through 2359.

  Second decimal point will toggle at a one second on/one second off
  rate whenever the time is displayed.

  The time is also displayed in binary using the discrete LEDs. The
  hours are shown on the LEDs in the first row, minutes on the second
  row, and seconds on the third row.

  Time set: pressing four keys below display will perform as follows
  (left to right):
  - go back one hour
  - advance one hour
  - go back one minute
  - advance one minute

  Chime: When enabled, will beep on the hour using the buzzer.

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

  To Do:

  Allow turning rotors to set date or alarm time (not time because it
  is too easy to bump them).

  Should store settings including alarm time in EEPROM and not reset
  them on powerup.

  Handle key held down as multiple keypresses.

*/

// Wire library for i2c.
#include <Wire.h>

// External library for HT16K33 chip.
#include "ht16k33.h"

// LED font table
#include "asciifont-pinout12.h"

// I2C address of DS32310 real-time clock.
#define DS3231_ADDR 0x68

// Lookup table of discrete LEDs.
byte ledTable[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 28, 29, 30, 31, 19, 20, 21, 22, 23, 24, 25, 26, 27 };

// Lookup table of key codes to characters.
const char keys[] = { 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'A', 'S', 'D', 'F', 'P', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', 'L', 'G', 'H', 'J', 'K', '1', '2', '3', '4' };

// Output used for buzzer.
#define BUZZER 13

// Lookup table of decimal point output pins.
int decimalPoint[4] = { A0, A1, A2, A3 };

// Lookup tables for names of the months of the year.
const char *monthName[] = { "JA", "FE", "MR", "AP", "MY", "JN", "JL", "AU", "SE", "OC", "NO", "DE" };

// Object instance for HT16K33 chip.
HT16K33 HT;

// Mode flags;
bool twentyFourHourMode = false; // true: 24 hour mode, false: 12 hour mode.
bool alarmEnabled = false; // True when alarm function is enabled.
bool chimeEnabled = false; // True when chime function is enabled.

// State flags;
bool alarmActive = false; // True when alarm is currently ringing.
bool alarmState = false; // State of alarm buzzer when ringing.
bool colon = false;  // Curent on/off state of flashing colon/decimal point.
enum { time, alarmSet, dateSet } mode;

// Current date/time
int hour, minute, second;
int year, month, day;

// Alarm time.
int alarmHour = 9, alarmMinute = 0;

// Idle time - since no key pressed (in seconds).
int idleTime = 0;


// Convert BCD to decimal.
int bcd2dec(int bcd) {
  return (bcd / 16 * 10) + (bcd % 16);
}


// Convert decimal to BCD.
int dec2bcd(int dec) {
  return (dec / 10 * 16) + (dec % 10);
}


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


// Display a character on one of the displays. Leftmost display is 0,
// rightmost 3.
void displayLetter(char letter, uint8_t dispeno) {
  uint8_t led;
  int8_t i;
  uint16_t val;

  led = (dispeno) * 16;
  if (letter > '`') {
    letter -= ('a' - 'A');
  }
  val = pgm_read_word(fontTable + letter - ' ');

  HT.setDisplayRaw(dispeno * 2 + 64 / 8, val & 0xFF);
  HT.setDisplayRaw(dispeno * 2 + 64 / 8 + 1, val >> 8);
  HT.sendLed();
}


// Get current time from RTC.
void getTime()
{
  year   = bcd2dec(i2c_read(DS3231_ADDR, 6)); // Last two digits only.
  month  = bcd2dec(i2c_read(DS3231_ADDR, 5));
  day    = bcd2dec(i2c_read(DS3231_ADDR, 4));
  hour   = bcd2dec(i2c_read(DS3231_ADDR, 2));
  minute = bcd2dec(i2c_read(DS3231_ADDR, 1));
  second = bcd2dec(i2c_read(DS3231_ADDR, 0));
}


// Write current time to RTC.
void setTime()
{
  i2c_write2(DS3231_ADDR, 6, dec2bcd(year)); // Last two digits only.
  i2c_write2(DS3231_ADDR, 5, dec2bcd(month));
  i2c_write2(DS3231_ADDR, 4, dec2bcd(day));
  i2c_write2(DS3231_ADDR, 2, dec2bcd(hour));
  i2c_write2(DS3231_ADDR, 1, dec2bcd(minute));
  i2c_write2(DS3231_ADDR, 0, dec2bcd(second));
}


// Set a decimal point on display (0-3) on or off.
void setDecimalPoint(int display, bool on = true)
{
  digitalWrite(decimalPoint[display], !on);
}


// Display time.
void displayTime(int hour, int minute)
{
  char str[4];

  if (twentyFourHourMode) {
    str[0] = '0' + hour / 10;
    str[1] = '0' + hour % 10;
    str[2] = '0' + minute / 10;
    str[3] = '0' + minute % 10;
    printDisplay(str);
    setDecimalPoint(3, false); // DOn't use am/pm indicator.
  } else {
    int tmpHour = hour % 12; // Calculate 12 hour time
    if (tmpHour == 0) {
      tmpHour = 12;
    }
    str[0] = '0' + tmpHour / 10;
    if (str[0] == '0') {
      str[0] = ' '; // Suppress leading zero.
    }
    str[1] = '0' + tmpHour % 10;
    str[2] = '0' + minute / 10;
    str[3] = '0' + minute % 10;
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
  char str[4];
  // Display the month and day.
  str[0] = monthName[month - 1][0];
  str[1] = monthName[month - 1][1];
  if (day < 10) {
    str[2] = ' ';
  } else {
    str[2] = '0' + day / 10;
  }
  str[3] = '0' + day % 10;
  printDisplay(str);
}


// Display year.
void displayYear()
{
  char str[4];
  str[0] = '2';
  str[1] = '0'; // Will fail in the year 2100.
  str[2] = '0' + year / 10;
  str[3] = '0' + year % 10;
  printDisplay(str);
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
  } else {
    return keys[abs(key) - 1];
  }
}


// Chime by briefly playing buzzer.
void chime()
{
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
}

// Turn alarm buzzer on or off.
void alarm(bool state = true)
{
  digitalWrite(BUZZER, state);
}


void setup() {
  // Set the pins used for decimal point output. These happen to be
  // analog outputs but are only used at digital levels.
  for (int i = 0; i < 4; i++) {
    pinMode(decimalPoint[i], OUTPUT);
  }

  pinMode(BUZZER, OUTPUT);    // Initialize buzzer digital pin as an output.

  // Initialize serial port for debug output.
  //Serial.begin(9600);

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

  // Start in time mode.
  mode = time;
}


void loop() {

  // Get current time from RTC.
  getTime();

  // Display current time, alarm time, or date depending on mode.
  switch (mode) {
    case time:
      displayTime(hour, minute);
      break;
    case alarmSet:
      displayTime(alarmHour, alarmMinute);
      break;
    case dateSet:
      displayDate();
      break;
  }

  // Display time in binary on the discrete LEDs.
  // Show hours on the LEDs in the first row.
  for (int b = 0; b < 8; b++) {
    if (hour & bit(b)) {
      HT.setLed(ledTable[8-b]);
    } else {
      HT.clearLed(ledTable[8-b]);
    }
  }
  // Show minutes on the LEDs in the second row.
  for (int b = 0; b < 8; b++) {
    if (minute & bit(b)) {
      HT.setLed(ledTable[16-b]);
    } else {
      HT.clearLed(ledTable[16-b]);
    }
  }
  // Show seconds on the LEDs in the third row.
  for (int b = 0; b < 8; b++) {
    if (second & bit(b)) {
      HT.setLed(ledTable[25-b]);
    } else {
      HT.clearLed(ledTable[25-b]);
    }
  }
  HT.sendLed();

  // Check if it is time to play the chime (hour rolled over).
  if (chimeEnabled && minute == 0 && second == 0) {
    chime();
  }

  // Check if it is time to play the alarm.
  if (alarmEnabled && alarmHour == hour && alarmMinute == minute  && second < 3) {
    alarmActive = true;
  }

  // Toggle alarm beep if it is active.
  if (alarmActive) {
    alarm(alarmState);
    alarmState = !alarmState;
  }

  // If no keys pressed for 5 seconds, exit alarm or date set modes and revert to time mode.
  if (idleTime > 5) {
    mode = time;
  }

  // Check if key pressed.
  char key = getKey();

  // If alarm is active, pressing any key will turn it off.
  if (alarmActive && key != 0) {
    alarmActive = false;
    key = 0; // Throw away key press.
  }

  // Update how long since a key was pressed.
  if (key != 0) {
    idleTime = 0; // Key pressed, reset time.
  } else {
    idleTime++; // No key pressed, update time.
  }

  // Handle time/alarm/date set keys 1-4.
  if (key == '1' || key == '2' || key == '3' || key == '4') {
    switch (mode) {
      case time:
        switch (key) {
          case '1':
            hour -= 1;
            if (hour < 0) {
              hour = 23;
            }
            break;
          case '2':
            hour = (hour + 1) % 24;
            break;
          case '3':
            minute -= 1;
            if (minute < 0) {
              minute = 59;
            }
            break;
          case '4':
            minute = (minute + 1) % 60;
            break;
        }
        setTime();
        break;
      case alarmSet:
        switch (key) {
          case '1':
            alarmHour -= 1;
            if (alarmHour < 0) {
              alarmHour = 23;
            }
            break;
          case '2':
            alarmHour = (alarmHour + 1) % 24;
            break;
          case '3':
            alarmMinute -= 1;
            if (alarmMinute < 0) {
              alarmMinute = 59;
            }
            break;
          case '4':
            alarmMinute = (alarmMinute + 1) % 60;
            break;
        }
        break;
      case dateSet:
        switch (key) {
          case '1':
            month -= 1;
            if (month <= 0) {
              month = 12;
            }
            break;
          case '2':
            month += 1;
            if (month > 12) {
              month = 1;
            }
            break;
          case '3':
            day -= 1;
            if (day <= 0) {
              day = 31;
            }
            break;
          case '4':
            day += 1;
            if (day >= 31) {
              day = 1;
            }
            break;
        }
        setTime();
        break;
    }
  }

  // Handle date key.
  if (key == 'D') {
    displayDate();
    delay(1000);
    displayYear();
    delay(1000);
  }

  // Handle toggle chime key.
  if (key == 'C') {
    chimeEnabled = !chimeEnabled;
    chimeEnabled ? printDisplay("CH Y") : printDisplay("CH N");
    delay(1000);
  }

  // Handle 12/24 hour mode key.
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
  if (key == 'A') {
    alarmEnabled = !alarmEnabled;
    alarmEnabled ? printDisplay("AL Y") : printDisplay("AL N");
    delay(1000);
    if (alarmEnabled) {
      displayTime(alarmHour, alarmMinute); // Show alarm time.
      delay(1000);
      displayTime(hour, minute);
    }
  }

  // Handle alarm mode key.
  if (key == 'S') {
    if (mode == alarmSet) {
      mode = time;
    } else {
      mode = alarmSet;
    }
  }

  // Handle set date keys.
  if (key == 'T') {
    if (mode == dateSet) {
      mode = time;
    } else {
      mode = dateSet;
    }
  }

  // Delay 1 second, unless a key was pressed.
  if (key == 0) {
    delay(1000);
  }

  // Toggle seconds decimal point.
  colon = !colon;
  setDecimalPoint(1, colon);
}
