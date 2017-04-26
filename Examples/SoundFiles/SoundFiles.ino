/*
  MeinEnigma Example

  Demonstrates playing sounds with the sound module. Module must be
  present and have MP3 files on the SD card.

  Uses code from the MeinEnigma software.

  Jeff Tranter <tranter@pobox.com.

*/

#include <AltSoftSerial.h>

AltSoftSerial altSerial;

// For sound module FN_M16P aka DFplayer
#define BUSY 12

#define dfcmd_PLAYNO 0x03       // Play the nth song
#define dfcmd_PLAYNAME 0x12     // Play song in /mp3 named nnnn*mp3
#define dfcmd_VOLUME 0x06       // Set volume
#define dfcmd_RESET  0x0c       // Reset unit
#define dfcmd_GETCNT 0x48       // Get SD card file count
#define dfcmd_GETSTATE 0x42     // Get current status
#define dfcmd_GETFEEDBACK 0x41  // Get feedback from module

// Buffer for sound commands.
uint8_t msgBuf[10]= { 0X7e, 0xff, 0, 0, 0, 0, 0, 0, 0, 0xef};

// Write data to sound board. Send "cmd" with the option "opt".
void sendCommand(uint8_t cmd, uint16_t opt=0) {
  uint8_t i;
  uint16_t csum;

  msgBuf[0]= 0x7e;        // Start
  msgBuf[1]= 0xff;        // Version
  msgBuf[2]= 6;           // Length = always 6
  msgBuf[3]= cmd;         // Command
  msgBuf[4]= 0;           // Feedback with 0x41, 0 if no and 1 if yes
  msgBuf[5]= opt >> 8;    // Optional value high byte
  msgBuf[6]= opt & 0xff;  // Optional value low byte
  csum = 0 - (msgBuf[1] + msgBuf[2] + msgBuf[3] + msgBuf[4] + msgBuf[5] + msgBuf[6]);
  msgBuf[7]= csum >> 8;
  msgBuf[8]= csum & 0xff;
  msgBuf[9]= 0xef;        // End

  for (i = 0; i < 10; i++) {
    altSerial.write(msgBuf[i]);
  }
}

// Write data to soundboard. To abort any currently playing sound, set
// wait=false.
void playSound(uint16_t fileno, boolean wait=true) {
  int16_t cnt;
  uint8_t retry;

  if (wait) {  // Should we make sure it's done playing.
              // It's just small snippets so it shouldn't take too long.
    cnt = 0;
    while (digitalRead(BUSY) == LOW && cnt < 300) {
      cnt++;
      delay(10);
    }
  }

  retry = 3;
  do {
    // Send play command.
    sendCommand(dfcmd_PLAYNAME, fileno);
    // Wait for it to start playing.
    cnt = 0;
    while (digitalRead(BUSY) == HIGH && cnt < 200) {
      cnt++;
      delay(1);
    }
    retry--;
  } while (digitalRead(BUSY) == HIGH && retry > 0); // If not started, send again.
}

void setup() {
    altSerial.begin(9600);
    sendCommand(dfcmd_RESET, 0);   // Reset unit.
    delay(500);    
    sendCommand(dfcmd_VOLUME, 30); // Set volume.
}

void loop() {
  for (int i = 1; i <= 26; i++) {
    playSound(1500 + i); // "zero"
  }
  delay(10000);
}
