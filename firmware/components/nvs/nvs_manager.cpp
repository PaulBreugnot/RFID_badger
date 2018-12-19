#include "nvs_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#include "string.h"

nvs_handle my_nvs_handle;

esp_err_t init_nvs() {
  /*
   * Need to be called at initialization to use the nvs (Non-Volatile Storage).
   */
  // Init nvs
  esp_err_t err = nvs_flash_init();
  if (err != ESP_OK) {
    // flash problem
    ESP_LOGI("NVS", "Error at nvs_flash_init : %i", err);
    return err;
  }
  else {
    // Open nvs
    err = nvs_open("key_storage", NVS_READWRITE, &my_nvs_handle);
    if (err != ESP_OK) {
      ESP_LOGI("NVS", "Error at nvs_open : %i", err);
    }
    size_t available_users_len;
    if ( nvs_get_str(my_nvs_handle, "available_users", NULL,  &available_users_len) == ESP_ERR_NVS_NOT_FOUND) {
      esp_err_t err = nvs_set_str(my_nvs_handle, "available_users", "");
      // Commit tag
      if (err != ESP_OK) {
        ESP_LOGI("NVS", "Error initializing available_users : %.3x", err);
        return err;
      }
    }
    return err;
  }

}

size_t get_available_users(char** available_users) {
  size_t available_users_len;
  nvs_get_str(my_nvs_handle, "available_users", NULL,  &available_users_len);

  *available_users = (char*) malloc(available_users_len);
  nvs_get_str(my_nvs_handle, "available_users", *available_users,  &available_users_len);

  return available_users_len;
}

void log_available_users() {
  char* available_users;
  size_t available_users_len = get_available_users(&available_users) - 1;

  int str_len = 3 * available_users_len + 2 * available_users_len / 4 + 1;

  char available_users_str[str_len];
  int offset = 0;
  for (int i = 0; i < available_users_len; i++) {
    if (i > 0 && i % 4 == 0) {
        sprintf(&available_users_str[3*i + offset], "- ");
        offset += 2;
    }
    sprintf(&available_users_str[3*i + offset], "%.2x ", available_users[i]);
  }

  available_users_str[str_len - 1] = 0x00;
  ESP_LOGI("NVS", "available_users : %s", available_users_str);
  free(available_users);
}

esp_err_t write_tag(const char *rfid_tag, const char *value) {
  /*
   * Write an rfid tag to the nvs.
   * The associated value can be a user name, a badge function...
   */
  esp_err_t err = nvs_set_str(my_nvs_handle, rfid_tag, value);
  // Commit tag
  if (err != ESP_OK) {
    ESP_LOGI("NVS", "Error at write_tag : %.3x", err);
    return err;
  }

  char* available_users;
  size_t available_users_len = get_available_users(&available_users);

  char new_available_users[available_users_len + 5];
  strcpy(new_available_users, available_users);
  strcat(new_available_users, rfid_tag);

  free(available_users);

  nvs_set_str(my_nvs_handle, "available_users", new_available_users);

  log_available_users();

  return nvs_commit(my_nvs_handle);
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
  esp_err_t err = nvs_get_str(my_nvs_handle, rfid_tag, NULL, &value_length);
  // char* read_value = (char *) malloc(value_length);
  // free(output_value);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    return INVALID_TAG;
  }
  else if (err == ESP_OK) {
    // The tag is valid, and the associated value is stored in output_value
    // if (read_value != NULL) {
    //   nvs_get_str(my_nvs_handle, rfid_tag, read_value, &value_length);
    //   // free(output_value);
    //   output_value = read_value;
    // }
    if (output_value != NULL) {
      *output_value = (char *) malloc(value_length);
      nvs_get_str(my_nvs_handle, rfid_tag, *output_value, &value_length);
      // free(output_value);
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
  esp_err_t err = nvs_erase_key(my_nvs_handle, rfid_tag);
  if (err != ESP_OK) {
    ESP_LOGI("NVS", "Error at delete_tag : %i", err);
    return err;
  }
  return nvs_commit(my_nvs_handle);
}

esp_err_t delete_all_tags() {
  /*
   * Remove all tags.
   */
  esp_err_t err = nvs_erase_all(my_nvs_handle);
  if (err != ESP_OK) {
    return err;
  }
  return nvs_commit(my_nvs_handle);
}

esp_err_t erase_nvs_partition() {
  /*
   * Remove all tags.
   */
  esp_err_t err = nvs_flash_erase();
  if (err != ESP_OK) {
    ESP_LOGI("NVS", "Error at erase_nvs_partition : %i", err);
  }
  return err;
}

int getUsersCount() {
  // int user_len;
  // nvs_get_used_entry_count(my_nvs_handle, (size_t*) &user_len);
  // return user_len;
  char* available_users;
  size_t available_users_len = get_available_users(&available_users) - 1;
  free(available_users);
  return available_users_len / 4;
}

void getUsers(User** users) {
  // users is suposed to have been allocated a sufficient amount of memory.
  // You can use getUsersCount() to do so.

  char* available_users;
  size_t available_users_len = get_available_users(&available_users) - 1; // -1 to remove the final 0

  size_t user_count = available_users_len / 4;
  for (int j = 0; j < user_count; j++) {
    char rfid[5];
    for (int i = 0; i < 4; i++) {
      rfid[i] = available_users[4 * j + i];
    }
    rfid[4] = 0x00;
    ESP_LOGI("NVS", "User rfid : %.2x %.2x %.2x %.2x"
                        ,rfid[0]
                        ,rfid[1]
                        ,rfid[2]
                        ,rfid[3]);
    // users[j].setRfid(rfid);
    size_t name_length;
    nvs_get_str(my_nvs_handle, rfid, NULL, &name_length); // set name_length
    ESP_LOGI("NVS", "User name length : %i", name_length);

    char name[name_length];
    nvs_get_str(my_nvs_handle, rfid, name, &name_length);
    // users[j].setName(name);

    User* testUser = new User(rfid, name);
    ESP_LOGI("TEST-USERS", "User rfid : %.2x, %.2x, %.2x, %.2x",
      testUser->getRfid()[0],
      testUser->getRfid()[1],
      testUser->getRfid()[2],
      testUser->getRfid()[3]);
    ESP_LOGI("TEST-USERS", "User name : %s", testUser->getName());
    users[j] = testUser;
    ESP_LOGI("CHECK-USERS", "User rfid : %.2x, %.2x, %.2x, %.2x",
      users[0]->getRfid()[0],
      users[0]->getRfid()[1],
      users[0]->getRfid()[2],
      users[0]->getRfid()[3]);
    ESP_LOGI("CHECK-USERS", "User name : %s", users[0]->getName());
    delete testUser;
  }
  
  free(available_users);
}
