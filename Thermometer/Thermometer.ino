#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


int temp, humid;
DHT dht(23, DHT11);



void OLED(void *param){
  while (1){
    temp = dht.readTemperature();
    humid = dht.readHumidity();
    if (temp >= 0 && temp <= 60){
    display.clearDisplay();
    display.setTextWrap(true);
    display.setCursor(2, 4);
    display.setTextSize(2);
    display.print(temp);
    display.print(" ");
    display.print((char)247);
    display.print("C");
    display.print("  ");
    display.print(humid);
    display.print(" ");
    display.print("H");
    display.display();
    }
    else{
    display.clearDisplay();
    display.setTextWrap(true);
    display.setCursor(2, 4);
    display.setTextSize(2);
    display.println("Error");
    display.setTextSize(1);
    display.print(temp);
    display.display();
    }
  }
}



void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  dht.begin();
  // initialize the display
  // note you may have to change the address
  // the most common are 0X3C and 0X3D

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // if you distribute your code, the adafruit license requires you show their splash screen
  // otherwise clear the video buffer memory then display
  display.display();
  //delay(2000);
  display.setRotation(0);
  display.clearDisplay();
  display.display();
    display.setTextColor(SSD1306_WHITE);
  // establish whatever pin reads you need
  temp = dht.readTemperature();
    humid = dht.readHumidity();
    display.setTextWrap(true);
    display.setCursor(2, 4);
    display.setTextSize(1);
    display.print(temp);
    display.print(" ");
    display.print((char)247);
    display.print("C");
    display.print("  ");
    display.print(humid);
    display.print(" ");
    display.print("H");
    display.display();
   xTaskCreatePinnedToCore(
    OLED,
    "OLED display",
    5000,
    NULL,
    1,
    NULL,
    0
  );
}



void loop(void) {


  }