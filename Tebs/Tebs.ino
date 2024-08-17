#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans12pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int sensorPin = 29;
int buzzerPin = 8;
int red = 6;
int green = 7;
int val,level1,level2;

unsigned long prev;
const long interval = 250;
void setup() {
  // put your setup code here, to run once:
  pinMode(sensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextWrap(true);
  
}
bool buzz;
void loop() {
  // put your main code here, to run repeatedly:
    unsigned long curr = millis();
  val = analogRead(sensorPin);
  level1 = map(val, 340, 370, 255, 0);
  level2 = map(val, 370, 430, 0, 255);
  display.clearDisplay();
  display.setCursor(5,(val < 400 ? 40 : 24));
  display.setFont(&FreeSans12pt7b);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print((val < 400 ? "No Smoke" : "Smoke \n Detected"));
  display.display();
  if (val < 315){
    analogWrite(green, 255);
    analogWrite(red, 0);
  } else if (val > 430){
    analogWrite(red, 255);
    analogWrite(green, 0);
  } else if (val >= 315 && val <= 430){
    analogWrite(red, level2);
    analogWrite(green, level1);
  }
  if (val >= 400){
    if (curr - prev >= interval){
      prev = curr;
      buzz = !buzz;
      digitalWrite(buzzerPin, (buzz ? 1: 0));
    }
  } else {
    digitalWrite(buzzerPin, 0);
  }
  delay(10);
}
