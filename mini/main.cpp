#include <IRremote.hpp>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

const uint16_t PWR = 8;
const uint16_t KEY_1 = 17;
const uint16_t KEY_2 = 18;
const uint16_t KEY_3 = 19;
const uint16_t KEY_4 = 20;
const uint16_t KEY_5 = 21;
const uint16_t KEY_6 = 22;
const uint16_t KEY_7 = 23;
const uint16_t KEY_8 = 24;
const uint16_t KEY_9 = 25;
const uint16_t KEY_0 = 16;
const uint16_t CH_PLUS = 2;
const uint16_t CH_MINUS = 3;
const uint16_t MUTE = 9;
const uint16_t KEY_OK = 68;
const uint16_t UP = 64;
const uint16_t LEFT = 7;
const uint16_t DOWN = 65;
const uint16_t RIGHT = 6;
const uint16_t BACK = 40;
const uint16_t EXIT = 91;
const uint16_t STOP = 177;
const uint16_t PLAY = 176;
const uint16_t PAUSE = 186;
const uint16_t BACKWARD = 143;
const uint16_t FORWARD = 142;
const uint16_t SETTINGS = 67;
const uint16_t RED = 114;
const uint16_t GREEN = 113;
const uint16_t YELLOW = 99;
const uint16_t BLUE = 97;
unsigned long lastTime;
unsigned long debounceTime = 250;
uint16_t key;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// IR remote Decode
uint16_t decodedIR()
{
  uint16_t command = 0;
  if (IrReceiver.decode())
  {
    IrReceiver.resume(); // Enable receiving of the next value
    unsigned long currentTime = millis();
    if ((currentTime - lastTime) >= debounceTime)
    {
      command = IrReceiver.decodedIRData.command;
      lastTime = currentTime;
    }
  }
  return command;
}

bool powerOn = false, showIntro = false, inOptions = false, executing = false, box = true;
int optionChoices = 0, selectedOption = 0;
String type;
bool canSelect()
{
  return (powerOn && inOptions && (optionChoices > 0 && optionChoices < 2));
}
bool canType()
{
  return (!inOptions && selectedOption == 0);
}
void RemoteKeys(void *parameter)
{
  while (1)
  {
    key = decodedIR();
    if (key != 0)
    {
      switch (key)
      {
      case PWR:
        if (!powerOn) {
          powerOn = true;
          showIntro = true;
          inOptions = true;
          executing = false;
        } else{
          showIntro = false;
          inOptions = false;
          executing = false;
          powerOn = false;
          display.clearDisplay();
        }
        Serial.println(powerOn);
        break;
      case KEY_1:
        type += (canType() ? "1" : type);
        break;
      case KEY_2:
        type += (canType() ? "2" : type);
        break;
      case KEY_3:
        type += (canType() ? "3" : type);
        break;
      case KEY_4:
        type += (canType() ? "4" : type);
        break;
      case KEY_5:
        type += (canType() ? "5" : type);
        break;
      case KEY_6:
        type += (canType() ? "6" : type);
        break;
      case KEY_7:
        type += (canType() ? "7" : type);
        break;
      case KEY_8:
        type += (canType() ? "8" : type);
        break;
      case KEY_9:
        type += (canType() ? "9" : type);
        break;
      case KEY_0:
        type += (canType() ? "0" : type);
        break;
      case CH_MINUS:
        type.remove(type.length() - 1);
        display.clearDisplay();
        break;
      case KEY_OK:
        if (inOptions){
          inOptions = false;
          executing = true;
        } else if(executing) {
          executing = false;
          inOptions = true;
        }
        Serial.println(inOptions);
        break;
      case UP:
        if (inOptions) optionChoices = (optionChoices > 0 ? optionChoices - 1 : 2); box = true;
        break;
      case LEFT:
        break;
      case RIGHT:
        break;
      case DOWN:
        if (inOptions) optionChoices = (optionChoices < 2 ? optionChoices + 1 : 0); box = true;
        break;
      case EXIT:
        break;
      case BACK:
        break;
      default:
        break;
      }
    }
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

void oledPrint(String text, GFXfont font, int x, int y)
{
  display.setCursor(x, y);
  display.setFont(&font);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print(text);
  display.display();
}

void oledPrintN(String text, GFXfont font, int x, int y)
{
  display.setCursor(x, y);
  display.setFont(&font);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print(text);
}

void oledSelectionBoxedPrint(int select, bool state)
{
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(16, 16);
  display.print("Type");
  display.setCursor(16, 38);
  display.print("Blink");
  display.setCursor(16, 59);
  display.print("...");
  switch (select)
  {
  case 0:
    if (state)
    {
      display.drawRect(8, 0, 128 - 16, 22, WHITE);
    }
    else
    {
      display.drawRect(8, 0, 128 - 16, 22, BLACK);
    }
    display.drawRect(8, 22, 128 - 16, 21, BLACK);
    display.drawRect(8, 43, 128 - 16, 21, BLACK);
    break;
  case 1:
    display.drawRect(8, 0, 128 - 16, 22, BLACK);
    if (state)
    {
      display.drawRect(8, 22, 128 - 16, 21, WHITE);
    }
    else
    {
      display.drawRect(8, 22, 128 - 16, 21, BLACK);
    }
    display.drawRect(8, 43, 128 - 16, 21, BLACK);
    break;
  case 2:
    display.drawRect(8, 0, 128 - 16, 22, BLACK);
    display.drawRect(8, 22, 128 - 16, 21, BLACK);
    if (state)
    {
      display.drawRect(8, 43, 128 - 16, 21, WHITE);
    }
    else
    {
      display.drawRect(8, 43, 128 - 16, 21, BLACK);
    }
    break;
  default:
    break;
  }
  display.display();
}

void oledSlowPrint(String text, GFXfont font, int x, int y, int delay)
{
  display.setCursor(x, y);
  display.setFont(&font);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  vTaskDelay(delay / portTICK_PERIOD_MS);
  for (int i = 0; i < text.length(); i++)
  {
    char c = text.charAt(i);
    display.print(c);
    vTaskDelay(delay / portTICK_PERIOD_MS);
    display.display();
  }
}

void oledClearDisplay(int delay)
{
  vTaskDelay(delay / portTICK_PERIOD_MS);
  display.clearDisplay();
}

void OLED(void *param)
{
  unsigned long previousMillis = 0;
  long interval = 400;
  while (1){
    while (powerOn){
      if (showIntro)
      {
        oledClearDisplay(0);
        oledSlowPrint("Welcome!", FreeSans12pt7b, 14, 36, 60);
        oledClearDisplay(750);
        oledSlowPrint("SAES", FreeSans12pt7b, 32, 40, 90);
        oledClearDisplay(500);
        oledSlowPrint("Simple", FreeSans12pt7b, 28, 40, 70);
        oledClearDisplay(500);
        oledSlowPrint("Input", FreeSans12pt7b, 37, 40, 80);
        oledClearDisplay(500);
        oledSlowPrint("Output", FreeSans12pt7b, 27, 40, 75);
        oledClearDisplay(500);
        showIntro = false;
      }
      while (inOptions)
      {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
          // save the last time you blinked
          previousMillis = currentMillis;
          box = !box;
        }
        oledSelectionBoxedPrint(optionChoices, box);
        vTaskDelay(15/portTICK_PERIOD_MS);      
      }
      selectedOption = optionChoices;
      switch (selectedOption){
        case 0:
          while (executing)
          {
            unsigned long currentMillis = millis();
            display.clearDisplay();
            oledPrintN(type, FreeSans9pt7b, 0, 14);
            if (currentMillis - previousMillis >= interval)
            {
              // save the last time you blinked
              previousMillis = currentMillis;
              box = !box;
              if (box)
              {
                display.fillRect(display.getCursorX(), display.getCursorY() - 2, 8, 4, WHITE);
              }
              else
              {
                display.fillRect(display.getCursorX(), display.getCursorY() - 2, 8, 4, BLACK);
              }
            }
            display.display();if (!powerOn) break;
            vTaskDelay(15/portTICK_PERIOD_MS);

          }
          break;
        case 1:
          while (executing)
          {
            display.clearDisplay();
            display.setFont(&FreeSans9pt7b);
            display.setTextColor(WHITE);
            display.setTextSize(1);
            display.setCursor(16, 16);
            display.print("Type");
            display.setCursor(16, 59);
            display.print("...");          
            display.fillRect(8, 22, 128 - 16, 21, WHITE);
            display.setTextColor(BLACK);
            display.setCursor(16, 38);
            display.print("Blink");
            display.display();
            digitalWrite(2, 1);if (!powerOn) break;
            vTaskDelay(500/portTICK_PERIOD_MS);
            digitalWrite(2,0);            
            vTaskDelay(500/portTICK_PERIOD_MS);
          }
          break;
        case 2:
          while (executing)
          {

            vTaskDelay(10/portTICK_PERIOD_MS);
          }
          break;
        default:
          break;
      }
      if(!powerOn) display.clearDisplay();
    }
    display.clearDisplay();
    vTaskDelay(10 / portTICK_PERIOD_MS);
    display.display();
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  IrReceiver.begin(23, false);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // if you distribute your code, the adafruit license requires you show their splash screen
  // otherwise clear the video buffer memory then display
  display.display();
  delay(100);
  display.clearDisplay();
  display.display();
  display.setTextWrap(true);
  xTaskCreatePinnedToCore(
      RemoteKeys,
      "Remote Keys",
      5000,
      NULL,
      2,
      NULL,
      0);
  xTaskCreatePinnedToCore(
      OLED,
      "Oled Display",
      5000,
      NULL,
      2,
      NULL,
      1);
}

void loop()
{
  // put your main code here, to run repeatedly:
 
}
 