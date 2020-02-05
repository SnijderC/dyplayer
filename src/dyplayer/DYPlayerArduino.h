#ifdef ARDUINO
#include <Arduino.h>
#include "DYPlayer.h"
namespace DY {
  class Player: public DYPlayer {
    public:
      HardwareSerial *port;
      Player();
      Player(HardwareSerial* port);
      void begin();
      void serialWrite(uint8_t *buffer, uint8_t len);
      bool serialRead(uint8_t *buffer, uint8_t len);
  };
}
#endif
