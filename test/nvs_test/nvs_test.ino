#include <flash_manager.h>

const char test_tag [] = {0x12, 0x34, 0x56, 0x78};
const char label [] = "test_tag";

void setup() {
  // Init nvs
  esp_err_t err = init_nvs();
  if (err != OK) {
    Serial.println("init_nvs error. err number : " + err);
    return;
  }
  else{
    // test write
    err = write_tag(test_tag, label);
    if (err != ESP_OK) {
      Serial.println("write error. err number : " + err);
      return;
    }
    else {
      // test check
      char* output;
      check_tag_result result = check_tag(test_tag, output);
      if (!result) {
        Serial.println("check tag error : " + result);
        return;
      }
      else {
        // test delete
        err = delete_tag(test_tag);
        if (err != ESP_OK) {
          Serial.println("delete tag error. err number : " + err);
          return;
        }
        else {
          // re-check
          check_tag_result result = check_tag(test_tag, output);
          if (result) {
            Serial.println("Tag still there?!");
            return;
          }
          else if (result == -1) {
            Serial.println("An esp error occured...");
            return;
          }
          else {
            Serial.println("Test successful!!");
          }
        }
      }
    }
  }
}

void loop() {
}

