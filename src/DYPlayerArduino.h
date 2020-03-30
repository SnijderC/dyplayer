#ifdef ARDUINO
#include <Arduino.h>
#include "DYPlayer.h"
#include "SoftwareSerial.h"

namespace DY {
  class Player: public DYPlayer {
    public:
      Stream *port;
      bool isSoftSerial;
      Player();
      Player(HardwareSerial* port);
      Player(SoftwareSerial* port);
      void begin();
      void serialWrite(uint8_t *buffer, uint8_t len);
      bool serialRead(uint8_t *buffer, uint8_t len);
  };
}
#endif
