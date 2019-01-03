#include "user.h"
#include "esp_log.h"

User::User() {
  this->rfid = nullptr;
  this->name = nullptr;
}
User::User(char* rfid, char* name) {
  this->rfid = rfid;
  this->name = name;
}

void User::setRfid(char* rfid) {
  this->rfid = rfid;
}

char* User::getRfid() {
  return rfid;
}

void User::setName(char* name) {
  this->name = name;
}

char* User::getName() {
  return name;
}

User::~User() {
  free(this->name);
  free(this->rfid);
}
