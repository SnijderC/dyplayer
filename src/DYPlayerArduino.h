#include <Arduino.h>
#include "DYPlayer.h"

class Player: public DYPlayer {
    public:
        HardwareSerial *port;
        Player();
        Player(HardwareSerial* port);
        void begin();
        void serialWrite(unsigned char buffer[], uint8_t len);
        bool serialRead(unsigned char buffer[], uint8_t len);
};
