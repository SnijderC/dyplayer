/*
  This is a hardware abstraction layer, it tells the library how to use the
  serial port on an Arduino board (and which port).
*/
#ifdef ARDUINO
#include "DYPlayerArduino.h"

namespace DY {
  Player::Player() {
    this->port = &Serial;
    this->isSoftSerial = false;
  }
#ifdef HAVE_HWSERIAL0
  Player::Player(HardwareSerial* port) {
    this->port = (Stream*)port;
    this->isSoftSerial = false;
  }
#endif
#ifdef __AVR__
  Player::Player(SoftwareSerial* port) {
    this->port = (Stream*)port;
    this->isSoftSerial = true;
  }
#endif
  void Player::begin() {
    if (isSoftSerial) {
#ifdef __AVR__
      ((SoftwareSerial*)port)->begin(9600);
#endif
    } else {
#ifdef HAVE_HWSERIAL0
      ((HardwareSerial*)port)->begin(9600);
#endif
    }
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
#endif
