#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD2004

int threshold = 540;

int leds[] = {2,3,4,7,6};

const int pin = A1;
const int button = 8;
const int pot = A2;

unsigned long prev1 = 0;
unsigned long prev2 = 0;
unsigned long prev3 = 0;

const long t1 = 1000;

int val, potval;
int level, lastlevel;

void setup() {
  lcd.backlight();
  lcd.begin(16, 2);
  lcd.print("HAPPY BIRTHDAY!!!");
  
  Serial.begin(9600); // Initialize Serial Monitor
  
  for (int i = 0; i < 5; i++){
  pinMode(leds[i], OUTPUT);
  }

  pinMode(pin, INPUT);
  pinMode(button, INPUT);
  

  delay(1000);
}

void loop(){
  

  unsigned long curr = millis();
  if (curr - prev1 >= t1/20){
    prev1 = curr;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Decibel Detected");

    val = analogRead(pin);

    lcd.setCursor(0, 1);
    lcd.print("val=");
    lcd.print(val);
    lcd.print("    "); // Clear any extra characters from the previous reading
  
    if (val <= threshold) { 
      lcd.setCursor(0, 2);
      lcd.print("               ");
    } else {
      lcd.setCursor(0, 2);
      lcd.print("Sound Detected");
    }

    if (digitalRead(button) == HIGH){
      potval = analogRead(pot);
      level = map(potval, 0, 1020, 0, 5);
      Serial.println(potval);
      Serial.println(level);
    } else{
      if (val >= 620){
        level = 5;
      } else if (val >=600 && val <620){
        level = 4;
      } else if (val >=580 && val <600){
        level = 3;
      } else if (val >=560 && val <580){
        level = 2;
      } else if (val >=540 && val <560){
        level = 1;
      } else if (val < 540){
        level = 0;
      }
    }

  }
  if (curr - prev2 >= t1/5){
    prev2 = curr;
    Serial.print("lastlevel = ");
    Serial.print(lastlevel);
    Serial.print("   level = ");
    Serial.println(level);
    if (lastlevel - level < 0){
      if (curr - prev3 >= t1/5){
        prev3 = curr;
        for (int i = lastlevel; i < level; i++){
          digitalWrite(leds[i], HIGH);
          delay(25);
        }
      }
    } else if (lastlevel - level > 0){
      if (curr - prev3 >= t1/5){
        prev3 = curr;
        for (int i = level; i > lastlevel; i--){
          digitalWrite(leds[i-1], LOW);
          delay(25);
        }
      }
    } else if (level == 0){
      if (curr - prev3 >= t1/5){
        prev3 = curr;
        for (int i = 5; i > -1; i--){
          digitalWrite(leds[i-1], LOW);
          delay(25);
        } 
      }
    }
    lastlevel = level;
  }
  if (curr - prev3 >= t1/2){
    prev3 = curr;
  }
}



