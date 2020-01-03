#include <Arduino.h>
#include <inttypes.h>
#include "DYPlayerArduino.h"

Player player;

void setup() {
    player.begin();
    // Or on another serial port.
    // player.begin((HardwareSerial*) Serial2);
    player.setDevice(FLASH);
    player.setVolume(15); // 50% Volume
}

void loop() {
  player.playSpecified(1);
  delay(5000);
  player.playSpecified(2);
  delay(5000);
  player.playSpecified(3);
  delay(5000);
}
