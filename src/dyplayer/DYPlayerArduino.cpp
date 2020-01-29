/*
  This is a hardware abstraction layer, it tells the library how to use the
  serial port on an Arduino board (and which port).
*/
#include "DYPlayerArduino.h"

namespace DY {
  Player::Player() {
    this->port = &Serial;
  }
  Player::Player(HardwareSerial* port) {
    this->port = port;
  }
  void Player::begin() {
    port->begin(9600);
  }
  void Player::serialWrite(uint8_t *buffer, uint8_t len) {
    port->write(buffer, len);
  }
  bool Player::serialRead(uint8_t *buffer, uint8_t len) {
    // Serial.setTimeout(1000); // Default timeout 1000ms.
    if(port->readBytes(buffer, len) > 0) {
      return true;
    }
    return false;
  }
}
