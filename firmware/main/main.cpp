/* Main
*/

#include "nvs_manager.h"
#include "led.h"
#include "FreeRTOS.h"
#include "esp_log.h"

extern "C" {
   void app_main();
   #include "display.h"
}

void app_main()
{
  init_leds();
	// init_display();
  Led redLed = Led(RED_LED);

  static BlinkParameter blinkParameter = BlinkParameter(redLed, 300);

  TaskHandle_t xHandle = NULL;

  xTaskCreate( taskBlinkLed, "BlinkRedLed", 512, &blinkParameter, tskIDLE_PRIORITY, &xHandle );

  vTaskDelay(10000 / portTICK_PERIOD_MS);
  ESP_LOGI("Task", "%i", uxTaskGetStackHighWaterMark(xHandle));

  vTaskDelete(xHandle);
  redLed.off();

}
