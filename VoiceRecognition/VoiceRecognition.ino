/**
  ******************************************************************************
  * @file    vr_sample_control_led.ino
  * @author  JiapengLi
  * @brief   This file provides a demostration on 
              how to control led by using VoiceRecognitionModule
  ******************************************************************************
  * @note:
        voice control led
  ******************************************************************************
  * @section  HISTORY
    
    2013/06/13    Initial version.
  */
  
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <Servo.h>
/**        
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.
Servo myservo;
uint8_t records[7]; // save record
uint8_t buf[64];
const int LED1 = 6, LED2 = 7;

void setup()
{
  /** initialize */
  myVR.begin(9600);
  
  myservo.attach(9);

  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }  
  for (int i = 4; i < 9; i++){
    myVR.load(uint8_t(i));
  }
}
bool led1,led2,servo;
void loop(){
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    Serial.println(buf[1], DEC);
    Serial.print(servo);
    Serial.print(led1);
    Serial.println(led2);
    if(!servo && !led1 && !led2){            
      if (buf[1] >= 4 && buf[1] <= 5){
        if (!servo && !led1 && !led2){
            led1 = true;
            servo = led2 = false;
            myVR.clear();
            for (int i = 0; i < 4; i++){
              myVR.load(uint8_t(i));
            }
          }Serial.println("led1");
        } else if (buf[1] >= 6 && buf[1] <= 7){
          if (!servo && !led1 && !led2){
              led2 = true;
              servo = led1 = false;
              myVR.clear();
              for (int i = 0; i < 4; i++){
                myVR.load(uint8_t(i));
              }
          }Serial.println("led2");
        } else if (buf[1] >= 8 && buf[1] <= 9){      
          if (!servo && !led1 && !led2){
            servo = true;
            led2 = led1 = false;myVR.clear();
            for(int i = 10; i < 14;i++){
              myVR.load(uint8_t(i));
            }
          }Serial.println("servo");
        }        
    }else{
        if (servo){
          if (buf[1] >= 10 && buf[1] <= 11){
            myservo.write(180);
            Serial.println("180");
          } else if (buf[1] >= 12 && buf[1] <= 13){
            myservo.write(0);
            Serial.println("0");
          }      
          myVR.clear();
        for (int i = 4; i < 9; i++){
          myVR.load(uint8_t(i));
        }
          servo = false;
        }else if(led1){
          if (buf[1] >= 0 && buf[1] <= 1){
            digitalWrite(LED1, HIGH);
            Serial.println("led1 on");
          } else if (buf[1] >= 2 && buf[1] <= 3){
            digitalWrite(LED1, LOW);
            Serial.println("led1 off");
          } 
          myVR.clear();
        for (int i = 4; i < 9; i++){
          myVR.load(uint8_t(i));
        }
          led1 = false;
        }else if(led2){
          if (buf[1] >= 0 && buf[1] <= 1){
            digitalWrite(LED2, HIGH);
            Serial.println("led2 on");
          } else if (buf[1] >= 2 && buf[1] <= 3){
            digitalWrite(LED2, LOW);
            Serial.println("led2 off");
          }
          myVR.clear();
        for (int i = 4; i < 9; i++){
          myVR.load(uint8_t(i));
        }
          led2 = false;    
        }                
    }
  }
}



