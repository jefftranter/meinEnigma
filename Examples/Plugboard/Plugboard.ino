/*
  MeinEnigma Example

  Demonstrates controlling the plugboard.
  Uses code from the MeinEnigma software.

  Jeff Tranter <tranter@pobox.com.

*/

#include <Wire.h>

/*
  The plugboard uses two MCP23017 16-bit i/o expander chips. The ports
  are assigned as follows:

  Chip:   U301
  Port:   GPA0 GPA1 GPA2 GPA3 GPA4 GPA5 GPA6 GPA7 GPB0 GPB1 GPB2 GPB3 GPB4 GPB5 GPB6 GPB7
  Plug #:  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16
  Letter:  Q    W    E    R    T    Z    U    I    O    A    S    D    F    G    H    J

  Chip:   U302
  Port:   GPA0 GPA1 GPA2 GPA3 GPA4 GPA5 GPA6 GPA7 GPB0 GPB1 GPB2 GPB3 GPB4 GPB5 GPB6 GPB7
  Plug #:  17   18   19   20   21   22   23   24   25   26   NC   NC   NC   NC   NC   NC
  Letter:  K    P    Y    X    C    V    B    N    M    L

*/

//  MCP23017 registers, all as seen from bank 0.
//
#define mcp_address 0x20 // I2C Address of MCP23017 (U301)
#define IODIRA      0x00 // I/O Direction Register Address of Port A
#define IODIRB      0x01 // I/O Direction Register Address of Port B
#define IPOLA       0x02 // Input polarity port register 
#define IPOLB       0x03 // "
#define GPINTENA    0x04 // Interrupt on change
#define GPINTENB    0x05 // "
#define DEFVALA     0x06 // Default value register
#define DEFVALB     0x07 // "
#define INTCONA     0x08 // Interrupt on change control register
#define INTCONB     0x09 // "
//#define IOCON     0x0A // Control register
#define IOCON       0x0B // "
#define GPPUA       0x0C // GPIO Pull-up resistor register
#define GPPUB       0x0D // "
#define INTFA       0x0E // Interrupt flag register
#define INTFB       0x0F // "
#define INTCAPA     0x10 // Interrupt captured value for port register
#define INTCAPB     0x11 // "
#define GPIOA       0x12 // General purpose i/o register
#define GPIOB       0x13 // "
#define OLATA       0x14 // Output latch register
#define OLATB       0x15 // "

// Lookup table of i/o port addresses for each plug number.
char address[26] = { mcp_address, mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address,  mcp_address, mcp_address + 1, mcp_address + 1, mcp_address + 1, mcp_address + 1, mcp_address + 1, mcp_address + 1, mcp_address + 1, mcp_address + 1, mcp_address + 1, mcp_address + 1 };

// Lookup table of register addresses for each plug number.
char reg[26] = { GPIOA, GPIOA,  GPIOA,  GPIOA,  GPIOA,  GPIOA,  GPIOA,  GPIOA, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOB, GPIOB };

// Lookup table of i/o port bits for each plug number.
char bit[26] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1 };

// Lookup table of letters for each plug #.
char plug[27] = "QWERTZUIOASDFGHJKPYXCVBNML";

// Write a single byte to i2c.
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

// Read two bytes starting at a specific address.
// Read is done in two sessions - as required by MCP23017 (page 5 in datasheet).
uint16_t i2c_read2(uint8_t unitaddr, uint8_t addr) {
  uint16_t val = 0;
  i2c_write(unitaddr, addr);
  Wire.requestFrom(unitaddr, (uint8_t)1);
  val = Wire.read();
  Wire.requestFrom(unitaddr, (uint8_t)1);
  return Wire.read() << 8 | val;
}

// Read a byte from specific address. Send one byte (address to read) and read a byte.
uint8_t i2c_read(uint8_t unitaddr, uint8_t addr) {
  i2c_write(unitaddr, addr);
  Wire.requestFrom(unitaddr, (uint8_t)1);
  return Wire.read();    // Read one byte.
}

void setup() {

  Serial.begin(9600);
  Serial.println("Plugboard Demonstration");

  Wire.begin(); // Enable the Wire library.

  // Initialize U301
  // Set up the port multiplier.
  // Init value for IOCON, bank(0)+INTmirror(no)+SQEOP(addr
  // inc)+DISSLW(Slew rate disabled)+HAEN(hw addr always
  // enabled)+ODR(INT open)+INTPOL(act-low)+0(N/A)
  i2c_write2(mcp_address, IOCON, 0b00011110);
  i2c_write2(mcp_address, IODIRA, 0xff); // Set all ports to inputs.
  i2c_write2(mcp_address, IODIRB, 0xff); // Set all ports to inputs.
  i2c_write2(mcp_address, GPPUA, 0xff);  // Enable pullup.
  i2c_write2(mcp_address, GPPUB, 0xff);  // "

  // Initialize U302 - same as above
  i2c_write2(mcp_address + 1, IOCON, 0b00011110);
  i2c_write2(mcp_address + 1, IODIRA, 0xff);
  i2c_write2(mcp_address + 1, IODIRB, 0xff);
  i2c_write2(mcp_address + 1, GPPUA, 0xff);
  i2c_write2(mcp_address + 1, GPPUB, 0xff);
}

void loop() {
  int val;

  // Display levels of each hardware port.
  for (int i = 0; i < 10; i++ ) {
    Serial.print("U301");
    Serial.print(" GPIOA = ");
    val = i2c_read(mcp_address, GPIOA);
    Serial.print(val, BIN);
    Serial.print(" GPIOB = ");
    val = i2c_read(mcp_address, GPIOB);
    Serial.print(val, BIN);

    Serial.print(" U302");
    Serial.print(" GPIOA = ");
    val = i2c_read(mcp_address + 1, GPIOA);
    Serial.print(val, BIN);
    Serial.print(" GPIOB = ");
    val = i2c_read(mcp_address + 1, GPIOB);
    Serial.println(val, BIN);
  }
  Serial.println();
  delay(3000);

  // Display level of each plug by name.
  for (int i = 0; i < 26; i++) {
    Serial.print("Plug ");
    Serial.print(i + 1);
    Serial.print(" \"");
    Serial.print(plug[i]);
    Serial.print("\" is ");
    val = (i2c_read(address[i], reg[i]) >> bit[i]) & 1;
    if (val) {
      Serial.println("high");
    } else {
      Serial.println("low");
    }
  }
  Serial.println();
  delay(3000);

  // Figure out what jumpers are connected by driving one port at a time
  // low and seeing what input port(s) go low.
  for (int i = 0; i < 26; i++) {

    // Set port i to be an output.
    i2c_write2(address[i], reg[i] - 0x12, ~(1 << bit[i])); // 0x12 is offset between GPIO and IODIR
    // And set it low.
    i2c_write2(address[i], reg[i], ~(1 << bit[i]));

    Serial.print("Plug ");
    Serial.print(i + 1);
    Serial.print(" \"");
    Serial.print(plug[i]);
    Serial.print("\" is connected to");

    for (int j = 0; j < 26; j++) {
      if (i == j) { // Don't check for jumper connected to itself.
        continue;
      }
      val = (i2c_read(address[j], reg[j]) >> bit[j]) & 1;
      if (val == 0) {
        Serial.print(" ");
        Serial.print(plug[j]);
      }
    }
    Serial.println();

    // Set port i back to being an input.
    i2c_write2(address[i], reg[i] - 0x12, ~(1 << bit[i]));
    // And set it high.
    i2c_write2(address[i], reg[i], 0xff);

  }
  Serial.println();
  delay(3000);
}
