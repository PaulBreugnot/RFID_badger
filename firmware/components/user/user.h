
class User {
private:
  char* rfid;
    char* name;

public:
  User();
  User(char* rfid, char* name);

  void setRfid(char* rfid);
  char* getRfid();

  void setName(char* name);
  char* getName();
  ~User();
};
