#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <Servo.h>

Servo servo;

VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.
Servo myservo;

uint8_t records[7]; // save record
uint8_t buf[64];

void setup() {
  // put your setup code here, to run once:
  myVR.begin(9600);
  Serial.begin(115200);
  
  myservo.attach(9);

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
        servo.write(180);
        myVR.clear();
        for(int i = 0; i < 6; i++){
          myVR.load(uint8_t(i));
        } wake1 = wake2 = false;
      } else if (buf[1] >= 9 && buf[1] <= 10){
        Serial.println("second");
        servo.write(90);
        myVR.clear();
        for(int i = 0; i < 6; i++){
          myVR.load(uint8_t(i));
        } wake1 = wake2 = false;
      }  else if (buf[1] >= 12 && buf[1] <= 13){
        Serial.println("third");
        servo.write(0);
        myVR.clear();
        for(int i = 0; i < 6; i++){
          myVR.load(uint8_t(i));
        } wake1 = wake2 = false;
      }
    }
  }
}
