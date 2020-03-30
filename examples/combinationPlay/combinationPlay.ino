/**
 * Combination play can be used a a playlist or to combine 2 sound samples.
 * For example, you could have sound samples for numbers, e.g. 40 and 2.
 * Combination play allows you to play 42.
 */

#include <Arduino.h>
#include "DYPlayerArduino.h"

// Initialise the player, it defaults to using Serial.
DY::Player player;

// Alternatively On boards with more than one serial port initialise on another
// serial port. You will have a Serial port free for debugging, uncomment the
// lines in the loop below too!
// DY::Player player(&Serial2);

void setup() {
  player.begin();
  player.setVolume(10); // 50% Volume
}

void loop() {
    char *sounds[] = { "01", "02", "03" };
    player.combinationPlay(sounds, 3);
    uint16_t lastSound = 0;
    uint8_t currentSound = 0;
    for (uint8_t i=0; i < 200; i++) {
      currentSound = player.getPlayingSound();
      if (lastSound != currentSound) {
        char currentStr[16];
        // On boards with more than one serial port:
        // sprintf(currentStr, "Playing sound %lu", currentSound);
        // Serial.print(currentStr);
        lastSound = currentSound;
        i = 0;
      }
      delay(50);
    }
  delay(30000);
}
