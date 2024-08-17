#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO16_GLCD_HEIGHT  16
#define LOGO16_GLCD_WIDTH   16

#define SOUND_SPEED 0.034

bool setDistance = true;


long duration;
double distanceCm, bottomDistance, distanceB, lastReading;
double x, y;

bool Redraw1 = true;
double ox , oy ;

const uint8_t trigger = 5;
const uint8_t echo = 18;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  xTaskCreatePinnedToCore(
      taskDisplay,
      "Lab 6",
      10000,
      NULL,
      1,
      NULL,
      1
  );
  
}

void loop() {
  // put your main code here, to run repeatedly:
    
}

void UDSGetDistance(){
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
}

void DrawBarChartV(Adafruit_SSD1306 &d, double curval, double x , double y , double w, double h , double loval , double hival , double inc , double dig, bool &Redraw)
{
  double stepval, my, level, i, data;
  int pct = 100;
  if (Redraw) {
    Redraw = false;
    d.fillRect(0, 0,  127 , 16, SSD1306_WHITE);
    d.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    d.setTextSize(1);
    d.setCursor(2, 4);
    d.print("Water");
    d.setCursor(34, 4);
    d.print("Level");
    // step val basically scales the hival and low val to the height
    // deducting a small value to eliminate round off errors
    // this val may need to be adjusted
    stepval = ( inc) * (double (h) / (double (hival - loval))) - .001;
    for (i = 0; i <= h; i += stepval) {
      my =  y - h + i;
      Serial.println("");
      d.drawFastHLine(x + w + 1, my,  5, SSD1306_WHITE);
      // draw lables
      d.setTextSize(1);
      d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      d.setCursor(x + w + 12, my - 3 );
      //data = hival - ( i * (inc / stepval));
      d.print(pct);
      d.print(" %");
      pct-=25;
    }
  }
  // compute level of bar graph that is scaled to the  height and the hi and low vals
  // this is needed to accompdate for +/- range
  level = (h * (((curval - loval) / (hival - loval))));
  // draw the bar graph
  // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
  d.drawRect(x, y - h, w, h, SSD1306_WHITE);
  d.fillRect(x, y - h, w, h - level,  SSD1306_BLACK);
  d.drawRect(x, y - h, w, h, SSD1306_WHITE);
  d.fillRect(x, y - level, w,  level, SSD1306_WHITE);
  // up until now print sends data to a video buffer NOT the screen
  // this call sends the data to the screen
  d.display();

}

void OLEDPrintln(uint16_t x, uint16_t y, uint8_t size, String print){
  display.clearDisplay();
  display.setTextWrap(true);
  display.setCursor(x, y);
  display.setTextSize(size);
  display.println(print);
  display.display();
}

void taskDisplay(void *param){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.display();
  display.setRotation(0);
  display.setTextColor(SSD1306_WHITE);
  GetBottomDistance();
}

void GetBottomDistance(){
  const int numReadings = 10;
  double readings[numReadings];  // the readings from the analog input
  int readIndex = 0;          // the index of the current reading
  double total = 0;              // the running total
  double average = 0;            // the average
  double lastReading;
  OLEDPrintln(2,4,2,"Getting distance in:");
  for (int i = 3; i > 0; i--){
    display.setCursor(0, 64);
    display.fillRect(0, 64, 40, 16, SSD1306_BLACK);
    display.println(i);
    display.display();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  OLEDPrintln(2,4,2,"Calculating, Don't move the UDS.");
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  for (int i = 0; i < 10; i++) {
    UDSGetDistance();
    // read from the sensor:
    readings[readIndex] = distanceCm;
    // add the reading to the total:
    total = total + readings[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;

    // if we're at the end of the array...
    if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  // calculate the average:
  average = total / numReadings;
  bottomDistance = average;//
  if (average < 2.5){
    display.clearDisplay();
    display.setCursor(2,4);
    display.setTextSize(2);
    display.println("Too Close!");
    display.println("Retrying in: ");
    display.display();
    for (int i = 3; i > 0; i--){
      display.setCursor(0, 64);
      display.fillRect(0, 64, 16, 60, SSD1306_BLACK);
      display.println(i);
      display.display();
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    GetBottomDistance();
  }
  else {
    OLEDPrintln(2,4,2,"Done!");
    vTaskDelay(750 / portTICK_PERIOD_MS);
    display.clearDisplay();
    display.setCursor(2,4);
    display.setTextSize(2);
    display.print("Distance from bottom is : ");
    display.print(bottomDistance);
    display.display();
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    display.clearDisplay();
  }
  setDistance = false;
  display.setRotation(3);
  DrawBarChartV(display, distanceB, 2, 125, 20, 100, 0, bottomDistance, (bottomDistance / 4.0), 0, Redraw1);
  while (!setDistance){
    UDSGetDistance();
    distanceB = bottomDistance - (distanceCm);
    
    if(((distanceCm > lastReading)  && ((distanceCm - lastReading) >= 0.10)) || ((distanceCm < lastReading)  && ((distanceCm - lastReading) <= -0.10))){//
    if (!Redraw1){
      Redraw1 = true;
    DrawBarChartV(display, distanceB, 2, 125, 20, 100, 0, bottomDistance , (bottomDistance / 4.0), 0, Redraw1);
    }
    lastReading = distanceB;
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
  }
  GetBottomDistance();
}

