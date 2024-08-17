#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <ESP32Servo.h>

#define TFT_RST 4
#define TFT_DC  2
#define TFT_CS  15

#define BACKCOLOR 0x18E3
#define SCALECOLOR 0xFFFF

const uint8_t oneWireBus = 5;
const uint8_t servo1Pin = 17;
const uint8_t servo2Pin = 16;

Servo servo1;
Servo servo2;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);


OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

float tempC;


int minUs = 500;
int maxUs = 2400;
int pos1;
int pos2;

const int analogIn = A0;
int RawValue = 0;
int LastPercent = 0;

// Global variables
  int valueBlock[500];
  int timeBlock[500];
  int locationBlock[500];
  int valuePos;
  int blockPos;
  int temp, humid;
  bool error;

  DHT dht(22, DHT11);
  
// Editable Variables

  uint16_t graphColor = ST77XX_BLUE;
  uint16_t pointColor = ST77XX_BLACK;
  uint16_t lineColor = ST77XX_GREEN;


  int graphRange = 50;
  int markSize = 3;
  
// Calculate Values
  const int numberOfMarks = 5;
  const int originX = 83;
  const int originY = 100;
  const int sizeX = 75;
  const int sizeY = 75;
  const int deviation = 15;
  
  int boxSize = (sizeX / numberOfMarks);
  int mark[] = {(boxSize + deviation), ((boxSize * 2) + deviation), ((boxSize * 3) + deviation), ((boxSize * 4) + deviation), ((boxSize * 5) + deviation), ((boxSize * 6) + deviation), ((boxSize * 7) + deviation), ((boxSize * 8) + deviation)};

  const int minorSizeY = (originY + 10);
  const int minorSizeX = (originX - 10);

  int numberSize = (sizeY / 6);
  int number[] = {numberSize, (numberSize * 2), (numberSize * 3), (numberSize * 4), (numberSize * 5), (numberSize * 6)};

  int numberValue = (graphRange / 6);
  int val[] = {graphRange, (numberValue * 5), (numberValue * 4), (numberValue * 3), (numberValue * 2), numberValue};

void drawHome()
{
  tft.fillScreen(BACKCOLOR);
  delay(500);
  
  tft.setCursor(60, 10); // set the cursor
  tft.setTextColor(ST77XX_WHITE); // set the colour of the text
  tft.setTextSize(2); // set the size of the text
  tft.print("Lab");
  
  tft.setCursor(47, 26); // set the cursor
  tft.print("6 - 9");

  tft.setTextColor(ST77XX_MAGENTA);
  tft.setCursor(2, 50);
  tft.print("Members: ");
  
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(0, 66);
  tft.setTextSize(1);
  tft.print("  Castro\n  Manocdoc\n  Narag\n  Salih\n  Sison");
  delay(4000);

  tft.fillScreen(ST77XX_WHITE);
  delay(500);
}

void drawGraph()
{
  // draw title
  tft.fillRect(75, 3, 82, 18, ST77XX_BLUE); // set the cursor
  tft.fillRect(92, 4, 32, 16, ST77XX_WHITE);
  tft.fillRect(149, 4, 7, 16, ST77XX_WHITE);
  tft.setCursor(150, 5);
  tft.setTextColor(ST77XX_BLUE); // set the colour of the text
  tft.setTextSize(1); // set the size of the text
  tft.print("H");
  tft.setCursor(101, 5);
  tft.setTextColor(ST77XX_BLUE); // set the colour of the text
  tft.setTextSize(2); // set the size of the text
  tft.print((char)247);
  tft.print("C");
  // draw outline
  tft.drawLine(originX, originY, (originX + sizeX), originY, graphColor);
  tft.drawLine(originX, originY, originX, (originY - sizeY), graphColor);

  // draw lables
  for(int i = 0; i < numberOfMarks; i++)
  {
    tft.drawLine(mark[i]+55, originY, mark[i]+55, minorSizeY, graphColor);
  }

  // draw numbers
  for(int i = 0; i < 6; i++)
  {
    tft.drawLine(originX-1, (originY - number[i]), minorSizeX+8, (originY - number[i]), graphColor);
  }
  int timeRange = 10;
  int adjustpixel = 0;
  for(int i = 0; i <= 4; i++)
    {
      tft.setCursor((mark[(4 - i)] - 5)+55 +adjustpixel, (originY + 16));
      tft.setTextColor(graphColor, ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println(timeRange);
      timeRange-=2;
      adjustpixel+=3;
    }

  // draw number values
  for(int i = 0; i < 6; i++)
  {
    tft.setCursor((minorSizeX - 30)+28, (number[i] + numberSize));
    tft.setTextColor(graphColor);
    tft.setTextSize(1);
    tft.println(val[i]);
  }
}

void graph()
{
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  temp = dht.readTemperature();
  humid = dht.readHumidity();
  
  tft.fillRect(75, 3, 25, 18, ST77XX_BLUE);
  tft.fillRect(124, 3, 25, 18, ST77XX_BLUE);
  if (temp > 50 || temp < 0){
  tft.fillRect((originX + 2), (originY - sizeY), sizeX, sizeY, ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setCursor((originX+20), (originY -50));
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.print("Too");
  tft.setCursor((originX+16), (originY -34));
  tft.print("High");
  error = true;
  }else{
  
  tft.setCursor(77, 5);
  tft.setTextColor(ST77XX_WHITE); // set the colour of the text
  tft.setTextSize(2); // set the size of the text
  tft.println(temp);
  if (humid < 99){
  tft.setCursor(126, 5);
  tft.setTextColor(ST77XX_WHITE); // set the colour of the text
  tft.setTextSize(2); // set the size of the text
  tft.println(humid);
  }

  timeBlock[valuePos] = ((millis() - 4500) / 1000);

  valueBlock[valuePos] = temp;
  if (error){
    tft.fillRect((originX + 2), (originY - sizeY), sizeX, sizeY, ST77XX_WHITE);
    tft.setTextWrap(true);
    error = false;
  }

  
  if(blockPos < 5)
  {
    // print the time
    //tft.setCursor((mark[valuePos] - 5)+56, (originY + 16));
    //tft.setTextColor(graphColor, ST77XX_WHITE);
    //tft.setTextSize(1);
    //tft.println(timeBlock[valuePos]);
    
    // map the value
    locationBlock[valuePos] = map(temp, 0, graphRange, originY, (originY - sizeY));

    // draw point
    tft.fillRect((mark[valuePos] - 1)+56, (locationBlock[valuePos] - 1), markSize, markSize, pointColor);

    // try connecting to previous point
    if(valuePos != 0)
    {
      tft.drawLine(mark[valuePos]+56, locationBlock[valuePos], mark[(valuePos - 1)]+56, locationBlock[(valuePos - 1)], lineColor);
    }

    blockPos++;
  }
  else
  {
    // clear the graph's canvas
    tft.fillRect((originX + 1), (originY - sizeY), sizeX, sizeY, ST77XX_WHITE);

    // map the value - current point
    locationBlock[valuePos] = map(temp, 0, graphRange, originY, (originY - sizeY));

    // draw point - current point
    tft.fillRect((mark[4])+56, (locationBlock[valuePos] - 1), markSize, markSize, pointColor);

    // draw all points
    for(int i = 0; i < 5; i++)
    {
      tft.fillRect((mark[(blockPos - (i + 1))] - 1)+56, (locationBlock[(valuePos - i)] - 1), markSize, markSize, pointColor);
    }

    // draw all the lines
    for(int i = 0; i < 4; i++)
    {
      tft.drawLine(mark[blockPos - (i + 1)]+56, locationBlock[valuePos - i], mark[blockPos - (i + 2)]+56, locationBlock[valuePos - (i + 1)], lineColor);
    }
    
    // change time lables
    //for(int i = 0; i <= 4; i++)
    //{
    //  tft.setCursor((mark[(4 - i)] - 5)+54, (originY + 16));
    //  tft.setTextColor(graphColor, ST77XX_WHITE);
    //  tft.setTextSize(1);
    //  tft.println(timeBlock[valuePos - i]);
    //}
  }

  valuePos++;}
}


void lab7(void *param){
  while(1){
    graph();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //Serial.printf("\nLab7 - Free Stack Space: %d", uxTaskGetStackHighWaterMark(NULL));
  }
}

void lab6(void *param){
  drawScale(-30);
  while (1){
    int newPercent;
  RawValue = analogRead(analogIn);
  newPercent = int((RawValue/4095.0)* 100.0);
    //
  if (newPercent != LastPercent){
    drawBar(-30, newPercent); 
    vTaskDelay(100 / portTICK_PERIOD_MS);  
}
    //Serial.printf("\nlab6 - Free Stack Space: %d", uxTaskGetStackHighWaterMark(NULL));
  }
}


void setup(void) {

  tft.initR(INITR_BLACKTAB);  

  sensors.begin();

  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo1.attach(servo1Pin, minUs, maxUs);
  servo2.attach(servo2Pin, minUs, maxUs);

  delay(500);
 
  tft.setRotation(1);

  drawHome();
  drawGraph();
  
  xTaskCreatePinnedToCore(
    lab7,
    "Lab 7",
    10000,
    NULL,
    1,
    NULL,
    1
  );
  
  xTaskCreatePinnedToCore(
    lab6,
    "Lab 6",
    10000,
    NULL,
    1,
    NULL,
    1
  );

}

void loop() {  
    //int newPercent;
    //RawValue = analogRead(analogIn);
    //newPercent = int((RawValue/4095.0)* 100.0);
    //
    //if (newPercent != LastPercent){
    //  drawBar(-30, newPercent); 
    //  delay(50);    
    //}
    //graph();
    //delay(2000); 
}

void drawScale(int adjustX){  
  tft.fillRect(0, 0, 70, 128, BACKCOLOR);
  tft.fillRoundRect(1,2,68,13,3, ST77XX_YELLOW);
  tft.drawRoundRect(1, 2, 68, 13, 3, ST77XX_MAGENTA);
  TFTPrint(3,5,1,ST77XX_BLACK,"Water Level");
  tft.drawFastVLine((55 + adjustX), 26,24, ST77XX_RED); // Vertical Scale Line  
  tft.drawFastVLine((55 + adjustX), 50,26, ST77XX_ORANGE);
  tft.drawFastVLine((55 + adjustX), 76,50, ST77XX_GREEN);

  tft.drawFastHLine((47 + adjustX), 26, 8, ST77XX_RED); // Major Division
  TFTPrint(0,18,1,ST77XX_RED,"100");
  TFTPrint(19,18,1,ST77XX_RED,"%");
  tft.drawFastHLine((50 + adjustX), 50, 5, ST77XX_ORANGE); // Minor Division
  TFTPrint(0,42,1,ST77XX_ORANGE,"75");
  TFTPrint(14,42,1,ST77XX_ORANGE,"%");
  tft.drawFastHLine((47 + adjustX), 75, 8, ST77XX_ORANGE); // Major Division
  TFTPrint(0,67,1,ST77XX_ORANGE,"50");
  TFTPrint(14,67,1,ST77XX_ORANGE,"%");
  tft.drawFastHLine((50 + adjustX), 100, 5, ST77XX_GREEN); // Minor Division
  TFTPrint(0,92,1,ST77XX_GREEN,"25");
  TFTPrint(14,92,1,ST77XX_GREEN,"%");
  tft.drawFastHLine((47 + adjustX), 125, 8, ST77XX_GREEN);  // Major Division
  TFTPrint(0,117,1,ST77XX_GREEN,"0");
  TFTPrint(8,117,1,ST77XX_GREEN,"%");
}


void drawBar (int adjustX,int nPer){

  if(nPer < LastPercent){
    tft.fillRect((61 + adjustX), 26 + (100-LastPercent), 20, LastPercent - nPer,  BACKCOLOR);     
  }
  else{
    tft.fillRect((61 + adjustX), 26 + (100-nPer), 20, nPer - LastPercent,  ST77XX_CYAN);
  }    
  LastPercent = nPer;  
  
}

void TFTPrint(uint16_t x, uint16_t y, uint8_t size,uint16_t color , String print){
  tft.setCursor(x, y);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.print(print);
}
  