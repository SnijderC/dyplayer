#include <Arduino.h>
#include "DYPlayerArduino.h"

// Initialise the player, it defaults to using Serial.
DY::Player player;

// Alternatively initialise on another serial port.
// DY::Player player(&Serial2);

void setup() {
  player.begin();
  player.setVolume(15); // 50% Volume
}

void loop() {
  player.playSpecified(1);
  delay(5000);
}
