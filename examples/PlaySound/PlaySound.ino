#include <Arduino.h>
#include "DYPlayerArduino.h"

Player player;
// Or on another serial port.
// Player player(&Serial2);
void setup() {
    player.begin();
    //player.setDevice(FLASH);
    player.setVolume(5); // 50% Volume
    player.getDevice();
}

void loop() {
  player.playSpecified(1);
  while (true) {
    delay(5000);
  }
}
