#include <Arduino.h>
#include "DYPlayerArduino.h"
#include <SoftwareSerial.h>

// Initialise on software serial port.
SoftwareSerial SoftSerial(10, 11);
DY::Player player(&SoftSerial);

void setup() {
  player.begin();
  // Also initiate the hardware serial port so we can use it for debug printing
  // to the console..
  Serial.begin(9600);
  player.setVolume(15); // 50% Volume
  player.setCycleMode(DY::PlayMode::Repeat); // Play all and repeat.
  player.play();
}

void loop() {

  // Print the number of the sound that is playing.
  Serial.print("Playing sound: ");
  Serial.println((int16_t)player.getPlayingSound());
  delay(500);
}
