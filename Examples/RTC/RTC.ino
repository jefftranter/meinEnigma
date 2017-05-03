/*
  MeinEnigma Example

  Demonstrates reading DS3231 real-time clock over I2C.
  See results through Ardino IDE serial monitor set to 9600 bps.

  Jeff Tranter <tranter@pobox.com.

*/

#include <Wire.h>

// I2C address of DS3231 real-time clock.
#define DS3231_ADDR 0x68

// Lookup tables for names of the days of the week and months of the year.
const char *dayName[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
const char *monthName[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

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

// Print a BCD number to the serial port with leading zero added if it
// is less than 0x10, e.g. prints "01" for 1, "11" for 11.
void printLeadingZero(int val)
{
  if (val < 0x10) {
    Serial.print("0");
  }
  Serial.print(val, HEX);
}

// Get time and date from RTC and print it out the serial port.
// Note that these could be wrong, especially day of the week, if you
// have not set the real-time clock.
void printTime() {
  uint8_t hour, minute, second;
  uint8_t year, month, day, dow;

  year   = i2c_read(DS3231_ADDR, 6);
  month  = i2c_read(DS3231_ADDR, 5);
  day    = i2c_read(DS3231_ADDR, 4);
  dow    = i2c_read(DS3231_ADDR, 3);
  hour   = i2c_read(DS3231_ADDR, 2 );
  minute = i2c_read(DS3231_ADDR, 1);
  second = i2c_read(DS3231_ADDR, 0);

  printLeadingZero(hour);
  Serial.print(":");
  printLeadingZero(minute);
  Serial.print(":");
  printLeadingZero(second);
  Serial.print(" ");
  printLeadingZero(day);
  Serial.print("-");
  Serial.print(monthName[month - 1]);
  Serial.print("-");
  Serial.print("20"); // RTC does not store the century.
  printLeadingZero(year);
  Serial.print(" ");
  Serial.println(dayName[dow - 1]);
}

void setup() {
  Serial.begin(9600); // Initialize serial port.
  Wire.begin();       // Initialize the Wire library.
  Serial.println("Real-time Clock Demo");
}

void loop() {
  printTime(); // Display the current time.
  delay(1000); // Wait one second.
}
