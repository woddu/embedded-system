#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else

#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"
#include "USBHIDSystemControl.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiAP.h>

// Replace with your network credentials
const char* ssid = "S2-Mini";
const char* password = "123456789";

AsyncWebServer server(80);

const char* PARAM_M = "move";
const char* PARAM_K = "type";
const char* PARAM_S = "s";

USBHIDSystemControl SystemControl;
USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;

bool left1, right1, up1, down1;

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() { // initialize the buttons' inputs:
  Serial.begin(115200);  
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while(1);
  }
  
  // Print ESP Local IP Address


  SystemControl.begin();
  Mouse.begin();
  Keyboard.begin();
  USB.begin();

  server.on("/m", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String move;
    if (request->hasParam(PARAM_M)) {
      move = request->getParam(PARAM_M)->value();
      Serial.println(move);
      if(move == "left") {
        Mouse.move(-5, 0);
      } else if(move == "left2"){
        left1 = false;
      } else if(move == "right2"){
        right1 = false;
      } else if(move == "left1"){
        left1 = true;
      } else if(move == "right1"){
        right1 = true;
      } else if(move == "right"){
        Mouse.move(5, 0);
      } else if(move == "up") {
        Mouse.move(0,-5);
      } else if(move == "down") {
        Mouse.move(0, 5);
      } else if(move == "up2"){
        up1 = false;
      } else if(move == "down2"){
        down1 = false;
      } else if(move == "up1"){
        up1 = true;
      } else if(move == "down1"){
        down1 = true;
      } else if(move == "click") {
        Mouse.press(MOUSE_LEFT);
        Mouse.release(MOUSE_LEFT);
      } 
    }          
  });

  server.on("/k", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String type;
    if (request->hasParam(PARAM_K)) {
      type = request->getParam(PARAM_K)->value();
      Serial.println(type);
      Keyboard.print(type);
    }              
  });
  
  server.on("/s", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String s;
    if (request->hasParam(PARAM_S)) {
      s = request->getParam(PARAM_S)->value();
      Serial.println(s);
      if (s == "true"){
        SystemControl.press(SYSTEM_CONTROL_POWER_OFF);
        SystemControl.release();
      }
    }              
  });
  server.onNotFound(notFound);
  server.begin();
  delay(1000);
  Serial.println( WiFi.softAPIP());
}

void loop() {
  // use serial input to control the mouse:
  // use the pushbuttons to control the keyboard:
  if (left1){
    Mouse.move(-5, 0);
  } else if (right1){
    Mouse.move(5, 0);
  } else if (up1){
    Mouse.move(0, -5);
  } else if (down1){
    Mouse.move(0, 5);
  }
  delay(62);
}
#endif /* ARDUINO_USB_MODE */
