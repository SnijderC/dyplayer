/*
  This is a hardware abstraction layer, it tells the library how to use the
  serial port on an Arduino board (and which port).
*/
#ifdef ESP_PLATFORM
#include "DYPlayerESP32.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "esp_system.h"
#include "driver/uart.h"

#define BUFFER_SIZE_RX 256
#define BUFFER_SIZE_TX 256

namespace DY {
  Player::Player(uart_port_t uart_num, uint8_t pin_tx, uint8_t pin_rx) {
    uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 0, // Ignored because hw flowctl is disabled
      .use_ref_tick = false
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, pin_tx, pin_rx, -1, -1));
    // Install UART driver using an event queue here
    QueueHandle_t uart_queue;
    ESP_ERROR_CHECK(uart_driver_install(
      uart_num,
      BUFFER_SIZE_RX,
      BUFFER_SIZE_TX,
      10,
      &uart_queue,
      0
    ));
    this->uart_num = uart_num;
  }
  void Player::serialWrite(uint8_t *buffer, uint8_t len) {
    // Blocking call, waits for space in hardware FIFO buffer.
    // Commands are short so this shouldn't be an issue.
    uart_write_bytes(uart_num, (char*)buffer, len);
  }
  bool Player::serialRead(uint8_t *buffer, uint8_t len) {
    int length = 0;
    while (length < len) {
      // Expected returns values should always fit in the FIFO buffer, they
      // are always just bytes of single digit length.
      ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    }
    length = uart_read_bytes(uart_num, buffer, length, 10);
    if(length == len) {
      return true;
    }
    return false;
  }
}
#endif
