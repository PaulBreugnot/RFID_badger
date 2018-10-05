// Functions to manage eeprom (e.g. to save/modify/delete RFID tags)
#include "errors.h"

#define INVALID_TAG 0
#define VALID_TAG 1
#define READ_ERROR -1
typedef int check_tag_result;


err_t init_nvs();
err_t write_tag(const char *rfid_tag, const char *value);
check_tag_result check_tag(const char *rfid_tag, const char *output_value);
