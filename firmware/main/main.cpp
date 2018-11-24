/* Main
*/

#include "nvs_manager.h"
#include "led.h"
#include "FreeRTOS.h"
#include "esp_log.h"
#include "MFRC522.h"

#define SDK_pin 33
#define RST_pin 32

volatile bool run_animation = true;

extern "C" {
   void app_main();
   #include "display.h"
}

void _init_leds(Led greenLed, Led yellowLed, Led redLed, TaskHandle_t xHandleLed) {
  /* Leds init */
  init_leds();

}

void _init_nvs() {
  /* NVS initialization */

  // Some test data
  const char test_tag [] = {0x1A, 0x44, 0xFD, 0x90, 0x00};
  const char label [] = "test_tag";

  // Format the nvs partition each time (test)
  esp_err_t err = erase_nvs_partition();
  if (err == ESP_OK) {
    err = init_nvs();
  };

  // Write test values
  if (err == ESP_OK) {
    err = write_tag(test_tag, label);
  }
  if (err == ESP_OK) {
    ESP_LOGI("NVS", "Write test tag : %.2x %.2x %.2x %.2x\n With label : %s"
                        ,test_tag[0]
                        ,test_tag[1]
                        ,test_tag[2]
                        ,test_tag[3]
                        ,label);
  }
  if (err != ESP_OK) {
    ESP_LOGI("NVS", "NVS error occured.");
  }
}

void app_main()
{

  /* Main */

  // Leds initialization
  Led greenLed = Led(GREEN_LED);
  Led yellowLed = Led(YELLOW_LED);
  Led redLed = Led(RED_LED);
  TaskHandle_t xHandleLed = NULL;
  _init_leds(greenLed, yellowLed, redLed, xHandleLed);

  static BlinkParameter blinkParameter = BlinkParameter(yellowLed, 1000);
  // The yellow led blinks continuously
  xTaskCreate( taskBlinkLed, "BlinkYellowLed", 512, &blinkParameter, tskIDLE_PRIORITY, &xHandleLed );

  // Display initialization
  _init_nvs();

  // Display initialization
  TaskHandle_t xHandleScreen = NULL;

  u8g2_t u8g2 = init_display(); // initialize display with the "Min Bot" header
  static AnimationParameter animationParameter;
  animationParameter.u8g2 = &u8g2;
  animationParameter.run = &run_animation;
  // Show the "present badge animation"
  xTaskCreate(task_show_present_badge, "PresentBadge", 2048, &animationParameter, tskIDLE_PRIORITY, &xHandleScreen );

  // RFID reader initialization
  MFRC522 mfrc522_module = MFRC522();
  ESP_LOGI("MFRC522", "init module...");
  mfrc522_module.PCD_Init(SDK_pin, RST_pin);

  while(1){
    vTaskDelay(500 / portTICK_PERIOD_MS);
    if (mfrc522_module.PICC_IsNewCardPresent()){
      // Handle rfid
      if(mfrc522_module.PICC_ReadCardSerial()){
        char rfid[5];
        for (int i = 0; i < 4; i ++){
           sprintf(&rfid[i], "%c", mfrc522_module.uid.uidByte[i]);
        }
        rfid[4] = 0x00;
        ESP_LOGI("MFRC522", "Read uid : %.2x %.2x %.2x %.2x", rfid[0], rfid[1], rfid[2], rfid[3]);

        if (check_tag(rfid, NULL)){
          run_animation = false; // Softly kill the animation
          vTaskDelay(1000 / portTICK_RATE_MS); // A delay to ensure animation is cleared

          vTaskSuspend(xHandleLed);
          yellowLed.off();
          greenLed.on();
          show_correct_rfid(&u8g2);

          ESP_LOGI("MAIN", "Good rfid!");
        }
        else {
          ESP_LOGI("MAIN", "Bad rfid!");
        }
      }
    }
  }
  // vTaskDelay(20000 / portTICK_PERIOD_MS);
  // ESP_LOGI("Task", "%i", uxTaskGetStackHighWaterMark(xHandleScreen));

  vTaskDelete(xHandleLed);
  vTaskDelete(xHandleScreen);
  yellowLed.off();

}
