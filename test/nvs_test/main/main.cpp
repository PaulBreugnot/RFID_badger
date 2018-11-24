/* Main
*/

#include "nvs_manager.h"
#include "esp_log.h"

const char test_tag [] = {0x12, 0x34, 0x56, 0x78, 0x00};
const char label [] = "test_tag";

extern "C" {
   void app_main();
}

void app_main()
{
  ESP_LOGI("NVS_TEST", "Erasing nvs partition...");
  esp_err_t err = erase_nvs_partition();
  if (err != OK) {
    ESP_LOGI("NVS_TEST", "erase_nvs_partition error. err number : %i", err);
    return;
  }
  else {
    // Init nvs
    esp_err_t err = init_nvs();
    if (err != OK) {
      ESP_LOGI("NVS_TEST", "init_nvs error. err number : %i", err);
      return;
    }
    else{
      // test write
      ESP_LOGI("NVS_TEST", "Write test tag : %.2x %.2x %.2x %.2x\n With label : %s"
                          ,test_tag[0]
                          ,test_tag[1]
                          ,test_tag[2]
                          ,test_tag[3]
                          ,label);
      err = write_tag(test_tag, label);
      if (err != ESP_OK) {
        ESP_LOGI("NVS_TEST", "write error. err number : %i", err);
        return;
      }
      else {
        // test check
        char* output;
        check_tag_result result = check_tag(test_tag, &output);
        ESP_LOGI("NVS_TEST", "Output : %s"
                            ,output);
        free(output);
        if (!result) {
          ESP_LOGI("NVS_TEST", "check tag error : %i", result);
          return;
        }
        else {
          // test delete
          err = delete_tag(test_tag);
          if (err != ESP_OK) {
            ESP_LOGI("NVS_TEST", "delete tag error. err number : %i", err);
            return;
          }
          else {
            // re-check
            check_tag_result result = check_tag(test_tag, &output);
            free(output);
            if (result) {
              ESP_LOGI("NVS_TEST", "Tag still there?!");
              return;
            }
            else if (result == -1) {
              ESP_LOGI("NVS_TEST", "An esp error occured...");
              return;
            }
            else {
              ESP_LOGI("NVS_TEST", "Test successful!!");
            }
          }
        }
      }
    }
  }
}
