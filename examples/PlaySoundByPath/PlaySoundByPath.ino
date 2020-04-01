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
  // For some reason periods need to be stars for this module.
  // Future releases will do this automatically.
  char path[] = "/00001.MP3";
  player.playSpecifiedDevicePath(DY::Device::Flash, path);
  delay(5000);
}
