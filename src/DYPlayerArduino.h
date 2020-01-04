#include <Arduino.h>
#include "DYPlayer.h"

class Player: public DYPlayer {
    public:
        HardwareSerial *port;
        Player();
        Player(HardwareSerial* port);
        void begin();
        void serialWrite(char buffer[], uint8_t len);
        bool serialRead(char buffer[], uint8_t len);
};
