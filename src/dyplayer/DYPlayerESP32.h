#ifdef ESP_PLATFORM
#include "DYPlayer.h"
namespace DY {
  class Player: public DYPlayer {
    public:
      Player(uint8_t uart_num*, uint8_t pin_rx, uint8_t pin_tx);
      void serialWrite(uint8_t *buffer, uint8_t len);
      bool serialRead(uint8_t *buffer, uint8_t len);
  };
}
#endif
