#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <Stepper_28BYJ_48.h>
#include <EEPROM.h>

#define ROTATE 530 
Stepper_28BYJ_48 stepper( 4, 5, 6, 7);

VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

uint8_t records[7]; // save record\
uint8_t buf[64];

int currentFloor = 1;

bool moving, up,down;

void goToFloor(int floor){
  int flor;
  if(floor > currentFloor && !moving){
    flor = floor;            
    up = true;
    down = !up;
    moving = true;
    for (int i = 0; i < floor - currentFloor;i++){
      stepper.step(-ROTATE);
    }            
    currentFloor = flor;
    moving = false;
  } else if (floor < currentFloor && !moving){
    flor = floor;            
    down = true;
    up = !down;
    moving = true;             
    for (int i = 0; i < currentFloor - floor;i++){
      stepper.step(ROTATE);
    }
    currentFloor = flor;
    moving = false; 
  }
  EEPROM.write(0, currentFloor);
}


void setup() {
  // put your setup code here, to run once:
  myVR.begin(9600);
  Serial.begin(115200);
    
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  for(int i = 0; i < 6; i++){
    myVR.load(uint8_t(i));
  }  
  currentFloor =  EEPROM.read(0);
  Serial.println(currentFloor);
}
// stepper.step(ROTATE);
// stepper.step(-ROTATE);
bool wake1, wake2;
void loop() {
  // put your main code here, to run repeatedly:
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    if (!wake1 || !wake2){
      if(buf[1] >= 0 && buf[1] <= 2){
        wake1 = true;
        Serial.println("dante");
        if (wake2){
          myVR.clear();
          myVR.load(uint8_t(6));
          myVR.load(uint8_t(7));
          myVR.load(uint8_t(9));
          myVR.load(uint8_t(10));
          myVR.load(uint8_t(12));
          myVR.load(uint8_t(13));
          Serial.println("1st - 3rd");
        }
      } else if (buf[1] >= 3 && buf[1] <= 5){
        Serial.println("jerome");
        wake2 = true;
        if (wake1){
          myVR.clear();
          myVR.load(uint8_t(6));
          myVR.load(uint8_t(7));
          myVR.load(uint8_t(9));
          myVR.load(uint8_t(10));
          myVR.load(uint8_t(12));
          myVR.load(uint8_t(13));
          Serial.println("1st - 3rd");
        }
      }
    } else if(wake1 && wake2) {
      if(buf[1] >= 6 && buf[1] <= 7){
        Serial.println("first");
        goToFloor(1);
        myVR.clear();
        for(int i = 0; i < 6; i++){
          myVR.load(uint8_t(i));
        } wake1 = wake2 = false;
      } else if (buf[1] >= 9 && buf[1] <= 10){
        Serial.println("second");
        goToFloor(2);
        myVR.clear();
        for(int i = 0; i < 6; i++){
          myVR.load(uint8_t(i));
        } wake1 = wake2 = false;
      }  else if (buf[1] >= 12 && buf[1] <= 13){
        Serial.println("third");
        goToFloor(3);        
        myVR.clear();
        for(int i = 0; i < 6; i++){
          myVR.load(uint8_t(i));
        } wake1 = wake2 = false;
      }
    }
  }
}
