#include <Arduino.h>
#include "DYPlayerArduino.h"

// Initialise the player, it defaults to using Serial.
DY::Player player;

// Alternatively initialise on another serial port.
// DY::Player player(&Serial2);

void setup() {
  player.begin();
  player.setVolume(15); // 50% Volume
  // Pick one of these..
  // player.setCycleMode(DY::PlayMode::Sequence); // Play all and stop.
  player.setCycleMode(DY::PlayMode::Repeat); // Play all and repeat.
  player.play();
}

void loop() {
  /* Nothing to do.. */
  delay(5000);
}
