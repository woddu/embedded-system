#include <ESP32Servo.h>

const int potpin = 36;
int val, temp, humi, dstmp;
int tpos,hpos,dspos;

Servo tservo;
Servo hservo;
Servo dsservo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tservo.attach(14, 500, 2400);
  hservo.attach(27, 500, 2400);
  dsservo.attach(16, 500, 2400);
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(potpin);
  temp = map(val , 0, 4095, 20, 50);
  tpos = map(temp , 20, 50, 129, 31);
  Serial.print(temp);
  Serial.print(" C  = ");
  Serial.println(tpos);
  if (temp >= 20 && temp <= 50){
    tservo.write(tpos);
  } else if (temp < 20){
    tservo.write(135);
  }else if (temp > 50){
    tservo.write(45);
  }
  humi = map(val , 0, 4095, 30, 90);
  hpos = map(humi , 30, 90, 40, 135);
  Serial.print(humi);
  Serial.print(" H  = ");
  Serial.println(hpos);
   if (humi >= 30 && humi <= 90){
    hservo.write(hpos);
  } else if (humi < 30){
    hservo.write(135);
  }else if (humi > 90){
    hservo.write(45);
  }
  dstmp = map(val , 0, 4095, 15, 80);
  dspos = map(dstmp , 15, 80, 157, 25);
  Serial.print(dstmp);
  Serial.print(" C  = ");
  Serial.println(dspos);
   if (dstmp >= 15 && dstmp <= 80){
    dsservo.write(dspos);
  } else if (dstmp < 15){
    dsservo.write(155);
  }else if (dstmp > 80){
    dsservo.write(25);
  }
  delay(250);
}
