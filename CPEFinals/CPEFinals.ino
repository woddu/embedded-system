#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include "DHT.h"
const int oneWireBus = 5;
const int buzzer1 = 26;
const int buzzer2 = 18;
int val, temp, humi, dstmp;
int tpos,hpos,dspos;
bool error[] = {false, false};
bool buzzer[] = {0, 0};
DHT dht(22, DHT11);

OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

Servo tservo;
Servo hservo;
Servo dsservo;

unsigned long prevMillis = 0;
const long servo = 125;
const long buzzer1i = 500;
const long buzzer2i = 150;


void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
  dht.begin();
  sensors.begin();
  tservo.attach(32, 500, 2400);
  hservo.attach(4, 500, 2400);
  dsservo.attach(2, 500, 2400);
  analogWrite(buzzer1, 190);
  vTaskDelay(125 / portTICK_PERIOD_MS);
  analogWrite(buzzer1, 100);
  vTaskDelay(125 / portTICK_PERIOD_MS);
  analogWrite(buzzer1, 140);
  vTaskDelay(125 / portTICK_PERIOD_MS);
  analogWrite(buzzer1, 0);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currMillis = millis();
  if (currMillis - prevMillis >= servo){
    sensors.requestTemperatures();
    dstmp = sensors.getTempCByIndex(0);
    temp = dht.readTemperature();
    humi = dht.readHumidity();
    tpos = map(temp , 20, 50, 129, 31);
    hpos = map(humi , 30, 90, 40, 135);
    dspos = map(dstmp , 15, 80, 157, 25);
    if (temp >= 20 && temp <= 50){
      error[0] = false;
      tservo.write(tpos);
    } else if (temp < 20 && temp >0){
      error[0] = false;
      tservo.write(129);
    }else if (temp > 50 && temp < 100){
      tservo.write(31);
      error[0] = false;
    } else if (temp > 100 || humi > 110) {
      error[0] = true;
    }
     if (humi >= 30 && humi <= 90){
      hservo.write(hpos);
    } else if (humi < 30 && humi > 0){
      hservo.write(40);
    }else if (humi > 90 && humi < 110){
      hservo.write(135);
    }
     if (dstmp >= 15 && dstmp <= 80){
      error[1] = false;
      dsservo.write(dspos);
    } else if (dstmp < 15 && dstmp > 10){
      error[1] = false;
      dsservo.write(157);
    }else if (dstmp > 80 && dstmp < 100){
      error[1] = false;
      dsservo.write(25);
    } else if (dstmp < 0) {
      error[1] = true;
    }
    //Serial.print(temp);
    //Serial.print("  ");
    //Serial.print(error[0]);
    //Serial.print(" C  = ");
    //Serial.println(tpos);
    //Serial.print(humi);
    //Serial.print(" H  = ");
    //Serial.println(hpos);
    //Serial.print(dstmp);
    //Serial.print(" ");
    //Serial.print(error[1]);
    //Serial.print(" C  = ");
    //Serial.println(dspos);
  }
  if (error[0]){
    if (currMillis - prevMillis >= buzzer1i){
      if (buzzer[0]){
        buzzer[0] = false;
        analogWrite(buzzer1, 170);
      } else {
        analogWrite(buzzer1, 100);
        buzzer[0] = true;
      }
    }
  } else { analogWrite(buzzer1, 0);}
  if (error[1]){
    if (currMillis - prevMillis >= buzzer1i){
      if (buzzer[1]){
        buzzer[1] = false;
        analogWrite(buzzer2, 210);      
      } else {
        analogWrite(buzzer2, 95);
        buzzer[1] = true;
      }
    }
  } else { analogWrite(buzzer2, 0);}
}
