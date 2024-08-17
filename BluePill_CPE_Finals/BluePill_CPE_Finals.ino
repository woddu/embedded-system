#include "DHT.h"

#include <Servo.h>

Servo TMPservo;  // create servo object to control a servo
Servo HMTservo;  // create servo object to control a servo
Servo DSservo;  // create servo object to control a servo

DHT dht(PA6, DHT11);

int DSPIN = PA8;

double temp;
int dspos;
uint16_t Temp;
int tpos;
uint16_t Humi;
int hpos;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  TMPservo.attach(PA10);
  HMTservo.attach(PA11);
  DSservo.attach(PA12); 
  dht.begin();
}
 
void loop()
{
  // put your main code here, to run repeatedly:
  temp = TempRead();
  temp  = temp * 0.0625; // conversion accuracy is 0.0625 / LSB
  Serial1.println(temp);
  if (temp >= 15 && temp <=85){
  dspos = map(temp, 15, 85, 130, 25);
  DSservo.write(dspos);
  }
  Humi  = dht.readHumidity();
  if (Humi >= 40 && Humi <=80){
  hpos = map(Humi, 40, 80, 140, 40);
  HMTservo.write(hpos);
  }
  // read temperature in degrees Celsius
  Temp = dht.readTemperature() ;
  if (temp >= 20 && temp <= 50){
  tpos = map(Temp, 20, 50, 140, 40);
  TMPservo.write(tpos);
  }
  delay(125);
}
 
boolean DS18B20_Init()
{
  pinMode(DSPIN, OUTPUT);
  digitalWrite(DSPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(DSPIN, LOW);
  delayMicroseconds(750);//480-960
  digitalWrite(DSPIN, HIGH);
  pinMode(DSPIN, INPUT);
  int t = 0;
  while (digitalRead(DSPIN))
  {
    t++;
    if (t > 60) return false;
    delayMicroseconds(1);
  }
  t = 480 - t;
  pinMode(DSPIN, OUTPUT);
  delayMicroseconds(t);
  digitalWrite(DSPIN, HIGH);
  return true;
}
 
void DS18B20_Write(byte data)
{
  pinMode(DSPIN, OUTPUT);
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(DSPIN, LOW);
    delayMicroseconds(10);
    if (data & 1) digitalWrite(DSPIN, HIGH);
    else digitalWrite(DSPIN, LOW);
    data >>= 1;
    delayMicroseconds(50);
    digitalWrite(DSPIN, HIGH);
  }
}
 
byte DS18B20_Read()
{
  pinMode(DSPIN, OUTPUT);
  digitalWrite(DSPIN, HIGH);
  delayMicroseconds(2);
  byte data = 0;
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(DSPIN, LOW);
    delayMicroseconds(1);
    digitalWrite(DSPIN, HIGH);
    pinMode(DSPIN, INPUT);
    delayMicroseconds(5);
    data >>= 1;
    if (digitalRead(DSPIN)) data |= 0x80;
    delayMicroseconds(55);
    pinMode(DSPIN, OUTPUT);
    digitalWrite(DSPIN, HIGH);
  }
  return data;
}
 
int TempRead()
{
  if (!DS18B20_Init()) return 0;
  DS18B20_Write (0xCC); // Send skip ROM command
  DS18B20_Write (0x44); // Send reading start conversion command
  if (!DS18B20_Init()) return 0;
  DS18B20_Write (0xCC); // Send skip ROM command
  DS18B20_Write (0xBE); // Read the register, a total of nine bytes, the first two bytes are the conversion value
  int temp = DS18B20_Read (); // Low byte
  temp |= DS18B20_Read () << 8; // High byte
  return temp;
}