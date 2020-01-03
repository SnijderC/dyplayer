/* 
    This code makes the library hardware independent, it should work on
    Arduino, Espressif boards, ARM boards etc.

    It is a hardware abstraction layer, it tells the library how to use the 
    serial port on an Arduino board (and which port).
*/
#include "DYPlayerArduino.h"
void Player::begin() {
    this->port = &Serial;
    port->begin(9600);
}
void Player::begin(void* data) {
    this->port = (HardwareSerial*) data;
    port->begin(9600);
}    
void Player::serialWrite(unsigned char buffer[], uint8_t len) {
    port->write(buffer, len);
}
bool Player::serialRead(unsigned char buffer[], uint8_t len) {
    // Serial.setTimeout(1000); // Default timeout 1000ms.
    if(port->readBytes(buffer, len) > 0) {
        return true;
    }
    return false;
}
