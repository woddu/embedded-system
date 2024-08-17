#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

double x, y;

bool Redraw1 = true;
double ox , oy ;

const int trigger = 5;
const int echo = 18;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

double duration;
double distanceCm;
double distanceb;

const int numReadings = 10;
double readings[numReadings];  // the readings from the analog input
int readIndex = 0;          // the index of the current reading
double total = 0;              // the running total
double average = 0;            // the average
double lastReading;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(115200);
  pinMode(trigger, OUTPUT); // Sets the trigPin as an Output
  pinMode(echo, INPUT); // Sets the echoPin as an Input

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  delay(1000);
  display.display();
  display.clearDisplay();
  display.setRotation(3);
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
    delay(100);
  }
  average = (total / numReadings);

}

void loop() {
  // Clears the trigPin
  UDSGetDistance();
  distanceb = average - (distanceCm);
  if (distanceCm < 2.0){
    display.clearDisplay();
    display.setCursor(0, 32);
    display.setTextSize(2);
    display.println("Error!");
    display.display();
    for (int i = 3; i > 0; i--){
    display.setCursor(0, 64);
    display.fillRect(0, 64, 40, 16, SSD1306_BLACK);
    display.println(i);
    display.display();
    delay(1000);
  }
    display.clearDisplay();
  }else {
    if(((distanceCm > lastReading)  && ((distanceCm - lastReading) >= 0.1)) || ((distanceCm < lastReading)  && ((distanceCm - lastReading) <= -0.1))){//
    if (!Redraw1){
      Redraw1 = true;
    }
    DrawBarChartV(display, distanceb, 2, 125, 20, 100, -1.3, average , (average / 4.0), 0, Redraw1);
    Serial.println(distanceCm);
    Serial.println(average);
    Serial.println(average*.75);
    Serial.println(distanceb >= average*.75);
    }
  }
  lastReading=distanceCm;
  delay(125);
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