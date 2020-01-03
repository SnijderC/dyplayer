#include <Arduino.h>
#include <inttypes.h>
#include "dysv17f"


/* 
    This code is makes the library hardware independent, it should work on
    Arduino, Espressif boards, ARM boards etc.

    It is a hardware abstraction layer, it tells the library how to use the 
    serial port on an Arduino board (and which port).
*/
class Player: public AbstractPlayer {
    public:
        HardwareSerial *port;
        void begin(void* data);
        void serialWrite(unsigned char buffer[], uint8_t len);
        bool serialRead(unsigned char buffer[], uint8_t len);
};

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

Player player;

void setup() {
    player.begin((void*)&Serial);
    player.setDevice(FLASH);
    // If the device was not set, flash the onboard led once every half second. 
    if (player.getDevice() != FLASH) {
        pinMode(LED_BUILTIN, OUTPUT);
        while (1) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(500);
        }
    }
}

void loop() {

}
