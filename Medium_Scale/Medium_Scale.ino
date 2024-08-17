#include <FreeRTOS.h>
#include <task.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/Picopixel.h>
#include <SPI.h>

#define TFT_CS         6
#define TFT_DC         7
#define TFT_RST        8 // Or set to -1 and connect to Arduino RESET pin

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

struct selection {
  bool in;
  int selected;
  bool inSelected;
  //selection selectedSelection{};
};
selection Options = {true, 0, false};

int selectedOptions;
int middleX = 103, middleY = 74;
int boxX,boxY;
int dBoxX, dBoxY;
int ir[] = {29, 28, 0, 1};
int prev[4], curr[4];

uint16_t primary = tft.color565(67, 105, 21);
uint16_t surface = tft.color565(253, 252, 245);
uint16_t onSurface = tft.color565(27, 28, 24);
uint16_t surfaceVariant = tft.color565(225, 228, 213);
uint16_t onSurfaceVariant = tft.color565(68, 72, 61);
uint16_t primaryContainer = tft.color565(194, 241, 141);
uint16_t onPrimaryContainer = tft.color565(15, 32, 0);

void drawHorizontal(int x){
  tft.fillRect(dBoxX, dBoxY, 4, 4, onSurface);
  dBoxX += x;
  if (dBoxX <= 52){
    dBoxX = 52;
  } else if(dBoxX >=154){
    dBoxX = 154;
  }  
}
void drawVertical(int y){
  tft.fillRect(dBoxX, dBoxY, 4, 4, onSurface);
  dBoxY += y;
  if (dBoxY <= 26){
    dBoxY = 26;
  } else if(dBoxY >=122){
    dBoxY = 122;
  }  
}

void moveHorizontal(int x){
  boxX += x;
  if (boxX <= 52){
    boxX = 52;
  } else if(boxX >=152){
    boxX = 152;
  }
  tft.fillRect(50, 24, 110, 104, surface);
  tft.fillRect(boxX, boxY, 6, 6, onSurface);
}
void moveVertical(int y){
  boxY += y;
  if (boxY <= 26){
    boxY = 26;
  } else if(boxY >=120){
    boxY = 120;
  }
  tft.fillRect(50, 24, 110, 104, surface);
  tft.fillRect(boxX, boxY, 6, 6, onSurface);
}

void color(uint8_t r, uint8_t g, uint8_t b){
  tft.color565(r, g, b);
}

void TFTPrint(uint16_t x, uint16_t y, uint8_t size,uint16_t color , String print){
  tft.setCursor(x, y);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.print(print);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for (int i = 0; i < 4; i++)
    pinMode(ir[i], INPUT);
  
}
unsigned long t2,t1, tOut,tHold;
bool up[2], down[2], right[2], left[2];

bool withinTime(){
  return (t2 - t1 < 800);
}
void resetMovement(){
  t2 = t1 = 0;
  for (int i = 0; i < 2; i++){
    up[i] = false;
    down[i] = false;
    left[i] = false;
    right[i] = false;
  }
}
bool noMovement(){
  return (!up[0] && !down[0] && !right[0] && !left[0] && !up[0] && !down[1] && !right[1] && !left[1]);
}
bool downward(){
  return (down[0] && down[1]);
}
bool upward(){
  return (up[0] && up[1]);
}
bool leftward(){
  return (left[0] && left[1]);
}
bool rightward(){
  return (right[0] && right[1]);
}
void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 4; i++)
    prev[i] = curr[i];
  for (int i = 0; i < 4; i++)
    curr[i] = digitalRead(ir[i]);

  if (curr[0] == 0 && prev[0] == 1){// Top
    if(upward()){
      t2 = millis();
      if (withinTime()){// v-----HERE 
        if(Options.in){
          Options.selected = (Options.selected <= 0 ? 0 : Options.selected - 1);
          
        }else if(Options.inSelected)        {
          switch(Options.selected){
            case 0:
              moveVertical(-4);
              break;
            case 1:
              drawVertical(-4);

              break;
            case 2:

              break;
            default:
              break;  
          }
        }        
        resetMovement();
      } else {
        resetMovement();
      }
    }
    else if (noMovement()){
      t1 = millis();
      down[0] = true;
      down[1] = false;
    }

     if (left[0] && !left[1]){
       t2 = millis();
       if (withinTime()){
         left[1] = true;
         t1 = millis();
       } else {
         resetMovement();
       }
     }
     if (right[0] && !right[1]){
       t2 = millis();
       if (withinTime()){
         right[1] = true;
         t1 = millis();
       } else {
         resetMovement();
       }
     }
  }
  if (curr[2] == 0 && prev[2] == 1){// Bottom
    if(downward()){

      t2 = millis();
      if (withinTime()){// v-----HERE 
        if(Options.in){
          Options.selected = (Options.selected >= 2 ? 2 : Options.selected + 1);
        } else if(Options.inSelected)        {
          switch(Options.selected){
            case 0:
              moveVertical(4);
              break;
            case 1:
              drawVertical(4);
              break;
            case 2:

              break;
            default:
              break;  
          }
        }
        resetMovement();
      } else {
        resetMovement();
      }
    }
    else if (noMovement()){
      t1 = millis();
      up[0] = true;

      up[1] = false;
    }

     if (left[0] && !left[1]){
       t2 = millis();
       if (withinTime()){
         left[1] = true;
         t1 = millis();
       } else {
         resetMovement();
       }
     }
     if (right[0] && !right[1]){
       t2 = millis();
       if (withinTime()){
         right[1] = true;
         t1 = millis();
       } else {
         resetMovement();
       }
     }
  }
  if (curr[1] == 0 && prev[1] == 1){// Left
    if (leftward()){
      t2 = millis();

      if (withinTime()){// v-----HERE 
        if (Options.in){
           
        }else if(Options.inSelected){
          switch(Options.selected){
            case 0:
              moveHorizontal(-4);
              break;
            case 1:
              drawHorizontal(-4);
              break;
            case 2:

              break;
            default:
              break;  
          }
        }
        resetMovement();
      } else {
        resetMovement();
      }
    }
    else if (noMovement()){
      t1 = millis();
      right[0] = true;
      right[1] = false;

    }

     if (down[0] && !down[1]){
       t2 = millis();
       if (withinTime()){
         down[1] = true;
         t1 = millis();
       } else {
         resetMovement();
       }
     }
     if (up[0] && !up[1]){
       t2 = millis();
       if (withinTime()){
         up[1] = true;
         t1 = millis();
       } else {
         resetMovement();
       }
     }
  }
  if(curr[1] == 0 && prev[1] == 0){
    if(right[0] && !right[1]){
      tHold = millis();
      if (tHold - t1 >= 1000){
        Options.inSelected = false;
        Options.in = true; 
      }
    }
  }
  if (curr[3] == 0 && prev[3] == 1){// Right
    if (rightward()){
      t2 = millis();

      if (withinTime()){// v-----HERE 
        if (Options.in){
          Options.in = false;
          Options.inSelected = true;
        }else if(Options.inSelected)        {
          switch(Options.selected){
            case 0:
              moveHorizontal(4);
              break;
            case 1:
              drawHorizontal(4);
              break;
            case 2:

              break;
            default:
              break;  
          }
        }
        resetMovement();
      } else {
        resetMovement();
      }
    }
    else if (noMovement()){
      t1 = millis();
      left[0] = true;
      left[1] = false;

    }

     if (down[0] && !down[1]){
       t2 = millis();
       if (withinTime()){
         down[1] = true;
         t1 = millis();
       } else {
         resetMovement();
       }
     }
     if (up[0] && !up[1]){
       t2 = millis();
       if (withinTime()){
         up[1] = true;
         t1 = millis();
       } else {
         resetMovement();
       }
     }
  }
  if (!noMovement()){
    tOut = millis();
    if (tOut - t1 >= 1200) {
      resetMovement();
      Serial.println("movement timeout");
    }
  }
  vTaskDelay(10/ portTICK_PERIOD_MS);
}
  //int width = tft.getCursorX(); // let w = 50, (160 - w) / 2

void setup1(){
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.setTextWrap(true);
  tft.setFont(&FreeSans9pt7b);
  tft.fillScreen(surface);
  tft.fillRect(0,0,160,24,primary);
  TFTPrint(23,18,1,0xFFFF,"Medium Scale");
  tft.fillRect(0, 24, 50, 104, surfaceVariant);// 50,24
  tft.setFont();
  TFTPrint(4,32,1,onSurfaceVariant,"Box");
  TFTPrint(4,47,1,onSurfaceVariant,"Draw");
  TFTPrint(4,62,1,onSurfaceVariant,"Blank");
  
}

void loop1(){
  while (Options.in){
    switch(Options.selected){
      case 0:
        tft.drawRect(2, 32-4, 50-4, 7+8, onSurfaceVariant);
        tft.drawRect(2, 47-4, 50-4, 7+8, surfaceVariant);
        tft.drawRect(2, 62-4, 50-4, 7+8, surfaceVariant);
        break;
      case 1:
        tft.drawRect(2, 32-4, 50-4, 7+8, surfaceVariant);
        tft.drawRect(2, 47-4, 50-4, 7+8, onSurfaceVariant);
        tft.drawRect(2, 62-4, 50-4, 7+8, surfaceVariant);
        break;
      case 2:
        tft.drawRect(2, 32-4, 50-4, 7+8, surfaceVariant);
        tft.drawRect(2, 47-4, 50-4, 7+8, surfaceVariant);
        tft.drawRect(2, 62-4, 50-4, 7+8, onSurfaceVariant);
        break;
      default:
        break;
    }
    vTaskDelay(10/ portTICK_PERIOD_MS);
  }
  switch(Options.selected){
    case 0:
      tft.fillRect(2, 32-4, 50-4, 7+8, onSurfaceVariant);
      TFTPrint(4,32,1,surfaceVariant,"Box");
      tft.drawRect(2, 47-4, 50-4, 7+8, surfaceVariant);
      tft.drawRect(2, 62-4, 50-4, 7+8, surfaceVariant);
      boxX = middleX; boxY = middleY;
      tft.fillRect(50, 24, 110, 104, surface);
      tft.fillRect(boxX, boxY, 6, 6, onSurface);
      
      while(Options.inSelected){
        
        
        vTaskDelay(5/ portTICK_PERIOD_MS);
      }tft.fillRect(50, 24, 110, 104, surface);
      tft.fillRect(0, 24, 50, 104, surfaceVariant);// 50,24
      tft.setFont();
      TFTPrint(4,32,1,onSurfaceVariant,"Box");
      TFTPrint(4,47,1,onSurfaceVariant,"Draw");
      TFTPrint(4,62,1,onSurfaceVariant,"Blank");
      break;
    case 1:
      tft.drawRect(2, 32-4, 50-4, 7+8, surfaceVariant);
      tft.fillRect(2, 47-4, 50-4, 7+8, onSurfaceVariant);
      TFTPrint(4,47,1,surfaceVariant,"Draw");
      tft.drawRect(2, 62-4, 50-4, 7+8, surfaceVariant);
      tft.fillRect(50, 24, 110, 104, surface);
      dBoxX = middleX; dBoxY = middleY;
      tft.fillRect(dBoxX, dBoxY, 4, 4, onSurface);
      bool blink;
      unsigned prev;
      while(Options.inSelected){
        unsigned curr = millis();
        if (curr - prev >= 500){
          prev = curr;
          blink = !blink;
          if (blink){
            tft.fillRect(dBoxX, dBoxY, 4, 4, onSurface);
          }else {
            tft.fillRect(dBoxX, dBoxY, 4, 4, surfaceVariant);
          }
        }
        vTaskDelay(10/ portTICK_PERIOD_MS);
      }tft.fillRect(50, 24, 110, 104, surface);
      tft.fillRect(0, 24, 50, 104, surfaceVariant);// 50,24
      tft.setFont();
      TFTPrint(4,32,1,onSurfaceVariant,"Box");
      TFTPrint(4,47,1,onSurfaceVariant,"Draw");
      TFTPrint(4,62,1,onSurfaceVariant,"Blank");
      break;
    case 2:
      tft.drawRect(2, 32-4, 50-4, 7+8, surfaceVariant);
      tft.drawRect(2, 47-4, 50-4, 7+8, surfaceVariant);
      tft.fillRect(2, 62-4, 50-4, 7+8, onSurfaceVariant);
      TFTPrint(4,62,1,surfaceVariant,"Blank");
      while(Options.inSelected){

        vTaskDelay(10/ portTICK_PERIOD_MS);
      }tft.fillRect(50, 24, 110, 104, surface);
      tft.fillRect(0, 24, 50, 104, surfaceVariant);// 50,24
      tft.setFont();
      TFTPrint(4,32,1,onSurfaceVariant,"Box");
      TFTPrint(4,47,1,onSurfaceVariant,"Draw");
      TFTPrint(4,62,1,onSurfaceVariant,"Blank");
      break;
    default:
      break;
  }
  vTaskDelay(10/ portTICK_PERIOD_MS);
}

