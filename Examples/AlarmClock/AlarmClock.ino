/*
  MeinEnigma Example

  Basic clock application with alarm.

  Jeff Tranter <tranter@pobox.com.

Feature Descripion
------------------

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
Leave alar mset mode if no key pressed for more than 5 seconds.

T - Set date. Pressing keys under display will set month and day,
similar to time set. Pressing T again will exit date set mode.
Leave date mode if no key pressed for more than 5 seconds.

*/

void setup() {

  // Initialize serial port for debug output.

  // Initialize HT16K33 chip for keyboard/display.

}

void loop() {

}
