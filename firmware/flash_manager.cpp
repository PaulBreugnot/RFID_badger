#include "flash_manager.h"
#include "nvs_flash.h"
#include "nvs.h"

nvs_handle my_handle;

esp_err_t init_nvs() {
  /*
   * Need to be called at initialization to use the nvs (Non-Volatile Storage).
   */
  // Init nvs
  esp_err_t err = nvs_flash_init();
  if (err != ESP_OK) {
    // flash problem
    return err;
  }
  else {
    // Open nvs
    return nvs_open("key_storage", NVS_READWRITE, &my_handle);
  }
}

esp_err_t write_tag(const char *rfid_tag, const char *value) {
  /*
   * Write an rfid tag to the nvs.
   * The associated value can be a user name, a badge function...
   */
  esp_err_t err = nvs_set_str(my_handle, rfid_tag, value);
  // Commit tag
  if (err != ESP_OK) {
    return err;
  }
  return nvs_commit(my_handle);
}

check_tag_result check_tag(const char *rfid_tag, char **output_value) {
  /*
   * Check if rfid_tag is a valid tag.
   * If its a valid tag, result is stored in output_value.
   * If you just want to check if the tag is valid, call this function
   * with output_value set NULL.
   */
  size_t value_length;
  // Check if the tag is valid (a.k.a is a valid nvs key) and eventually 
  // store the length of the associated value in value_length for later.
  esp_err_t err = nvs_get_str(my_handle, rfid_tag, NULL, &value_length);
  char* read_value = (char *) malloc(value_length);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    return INVALID_TAG;
  }
  else if (err == ESP_OK) {
    // The tag is valid, and the associated value is stored in output_value
    if (read_value != NULL) {
      nvs_get_str(my_handle, rfid_tag, read_value, &value_length);
      free(*output_value);
      *output_value = read_value;
    }
    return VALID_TAG;
  }
  else {
    return READ_ERROR;
  }
}

esp_err_t delete_tag(const char *rfid_tag) {
  /*
   * Remove a tag from the nvs, so the tag become invalid.
   */
  esp_err_t err = nvs_erase_key(my_handle, rfid_tag);
  if (err != ESP_OK) {
    return err;
  }
  return nvs_commit(my_handle);
}

esp_err_t delete_all_tags() {
  /*
   * Remove all tags.
   */
  esp_err_t err = nvs_erase_all(my_handle);
  if (err != ESP_OK) {
    return err;
  }
  return nvs_commit(my_handle);
}
