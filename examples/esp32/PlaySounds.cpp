#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/uart.h>
#include "DYPlayerESP32.h"

#define TAG "dyplayer"

// Initialise the player on uart2, pins TX: 18, RX: 19.
DY::Player player(UART_NUM_2, 18, 19);

extern "C" void app_main(void) {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  player.setVolume(15); // 50% Volume

  ESP_LOGI(TAG, "Sound count: %u", (uint8_t) player.getSoundCount());
  ESP_LOGI(TAG, "Player device %u", (uint8_t) player.getDeviceOnline());
  while(true) {
    // Change to next sound after 5 seconds (non-blocking call so will cut off
    // currently playing sound if any).
    for (uint8_t i = 1; i < player.getSoundCount(); i++) {
      player.playSpecified(i);
      ESP_LOGI(TAG, "Playing song %d", player.getPlayingSound());
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}
