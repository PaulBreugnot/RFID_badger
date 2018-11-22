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
	u8g2_t u8g2 = init_display();
  Led redLed = Led(YELLOW_LED);

  static BlinkParameter blinkParameter = BlinkParameter(redLed, 300);

  TaskHandle_t xHandleLed = NULL;

  xTaskCreate( taskBlinkLed, "BlinkRedLed", 512, &blinkParameter, tskIDLE_PRIORITY, &xHandleLed );

  TaskHandle_t xHandleScreen = NULL;

  xTaskCreate(task_show_present_badge, "PresentBadge", 1024, &u8g2, tskIDLE_PRIORITY, &xHandleScreen );

  vTaskDelay(20000 / portTICK_PERIOD_MS);
  ESP_LOGI("Task", "%i", uxTaskGetStackHighWaterMark(xHandleScreen));

  vTaskDelete(xHandleLed);
  vTaskDelete(xHandleScreen);
  redLed.off();

}
