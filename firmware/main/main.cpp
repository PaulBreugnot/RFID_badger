/* Main
*/
#include <string.h>

#include "nvs_manager.h"
#include "led.h"
#include "FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "MFRC522.h"

#define SDK_pin 33
#define RST_pin 32

#define ANIM_STACKSIZE 1536
#define LED_STACKSIZE 512

volatile bool run_animation = true;

extern "C" {
   void app_main();
   #include "display.h"
}

void _init_nvs() {
  /* NVS initialization */

  // Some test data
  const char test_tag [] = {0x1A, 0x44, 0xFD, 0x90, 0x00};
  const char label [] = "Paul Breugnot";

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
  init_leds();

  // The yellow led blinks continuously
  TaskHandle_t xHandleWaitingLed;
  static BlinkParameter blinkParameter = BlinkParameter(yellowLed, 1000);
  StaticTask_t xTaskLedBuffer;
  StackType_t xStackLed[ LED_STACKSIZE ];

  xHandleWaitingLed = xTaskCreateStatic(
    taskBlinkLed,
    "BlinkYellowLed",
    LED_STACKSIZE,
    &blinkParameter,
    tskIDLE_PRIORITY + 1,
    xStackLed,
    &xTaskLedBuffer
  );

  // Display initialization
  _init_nvs();

  // Display initialization
  TaskHandle_t xHandleScreen = NULL;

  u8g2_t u8g2 = init_display();

  // Set the "present badge" animation
  static AnimationParameter animationParameter;
  animationParameter.u8g2 = &u8g2;
  animationParameter.run = &run_animation;
  xTaskCreate(task_show_present_badge, "PresentBadge", ANIM_STACKSIZE, &animationParameter, tskIDLE_PRIORITY, &xHandleScreen );

  // RFID reader initialization
  MFRC522 mfrc522_module = MFRC522();
  ESP_LOGI("MFRC522", "init module...");
  mfrc522_module.PCD_Init(SDK_pin, RST_pin);

  // Uncomment to test memory usage
  // vTaskDelay(20000 / portTICK_RATE_MS);
  // ESP_LOGI("MEM", "screen %i", uxTaskGetStackHighWaterMark(xHandleScreen));
  // ESP_LOGI("MEM", "led %i", uxTaskGetStackHighWaterMark(xHandleWaitingLed));

  while(1){
    // Main loop
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

        char* label;
        if (check_tag(rfid, &label)){
          ESP_LOGI("MAIN", "Good rfid! %s", label);
          run_animation = false; // Softly kill the animation
          vTaskDelay(500 / portTICK_RATE_MS); // A delay to ensure animation is cleared

          // Suspend yellow led animation
          vTaskSuspend(xHandleWaitingLed);
          yellowLed.off();

          greenLed.on();
          ESP_LOGI("MAIN", "Show correct rfid");
          show_correct_rfid(&u8g2);
          vTaskDelay(500 / portTICK_RATE_MS);
          show_welcome(&u8g2, label);
          free(label);
          vTaskDelay(1000 / portTICK_RATE_MS);

          // Blink red led
          TaskHandle_t xHandleWarningLed;
          static BlinkParameter blinkParameter = BlinkParameter(redLed, 200);
          StaticTask_t xTaskWarningLedBuffer;
          StackType_t xStackWarningLed[ LED_STACKSIZE ];

          xHandleWarningLed = xTaskCreateStatic(
            taskBlinkLed,
            "BlinkYellowLed",
            LED_STACKSIZE,
            &blinkParameter,
            tskIDLE_PRIORITY + 1,
            xStackWarningLed,
            &xTaskWarningLedBuffer
          );

          show_door_unlocked(&u8g2);

          bool locked = false;
          while (!locked) {
            // Wait to close the door
            vTaskDelay(500 / portTICK_PERIOD_MS);
            if (mfrc522_module.PICC_IsNewCardPresent()){
              // Handle rfid
              if(mfrc522_module.PICC_ReadCardSerial()){
                char lock_rfid[5];
                for (int i = 0; i < 4; i ++){
                   sprintf(&lock_rfid[i], "%c", mfrc522_module.uid.uidByte[i]);
                }
                lock_rfid[4] = 0x00;
                if (check_tag(lock_rfid, NULL)){
                  ESP_LOGI("MAIN", "Lock door.");
                  locked = true;
                  show_door_lock(&u8g2);
                  vTaskDelay(500 / portTICK_RATE_MS);
                }
                else {
                  ESP_LOGI("MAIN", "Bad rfid.");
                  show_wrong_rfid(&u8g2);

                  vTaskDelay(1000 / portTICK_RATE_MS);

                  show_door_unlocked(&u8g2);
                }
              }
            }
          }
          vTaskDelete(xHandleWarningLed);
          greenLed.off();
        }
        else {
          ESP_LOGI("MAIN", "Bad rfid!");
          run_animation = false; // Softly kill the animation
          vTaskDelay(500 / portTICK_RATE_MS); // A delay to ensure animation is cleared

          // Blink red led
          TaskHandle_t xHandleWrongLed;
          static BlinkParameter blinkParameter = BlinkParameter(redLed, 200);
          StaticTask_t xTaskWrongLedBuffer;
          StackType_t xStackWrongLed[ LED_STACKSIZE ];

          xHandleWrongLed = xTaskCreateStatic(
            taskBlinkLed,
            "BlinkRedLed",
            LED_STACKSIZE,
            &blinkParameter,
            tskIDLE_PRIORITY + 1,
            xStackWrongLed,
            &xTaskWrongLedBuffer
          );

          show_wrong_rfid(&u8g2);

          vTaskDelay(2500 / portTICK_PERIOD_MS);
          vTaskDelete(xHandleWrongLed);
        }
        redLed.off();
        vTaskResume(xHandleWaitingLed);

        // Show the "present badge animation"
        run_animation = true;
        xTaskCreate(task_show_present_badge, "PresentBadge", ANIM_STACKSIZE, &animationParameter, tskIDLE_PRIORITY, &xHandleScreen );
      }
    }
  }

  vTaskDelete(xHandleWaitingLed);
  vTaskDelete(xHandleScreen);
  yellowLed.off();

}
