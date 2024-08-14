#ifndef DYPLAYERARDUINO_H
#define DYPLAYERARDUINO_H

#ifdef ARDUINO
#include <Arduino.h>
#include "DYPlayer.h"

#ifdef __has_include
#if __has_include("SoftwareSerial.h")
#define HAS_SOFTWARE_SERIAL
#endif
#endif

#ifdef __has_include
#if __has_include("HardwareSerial.h")
#define HAS_HARDWARE_SERIAL
#endif
#endif

// Include SoftwareSerial for Arduino boards that probably support it.
#ifdef HAS_SOFTWARE_SERIAL
#include "SoftwareSerial.h"
#endif

namespace DY
{
  class Player : public DYPlayer
  {
  public:
    Stream *port;
    bool isSoftSerial;
    Player();
#ifdef HAS_HARDWARE_SERIAL
    Player(HardwareSerial *port);
#endif

#ifdef HAS_SOFTWARE_SERIAL
    Player(SoftwareSerial *port);
#endif
    void begin();
    void serialWrite(uint8_t *buffer, uint8_t len);
    bool serialRead(uint8_t *buffer, uint8_t len);
  };
}
#endif

#endif
