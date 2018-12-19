/* Main
*/
#include <string.h>

#include "nvs_manager.h"
#include "led.h"
#include "FreeRTOS.h"
#include "esp_log.h"
#include "MFRC522.h"
#include "driver/rtc_io.h"

#include "server.h"
// #include "user.h"

#define SDK_pin 33
#define RST_pin 32

#define WAKE_UP_PIN 27

#define ANIM_STACKSIZE 1536
#define LED_STACKSIZE 512

// volatile bool run_animation = true;

extern "C" {
   void app_main();
   #include "display.h"
}

void _init_nvs() {
  /* NVS initialization */

  // Some test data
  const char test_tag [] = {0x1A, 0x44, 0xFD, 0x90, 0x00};
  const char test_tag_2 [] = {0xC6, 0xE5, 0xC5, 0x1A, 0x00};
  const char label [] = "Paul Breugnot";
  const char label2 [] = "Sakata Gintoki";

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
    ESP_LOGI("NVS", "Written test tag : %.2x %.2x %.2x %.2x\n With label : %s"
                        ,test_tag[0]
                        ,test_tag[1]
                        ,test_tag[2]
                        ,test_tag[3]
                        ,label);
  }
  if (err != ESP_OK) {
    ESP_LOGI("NVS", "NVS error occured.");
  }
  write_tag(test_tag_2, label2);

  // User* users[getUsersCount()];
  // ESP_LOGI("USERS", "Current user count : %i", getUsersCount());
  // getUsers(users);

  // for(int i = 0; i < getUsersCount(); i ++) {
  //   ESP_LOGI("USERS", "User %i rfid : %.2x, %.2x, %.2x, %.2x", i,
  //     users[i]->getRfid()[0],
  //     users[i]->getRfid()[1],
  //     users[i]->getRfid()[2],
  //     users[i]->getRfid()[3]);
  //   ESP_LOGI("USERS", "User %i name : %s", i, users[i]->getName());
  // }
  // free(users);
}

void app_main()
{

  /* Main */

  // Leds initialization
  Led greenLed = Led(GREEN_LED);
  Led yellowLed = Led(YELLOW_LED);
  Led redLed = Led(RED_LED);
  init_leds();

  // Nvs initialization
  _init_nvs();

  // Display initialization
  u8g2_t u8g2 = init_display();
  int animOffset = 0;
  show_present_badge(&u8g2, animOffset);

  // RFID reader initialization
  MFRC522 mfrc522_module = MFRC522();
  ESP_LOGI("MFRC522", "init module...");
  mfrc522_module.PCD_Init(SDK_pin, RST_pin);

  // Init wake up GPIO
  rtc_gpio_init((gpio_num_t) WAKE_UP_PIN);
  rtc_gpio_set_direction((gpio_num_t) WAKE_UP_PIN, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pulldown_en((gpio_num_t) WAKE_UP_PIN);

  esp_sleep_enable_ext0_wakeup((gpio_num_t) WAKE_UP_PIN, 1);

  while(1){
    // Main loop
    vTaskDelay(500 / portTICK_PERIOD_MS);
    yellowLed.toggle();
    show_present_badge(&u8g2, animOffset);
    animOffset++;
    if (animOffset > 3) {
      animOffset = 0;
    }
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

          redLed.off();
          yellowLed.off();

          greenLed.on();
          ESP_LOGI("MAIN", "Show correct rfid");
          show_correct_rfid(&u8g2);
          vTaskDelay(500 / portTICK_RATE_MS);
          show_welcome(&u8g2, label);
          free(label);
          vTaskDelay(1000 / portTICK_RATE_MS);

          show_door_unlocked(&u8g2);

          bool locked = false;
          while (!locked) {
            // Wait to close the door
            vTaskDelay(500 / portTICK_PERIOD_MS);
            redLed.toggle();
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
          greenLed.off();
        }
        else {
          ESP_LOGI("MAIN", "Bad rfid!");
          show_wrong_rfid(&u8g2);
          for (int i = 0; i < 5; i++){
            redLed.toggle();
            vTaskDelay(300 / portTICK_RATE_MS);
          }
        }
        redLed.off();
      }
    }
    if (!rtc_gpio_get_level((gpio_num_t) WAKE_UP_PIN)) {
      u8g2_ClearDisplay(&u8g2);
      u8g2_SetPowerSave(&u8g2, 1);

      gpio_set_level((gpio_num_t) RST_pin, 0);

      esp_deep_sleep_start();
    }
  }

}
