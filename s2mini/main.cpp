#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"
#include "ESP32Servo.h"
#include "Preferences.h"

#define WIFI_SSID         "RonGods"    
#define WIFI_PASS         "spiderron"
#define APP_KEY           "fdb7f13c-ab22-41a4-a6c1-9c1d1574e40e"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "9b4fd0cb-1e45-49b7-b064-57f43ede218d-057932c1-7859-49f4-830e-c35916465f84"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"

#define BACKYARD       "663b7c816443b9bfe2b63d9b"    // Should look like "5dc1564130xxxxxxxxxxxxxx"

#define BAUD_RATE         115200                // Change baudrate to your need

const char* PARAM_SWITCH= "switch";

// the number of the LED pin
const int led1 = 3;  // 16 corresponds to GPIO16
const int led2 = 5;  // 16 corresponds to GPIO16
const int led3 = 7;  // 16 corresponds to GPIO16
const int servoPin = 9;
const int solenoid = 11;  // 16 corresponds to GPIO16
const int pump = 12;  // 16 corresponds to GPIO16
const int irOutside = 18;
const int irInside = 16;

// setting PWM properties
const int freq = 5000;
const int resolution = 8;

bool whiteLeds, poolLeds, moving1;

int openGarage1, lastGarage1;

unsigned long tOpened;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

Preferences pref;

Servo myservo;

int prev[2], curr[2];

bool onPowerStateBackyard(const String &deviceId, bool &state) {
  Serial.printf("Device 1 turned %s\r\n", state?"on":"off");
  if (state){    
    whiteLeds = true;
  } else {
    whiteLeds = false;
  }
  return true; // request handled properly
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP); 
    WiFi.setAutoReconnect(true);
  #elif defined(ESP32)
    WiFi.setSleep(false); 
    WiFi.setAutoReconnect(true);
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASS); 

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }

  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// setup function for SinricPro
void setupSinricPro() {
  // add devices and callbacks to SinricPro
  SinricProSwitch& mySwitch1 = SinricPro[BACKYARD];
  mySwitch1.onPowerState(onPowerStateBackyard);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void WhiteLight(void * param){
  while(1){
    if (whiteLeds){
      for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(4, dutyCycle);
        delay((whiteLeds ? 15 : 0));
      }
      // decrease the LED brightness
      for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
        // changing the LED brightness with PWM
        ledcWrite(4, dutyCycle);   
        delay((whiteLeds ? 15 : 0));
      }
    } else {
      ledcWrite(0, 0);   
      delay(10);
    }
  }
}

void Garage1(void * param){
  while(1){
    if(openGarage1 == 1){      
      if (!moving1){    
        moving1 = true;
        int a = 0;
        while (a <= 90){
          myservo.write(a);                             
          delay(20);
          a++;
        }
        moving1 = false;
        openGarage1 = 0;
        lastGarage1 = 1;
      }
      pref.putBool("GarageDoor1", openGarage1);
    } else if(openGarage1 == 2) {      
      if (!moving1){    
        moving1 = true;
        int a = 90;
        while (a >= 0){
          myservo.write(a);                    
          delay(20);
          a--;
        }
        moving1 = false;
        openGarage1 = 0;
        lastGarage1 = 2;
      }
      pref.putBool("GarageDoor1", openGarage1);
    }
    delay(10);
  }
}

void PoolLED(void * param){
  while(1){
    if(poolLeds){
      for(int dutyCycle = 10; dutyCycle <= 255; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(5, dutyCycle);
        delay((poolLeds ? 25 : 0));
      }
      // decrease the LED brightness
      for(int dutyCycle = 255; dutyCycle >= 10; dutyCycle--){
        // changing the LED brightness with PWM
        ledcWrite(5, dutyCycle);   
        delay((poolLeds ? 25 : 0));
      }
    } else {
      ledcWrite(5, 0);
      delay(10);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  delay(1000);
  ledcSetup(4, freq, resolution);
  ledcSetup(5, freq, resolution);
  pinMode(led2, OUTPUT);
  pinMode(solenoid, OUTPUT);
  pinMode(pump, OUTPUT);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(led1, 4);
  ledcAttachPin(led3, 5);
  
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);          // Standard 50hz servo
  myservo.attach(servoPin, 500, 2400);
  pref.begin("Backyard", false);

  xTaskCreate(
    WhiteLight,
    "White Light",
    5000,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    Garage1,
    "Garage1",
    8000,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    PoolLED,
    "Pool LED",
    5000,
    NULL,
    1,
    NULL
  );

  openGarage1 = pref.getBool("GarageDoor1");  

  setupWiFi();

  server.on("/garden", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String num;
    // GET input1 value on <ESP_IP>/led?num=<num>
    if (request->hasParam(PARAM_SWITCH) ) {
      num = request->getParam(PARAM_SWITCH)->value();
      Serial.println(num);
      if (num == "1on"){
        whiteLeds = true;
      } else if (num == "1off"){
        whiteLeds = false;
      } else if (num == "2on"){
        digitalWrite(led2, HIGH);
      } else if (num == "2off"){
        digitalWrite(led2, LOW);
      } else if (num == "3on"){
        poolLeds = true;
      } else if (num == "3off"){
        poolLeds = false;
      } else if (num == "4on"){
        digitalWrite(solenoid, HIGH);
      } else if (num == "4off"){
        digitalWrite(solenoid, LOW);        
      } else if (num == "5on"){
        if (lastGarage1 != 1){
          openGarage1 = 1;
        }
      } else if (num == "5off"){
        if (lastGarage1 != 2){
          openGarage1 = 2;
        }
      } else if (num == "6on"){
        digitalWrite(pump, HIGH);
      } else if (num == "6off"){
        digitalWrite(pump, LOW);        
      } else if (num == "allon"){
        whiteLeds = true;
        poolLeds = true;
        digitalWrite(led2, HIGH);
        digitalWrite(solenoid, HIGH);
        digitalWrite(pump, HIGH); 
      } else if (num == "alloff"){
        whiteLeds = false;
        poolLeds = false;
        digitalWrite(led2, LOW);
        digitalWrite(solenoid, LOW);
        digitalWrite(pump, LOW);
      }
    } else {
      num = "No message sent";
    }
    request->send(200, "text/plain", num);
  }
  );

  server.begin();
  setupSinricPro();

}

void loop() {
  // put your main code here, to run repeatedly:
  SinricPro.handle();
}

