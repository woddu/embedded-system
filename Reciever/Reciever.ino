/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

uint8_t led1pin = 22;
uint8_t led2pin = 21;
uint8_t led3pin = 19;
uint8_t led4pin = 18;
uint8_t led5pin = 5;
uint8_t led6pin = 17;
uint8_t led7in = 16;
uint8_t led8pin = 4;
uint8_t led9pin = 2;
uint8_t led10pin = 15;

struct led{
  int lednum;
  int ledstate;
};

led led1 = {1,0};
led led2 = {2,0};
led led3 = {3,0};
led led4 = {4,0};
led led5 = {5,0};
led led6 = {6,0};
led led7 = {7,0};
led led8 = {8,0};
led led9 = {9,0};
led led10 = {10,0};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  int lednum;
  memcpy(&lednum, incomingData, sizeof(int));
  switch (lednum){
    case 1:
      memcpy(&led1, incomingData, sizeof(led1));
      Serial.print("lednum = ");
      Serial.print(led1.lednum);
      Serial.print("lednum = ");
      Serial.print(led1.ledstate);
      Serial.println();
      break;
    case 2:
      memcpy(&led2, incomingData, sizeof(led2));
      Serial.print("lednum = ");
      Serial.print(led2.lednum);
      Serial.print("lednum = ");
      Serial.print(led2.ledstate);
      Serial.println();
      break;
    case 3:
      memcpy(&led3, incomingData, sizeof(led3));
      Serial.print("lednum = ");
      Serial.print(led3.lednum);
      Serial.print("lednum = ");
      Serial.print(led3.ledstate);
      Serial.println();
      break;
    case 4:
      memcpy(&led4, incomingData, sizeof(led4));
      Serial.print("lednum = ");
      Serial.print(led4.lednum);
      Serial.print("lednum = ");
      Serial.print(led4.ledstate);
      Serial.println();
      break;
    case 5:
      memcpy(&led5, incomingData, sizeof(led5));
      Serial.print("lednum = ");
      Serial.print(led5.lednum);
      Serial.print("lednum = ");
      Serial.print(led5.ledstate);
      Serial.println();
      break;
    case 6:
      memcpy(&led6, incomingData, sizeof(led6));
      Serial.print("lednum = ");
      Serial.print(led6.lednum);
      Serial.print("lednum = ");
      Serial.print(led6.ledstate);
      Serial.println();
      break;
    case 7:
      memcpy(&led7, incomingData, sizeof(led7));
      Serial.print("lednum = ");
      Serial.print(led7.lednum);
      Serial.print("lednum = ");
      Serial.print(led7.ledstate);
      Serial.println();
      break;
    case 8:
      memcpy(&led8, incomingData, sizeof(led8));
      Serial.print("lednum = ");
      Serial.print(led8.lednum);
      Serial.print("lednum = ");
      Serial.print(led8.ledstate);
      Serial.println();
      break;
    case 9:
      memcpy(&led9, incomingData, sizeof(led9));
      Serial.print("lednum = ");
      Serial.print(led9.lednum);
      Serial.print("lednum = ");
      Serial.print(led9.ledstate);
      Serial.println();
      break;
    case 10:
      memcpy(&led10, incomingData, sizeof(led10));
      Serial.print("lednum = ");
      Serial.print(led10.lednum);
      Serial.print("lednum = ");
      Serial.print(led10.ledstate);
      Serial.println();
      break;
    
    default:
      Serial.println("Unknown");
      break;
  }
}

void leds(void *param){
  while(1){
    digitalWrite(led1pin, led1.ledstate);
    digitalWrite(led2pin, led2.ledstate);
    digitalWrite(led3pin, led3.ledstate);
    digitalWrite(led4pin, led4.ledstate);
    digitalWrite(led5pin, led5.ledstate);
    digitalWrite(led6pin, led6.ledstate);
    digitalWrite(led7pin, led7.ledstate);
    digitalWrite(led8pin, led8.ledstate);
    digitalWrite(led9pin, led9.ledstate);
    digitalWrite(led10pin, led10.ledstate);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(led1pin, OUTPUT);
  pinMode(led2pin, OUTPUT);
  pinMode(led3pin, OUTPUT);
  pinMode(led4pin, OUTPUT);
  pinMode(led5pin, OUTPUT);
  pinMode(led6pin, OUTPUT);
  pinMode(led7pin, OUTPUT);
  pinMode(led8pin, OUTPUT);
  pinMode(led9pin, OUTPUT);
  pinMode(led10pin, OUTPUT);

  xTaskCreatePinnedToCore(
    leds,
    "leds",
    5000,
    NULL,
    1,
    NULL,
    0
  );

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}
