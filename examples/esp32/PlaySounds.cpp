#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/uart.h"
#include <DYPlayerESP32.h>

// Initialise the player on uart2, pins TX: 18, RX: 19.
DY::Player player(UART_NUM_2, 18, 19);

extern "C" void app_main(void) {
  player.setVolume(15); // 50% Volume
  while(true) {
    // Try to play sound: 00001.MP3, 00002.MP3, 00003.MP3, 00004.MP3, 00005.MP3
    // Change to next sound after 5 seconds (non-blocking call so will cut off
    // currently playing sound if any).
    for (uint8_t i = 1; i < 10; i++) {
      player.playSpecified(i);
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  }
}
