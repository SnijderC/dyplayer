#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <DYPlayerESP32.h>
// Initialise the player, it defaults to using Serial.
DY::Player player(UART_NUM_2, 18, 19);

extern "C" void app_main(void) {
  player.setVolume(15); // 50% Volume
  while(true) {
    player.playSpecified(1);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
