/*
  MeinEnigma Example

  Read the values of the function switch and the big red switch.
  See results through Arduino IDE serial monitor set to 9600 bps.

  Jeff Tranter <tranter@pobox.com.

*/

// Pin numbers for the switches.
#define RED_SWITCH  A6
#define MODE_SWITCH A7

void setup() {
  Serial.begin(9600);           // Initialize serial port.
  pinMode(RED_SWITCH,  INPUT);  // Initialize both pins as inputs.
  pinMode(MODE_SWITCH, INPUT);
}

void loop() {

  // Red switch goes to an analog input but is at a digital level.
  // High when released, low when pressed.
  int val = analogRead(RED_SWITCH);
  Serial.print("Red switch  = ");
  Serial.print(val);
  if (val < 512) {
    Serial.print(" (pressed)");
  } else {
    Serial.print(" (released)");
  }

  // Mode switch returns different values depending on position.
  // Can figure out the position based on the value.
  val = analogRead(MODE_SWITCH);
  Serial.print("  Mode switch = ");
  Serial.print(val);
  if (val < 1024 * 1/8) {
    Serial.println(" (pos 5)");
  } else if (val < 1024 * 3/8) {
    Serial.println(" (pos 4)");
  } else if (val < 1024 * 5/8) {
    Serial.println(" (pos 3)");
  } else if (val < 1024 * 7/8) {
    Serial.println(" (pos 2)");
  } else {
    Serial.println(" (pos 1)");
  }

  delay(500);  // Wait for a half second.
}
