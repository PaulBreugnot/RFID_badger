// Functions to manage eeprom (e.g. to save/modify/delete RFID tags)

// #include "errors.h"
#include "esp_err.h"
#include "nvs.h"
#include "user.h"


#define INVALID_TAG 0
#define VALID_TAG 1
#define READ_ERROR -1
typedef int check_tag_result;

esp_err_t init_nvs();
esp_err_t write_tag(const char *rfid_tag, const char *value);
check_tag_result check_tag(const char *rfid_tag, char **output_value);
esp_err_t delete_tag(const char *rfid_tag);
esp_err_t delete_all_tags();
esp_err_t erase_nvs_partition();
size_t get_available_users(char* available_users);
void log_available_users();

int getUsersCount();
void getUsers(User** users_ptr_array);
