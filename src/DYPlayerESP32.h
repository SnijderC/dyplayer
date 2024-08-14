#ifndef DYPLAYERESP32_H
#define DYPLAYERESP32_H

#ifdef ESP_PLATFORM
#include "driver/uart.h"
#include "DYPlayer.h"
namespace DY
{
  class Player : public DYPlayer
  {
  public:
    Player(uart_port_t uart_num, uint8_t pin_rx, uint8_t pin_tx);
    void serialWrite(uint8_t *buffer, uint8_t len);
    bool serialRead(uint8_t *buffer, uint8_t len);
    uart_port_t uart_num;
  };
}
#endif

#endif