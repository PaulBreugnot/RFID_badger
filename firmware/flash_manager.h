// Functions to manage eeprom (e.g. to save/modify/delete RFID tags)

#include "errors.h"
#include "esp_err.h"
#include "nvs.h"


#define INVALID_TAG 0
#define VALID_TAG 1
#define READ_ERROR -1
typedef int check_tag_result;

err_t init_nvs();
err_t write_tag(const char *rfid_tag, const char *value);
check_tag_result check_tag(const char *rfid_tag, char **output_value);
esp_err_t delete_tag(const char *rfid_tag);
esp_err_t delete_all_tags();
