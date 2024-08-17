
#include <Arduino.h>
#include <Preferences.h>

#define ID 'T'

struct tarheta {
  uint8_t id;
  String name;
  uint8_t group;
  bool male;
  bool absent;
  bool isMS;
  bool isIID;
};

Preferences preferences;

int currentID;

String toID(int id){
  return String(ID) + String(id);
}

void AddTarheta(String name, int group, bool male, bool absent, bool isMS, bool isIID){

  tarheta temp;
  temp.name = name;
  temp.group = group;
  temp.male = male;
  temp.absent = absent;
  temp.isMS = isMS;
  temp.isIID = isIID;
  
  preferences.begin("Purok 8", false);
  if (!preferences.isKey("CurrentID")){
    preferences.putInt("CurrentID", 0);
    currentID = preferences.getInt("CurrentID");
  } else {
    currentID = preferences.getInt("CurrentID");
    currentID++;
  }
  preferences.putBytes(toID(currentID).c_str(), &temp, sizeof(temp));
  preferences.end();
}

void EditTarheta(tarheta * tarheta){

}
// preferences.begin("Purok 8", false);
// if (!preferences.isKey("CurrentID")){
//   preferences.putInt("CurrentID", 0);
//   Serial.println("no currID");
// }
// int two = 2;
// if (!preferences.isKey(toID(two).c_str())){
//   Serial.println("no " + toID(two));
// }
// preferences.end();