#ifdef ARDUINO
#include <Arduino.h>
#include "DYPlayer.h"

// Include SoftwareSerial for Arduino boards that probably support it.
#include "SoftwareSerial.h"

namespace DY {
  class Player: public DYPlayer {
    public:
      Stream *port;
      bool isSoftSerial;
      Player();
#ifdef HAVE_HWSERIAL0
      Player(HardwareSerial* port);
#endif
      Player(SoftwareSerial* port);
      void begin();
      void serialWrite(uint8_t *buffer, uint8_t len);
      bool serialRead(uint8_t *buffer, uint8_t len);
  };
}
#endif
