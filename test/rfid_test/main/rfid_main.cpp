#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "MFRC522.h"

#define SDK_pin 33
#define RST_pin 32

extern "C" {
   void app_main();
}

void app_main()
{
    MFRC522 mfrc522_module = MFRC522();
    ESP_LOGI("MFRC522", "init module...");
    mfrc522_module.PCD_Init(SDK_pin, RST_pin);

    mfrc522_module.PCD_DumpVersionToSerial();

    while(1){
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      if (mfrc522_module.PICC_IsNewCardPresent()){
        ESP_LOGI("MFRC522", "There is a card!");
        if(mfrc522_module.PICC_ReadCardSerial()){
          ESP_LOGI("MFRC522", "Wow, a uid!");
          char str_uid[30];
          for (int i = 0; i < 10; i ++){
            sprintf(&str_uid[3*i], "%.2x ", mfrc522_module.uid.uidByte[i]);
            // ESP_LOGI("MFRC522", "Uid byte %i: %.2x", i, mfrc522_module.uid.uidByte[i]);
          }
          ESP_LOGI("MFRC522", "Read uid : %s", str_uid);
        }
      }
      else {
        ESP_LOGI("MFRC522", "Nada");
      }
    }
}
