#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#include <WiFi.h>
#include "time.h"

#define TFT_RST 4
#define TFT_DC  2
#define TFT_CS  15

const char* ssid     = "realme C35";
const char* password = "12345567";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 28800;
const int   daylightOffset_sec = 0;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tft.initR(INITR_BLACKTAB);
  tft.setTextWrap(true);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(tft.getRotation()+3);
  tft.setCursor(0, 20);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.println("Connecting to Wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }
  tft.println("");
  tft.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  displayTime();
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

}

void loop(void) {
  //// put your main code here, to run repeatedly:
  delay(1000);
  displayTime();
}

void displayTime(){
  tft.fillScreen(ST77XX_BLACK);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    tft.setCursor(0, 10);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(3);
    tft.println("Failed to obtain time");
    
    return;
  }
  tft.setCursor(0, 20);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(5.7);
  tft.println(&timeinfo, "%I:%M");
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.println(&timeinfo, "  %A");
  tft.println(&timeinfo, " %B %d ");
  tft.setTextColor(ST77XX_ORANGE);
  tft.println(&timeinfo, "  %H:%M:%S");
}
