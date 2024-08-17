#include "EEPROM.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define CHAR_SPACING  1

#define EEPROM_SIZE 1
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4 // 4 blocks
#define CS_PIN 5

MD_MAX72XX ledMatrix = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


int addr = 0;

struct hc595 {
  uint8_t data;
  uint8_t latch;
  uint8_t clock;
};

hc595 srTime = { 33, 25, 26 };
int digit[] = {
  B01111110, B00110000, B01101101, B1111001, B00110011, B01011011, B01011111, B01110000, B01111111, B01111011
};

int segment[] = {
  B00000001, B00000010, B00000100, B00001000, B00010000, B00100000
};

static const TickType_t timerSecDelay = 1000 / portTICK_PERIOD_MS;
static TimerHandle_t timerCountDown = NULL;
static TimerHandle_t readySetGo = NULL;
static TaskHandle_t ledMatrixLoop = NULL;

bool play, gameOver, scored, newHighScore;

int countdown = 3;

int timer = 50;
int timerTemp;
int timerDigit[2];
bool playTimer = false;

int score = 0;
int scoreTemp;
int scoreDigit[2];

int highScore = 0;
int highScoreTemp;
int highScoreDigit[2];

const int resetBtn = 12, playBtn = 13;
int ir[] = { 14, 27 };


void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t   state = 0;
  uint8_t   curLen;
  uint16_t  showLen;
  uint8_t   cBuf[8];
  int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

  ledMatrix.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do     // finite state machine to print the characters in the space available
  {
    switch(state)
    {
      case 0: // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0')
        {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = ledMatrix.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1: // display the next part of the character
        ledMatrix.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen)
        {
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3:	// display inter-character spacing or end of message padding (blank columns)
        ledMatrix.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  ledMatrix.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}


void displayTimer() {
  timerTemp = timer;
  for (int i = 1; i >= 0; i--) {
    timerDigit[i] = timerTemp % 10;
    timerTemp = timerTemp / 10;
  }
  scoreTemp = score;
  for (int i = 1; i >= 0; i--) {
    scoreDigit[i] = scoreTemp % 10;
    scoreTemp = scoreTemp / 10;
  }
  highScoreTemp = highScore;
  for (int i = 1; i >= 0; i--) {
    highScoreDigit[i] = highScoreTemp % 10;
    highScoreTemp = highScoreTemp / 10;
  }
  latchTimer(timerDigit[0], timerDigit[1], highScoreDigit[0], highScoreDigit[1], scoreDigit[0], scoreDigit[1]);
  
}

void latchTimer(int tens3, int ones3, int tens2, int ones2, int tens1, int ones1) {
  digitalWrite(srTime.data, LOW);
  int _4digits[] = { ones1, tens1, ones2, tens2, ones3, tens3 };
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 7; j++) {
      if (_4digits[i] == 0 && i == 1) {
        digitalWrite(srTime.data, LOW);
      } else {
        if ((bitRead(digit[_4digits[i]], j) == 1)) {
          digitalWrite(srTime.data, HIGH);
        } else {
          digitalWrite(srTime.data, LOW);
        }
      }
      digitalWrite(srTime.clock, HIGH);
      digitalWrite(srTime.clock, LOW);
    }
    for (int j = 0; j < 9; j++){
      
        if ((bitRead(segment[i], j) == 1)){
          digitalWrite(srTime.data, LOW);
        } else {
          digitalWrite(srTime.data, HIGH);
        }
        digitalWrite(srTime.clock, HIGH);
        digitalWrite(srTime.clock, LOW);
      
    }
    digitalWrite(srTime.latch, HIGH);
    digitalWrite(srTime.latch, LOW);
    vTaskDelay(3 / portTICK_PERIOD_MS);
  }
}

void ReadySetGo(TimerHandle_t timerr) {
  bool stop = false;
  if (!play){
    Serial.println(countdown);
    switch(countdown){
      case 1:
        printText(0, MAX_DEVICES-1,"     1");
        break;
      case 2:
        printText(0, MAX_DEVICES-1,"     2");
        break;
      case 3:
        printText(0, MAX_DEVICES-1,"     3");
        break;
      default:
        break;
    }
     // display text
    tone(4, 2000);
    for (int i = 15; i >= 0; i--){
      if(i == 7){
      noTone(4);
      }
      ledMatrix.control(MD_MAX72XX::INTENSITY, i);
      delay(62);
    }
    countdown--;
    if (countdown <= 0){
      countdown = 3;
      tone(4, 4000);
      ledMatrix.control(MD_MAX72XX::INTENSITY, 15);
      printText(0, MAX_DEVICES-1,"   Go!"); 
      vTaskDelay(750/ portTICK_PERIOD_MS);
      stop = true;
      noTone(4);
    }
  }
  if (stop){
      play = true;
      vTaskResume(ledMatrixLoop);
    xTimerStop(readySetGo, portMAX_DELAY);
    xTimerStart(timerCountDown, portMAX_DELAY);
  }
}

void Timer(TimerHandle_t timerr) {
  bool stop = false;
  if (play){
    timer--;
    if (timer < 0) {
      timer = 00;
      stop = true;
    }
  }
  if (stop) {
    if (score > highScore){
      newHighScore = true;
      highScore = score;
      EEPROM.write(addr, highScore);
      EEPROM.commit();
      score = 0;
    }
    play = false;
    gameOver = true;
    vTaskSuspend(ledMatrixLoop);
    tone(4, 4000);
    printText(0, MAX_DEVICES-1,"   Game"); // display text
    vTaskDelay(850/ portTICK_PERIOD_MS);
    printText(0, MAX_DEVICES-1,"   Over"); // display text
    vTaskDelay(850/ portTICK_PERIOD_MS);
    noTone(4);
    vTaskResume(ledMatrixLoop);
    xTimerStop(timerCountDown, portMAX_DELAY);

  }
}

void LedMatrix(void* param){
  while (1){
    if (play && !scored){
      // ledMatrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
      // for (uint8_t i = 0; i<ledMatrix.getColumnCount(); i++)
      // {
      //   uint8_t r = random(ROW_SIZE);
      //   uint8_t cd = 0;
    
      //   for (uint8_t j = 0; j<r; j++)
      //     cd |= 1 << j;
    
      //   ledMatrix.setColumn(i, ~cd);
      // }
      ledMatrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
      ledMatrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
      for (uint8_t i = 0; i<MAX_DEVICES; i++)
      {
        uint8_t r = random(ROW_SIZE);
        uint8_t cd = 0;

        for (uint8_t j=0; j<r; j++)
          cd |= 1<<j;
        for (uint8_t j=0; j<COL_SIZE-1; j++)
          ledMatrix.setColumn(i, j, ~cd);
      }
      ledMatrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
      vTaskDelay(125/ portTICK_PERIOD_MS);
    } else if(scored&& play){
      int rm = random(0,2);
      switch (rm){
        case 0:
          printText(0, MAX_DEVICES-1,"  Nice!");
          break;
        case 1:
          printText(0, MAX_DEVICES-1,"   Eyy!");
          break;
        case 2:
          printText(0, MAX_DEVICES-1,"  Cool!");
          break;
      }
      vTaskDelay(750/ portTICK_PERIOD_MS);
      scored = false;
    } else if (newHighScore && !play){
      printText(0, MAX_DEVICES-1,"    New"); // display text
      vTaskDelay(850/ portTICK_PERIOD_MS);
      printText(0, MAX_DEVICES-1,"HighScore"); // display text
      vTaskDelay(850/ portTICK_PERIOD_MS);
      newHighScore = false;
    }else {
      printText(0, MAX_DEVICES-1," Press"); // display text
      vTaskDelay(850/ portTICK_PERIOD_MS);
      printText(0, MAX_DEVICES-1," Start"); // display text
      if (!play) {
        vTaskDelay(850/ portTICK_PERIOD_MS);
        ledMatrix.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
        ledMatrix.clear();
      }
    }
  }
}

void DigitDisplay(void* param) {
  unsigned long prevM;
  bool blink;
  int blinks;
  while (1) {
    while (gameOver){
      unsigned long currM = millis();
      if (currM - prevM >= 250){
        prevM = currM;
        blink = !blink;
        blinks++;
      }
      if(blink){
        for( int i = 0; i < 16; i++){
          digitalWrite(srTime.data, LOW);
          digitalWrite(srTime.clock, HIGH);
          digitalWrite(srTime.clock, LOW);
        }
        digitalWrite(srTime.latch, HIGH);
        digitalWrite(srTime.latch, LOW);
      } else {
        displayTimer();
      }
      if (blinks >= 16){
        blinks = blink = 0;
        gameOver = false;
      }
    }
    displayTimer();
    
  }
}

void setup() {
  // put your setup code here, to run once:
  EEPROM.begin(EEPROM_SIZE);
  highScore = EEPROM.read(addr);
  Serial.begin(115200);
  pinMode(playBtn, INPUT_PULLDOWN);
  pinMode(resetBtn, INPUT_PULLDOWN);
  pinMode(srTime.data, OUTPUT);
  pinMode(srTime.latch, OUTPUT);
  pinMode(srTime.clock, OUTPUT);
  ledMatrix.begin();         // initialize the object

  timerCountDown = xTimerCreate("Sleep Timer", timerSecDelay, pdTRUE, (void*)0, Timer);
  readySetGo = xTimerCreate("Ready Set Go", timerSecDelay, pdTRUE, (void*)1, ReadySetGo);
  
  xTaskCreatePinnedToCore(
    DigitDisplay,
    "Digit Display",
    5000,
    NULL,
    2,
    NULL,
    0);
  xTaskCreatePinnedToCore(
    LedMatrix,
    "Matrix Display",
    5000,
    NULL,
    2,
    &ledMatrixLoop,
    0);
}
int curr[2], prev[2];

bool up, down;
bool noMovement(){
  return !up && !down;
}
unsigned long t4, t3, t2, t1, tOut;
int rstCurr, rstPrev;
void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 2; i++)
    prev[i] = curr[i];
  for (int i = 0; i < 2; i++)
    curr[i] = digitalRead(ir[i]);
  rstPrev = rstCurr;
  rstCurr = digitalRead(resetBtn);
  if (rstCurr == 1 && rstPrev == 0){
    xTimerStop(timerCountDown, portMAX_DELAY);
    timer = 50;
    score = 0;
    play = false;
    t4 = t3 = 0;
    t3 = millis();
  } else if(rstCurr == 1 && rstPrev == 1){
    t4 = millis();
    if (t4 - t3 >= 5500){
      highScore = 0;
      EEPROM.write(addr, 0);
      EEPROM.commit();
      t4 = t3 = 0;
    }
  }

  if (digitalRead(playBtn) == 1){
    if(!play){
      timer = 50;
      score = 0;
      xTimerStart(readySetGo, portMAX_DELAY);
      vTaskSuspend(ledMatrixLoop);
    }
  }
  if (play){
    if (curr[1] == 0 && prev[1] == 1) {
      if (noMovement()){
        t1 = millis();
        down = true;
        up = false;
      } else if (up && !down){
        t2 = millis();
        if (t2 - t1 < 900) {          
          score--;
          if (score <= 0) score = 0;
        }
        up = down = t2 = t1 = 0;
      }
    }
    if (curr[0] == 0 && prev[0] == 1) {
      if (noMovement()){
        up = true;
        down = false;
        t1 = millis();
      } else if (!up && down){
        t2 = millis();
        if (t2 - t1 < 900) {
          timer += 3;
          if (timer >= 50) timer = 50;
          score++;
          if (score >= 99) score = 99;
          if(!scored) scored = true;
        }
        up = down = t2 = t1 = 0;
      }
    }
  }
  tOut = millis();
  if (tOut - t1 >= 1200){
    t2 = t1 = tOut  = up = down = 0;
  }
  vTaskDelay(5 / portTICK_PERIOD_MS);
}
