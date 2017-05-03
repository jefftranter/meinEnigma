/*
  MeinEnigma Example

  Turn the buzzer on and off at a one second rate. Also blinks the
  Arduino on-board LED since it is on the same digital pin.

  Jeff Tranter <tranter@pobox.com.

*/

#define BUZZER 13

void setup() {
  pinMode(BUZZER, OUTPUT);    // Initialize digital pin 13 as an output.
}

void loop() {
  digitalWrite(BUZZER, HIGH); // Turn the buzzer on.
  delay(500);                 // Wait for half a second.
  digitalWrite(BUZZER, LOW);  // Turn the buzzer off.
  delay(500);                 // Wait for half a second.
}
