#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

static const TickType_t timeDelay = 10 / portTICK_PERIOD_MS;
static TimerHandle_t timeCountDown = NULL;


struct hc595{ uint8_t data;  uint8_t latch; uint8_t clock;};
hc595 srScore = {29, 28, 27};
hc595 srTime = {2, 3, 4};

int button[] = {20, 19, 18, 17, 16, 21};
int val[6], prev [6];

int digit[] = {
  0b01111110,0b00110000,0b01101101,0b1111001,0b00110011,0b01011011,0b01011111,0b01110000,0b01111111,0b01111011
};
int digitdecimal[] = {
  0b11111110,0b10110000,0b11101101,0b1111001,0b10110011,0b11011011,0b11011111,0b11110000,0b11111111,0b11111011
};

int segment[] = { 
  0b00000001,0b00000010,0b00000100,0b00001000
};

int _time[] = {12, 0, 0};
const int minute = 0;
const int seconds = 1;
const int mili = 2;
int timeTemp;
int timeDigit[4];
bool stopTime = true;
bool done = false;
bool longPress;
bool blinkA;
bool blink = false;
bool add = true;
int tm1Score = 0;
int tm1STemp;
int tm1SDigit[3];
int tm2Score = 0;
int tm2STemp;
int tm2SDigit[3];

unsigned long prevT;

//thousands digit display
void displayTime(int thousands, int hundreds, int tens, int ones){
  int _4digits[] = { ones, tens, hundreds, thousands};
  for (int i = 0; i < 4;i++){
    for (int j = 0; j < 7;j++){
        if ((bitRead(digit[_4digits[i]], j) == 1)){    
          digitalWrite(srTime.data, LOW);
        } else {
          digitalWrite(srTime.data, HIGH);
        }
      digitalWrite(srTime.clock, HIGH);
      digitalWrite(srTime.clock, LOW);
    }
      for (int j = 0; j < 9;j++){
        if (bitRead(segment[i], j) == 1){
          digitalWrite(srTime.data, HIGH);
        } else {
          digitalWrite(srTime.data, LOW);
        }
        digitalWrite(srTime.clock, HIGH);
        digitalWrite(srTime.clock, LOW);
      }
      digitalWrite(srTime.latch, HIGH);
      digitalWrite(srTime.latch, LOW);
      vTaskDelay(3 / portTICK_PERIOD_MS);
  }
}
void displayScore(int thousands, int hundreds, int tens, int ones){
  int _4digits[] = { ones, tens, hundreds, thousands};
  for (int i = 0; i < 4;i++){
    for (int j = 0; j < 7;j++){
        if ((bitRead(digit[_4digits[i]], j) == 1)){    
          digitalWrite(srScore.data, HIGH);
        } else {
          digitalWrite(srScore.data, LOW);
        }
      digitalWrite(srScore.clock, HIGH);
      digitalWrite(srScore.clock, LOW);
    }
      for (int j = 0; j < 9;j++){
        if (bitRead(segment[i], j) == 1){
          digitalWrite(srScore.data, LOW);
        } else {
          digitalWrite(srScore.data, HIGH);
        }
        digitalWrite(srScore.clock, HIGH);
        digitalWrite(srScore.clock, LOW);
      }
      digitalWrite(srScore.latch, HIGH);
      digitalWrite(srScore.latch, LOW);
      vTaskDelay(3 / portTICK_PERIOD_MS);
  }
}

void turncateScores(){
  tm1STemp = tm1Score;
  tm2STemp = tm2Score;
  for (int i = 1; i >= 0; i--){
    tm1SDigit[i] = tm1STemp % 10;
    tm1STemp = tm1STemp / 10;
  }
  for (int i = 1; i >= 0; i--){
    tm2SDigit[i] = tm2STemp % 10;
    tm2STemp = tm2STemp / 10;
  }
  displayScore(tm1SDigit[0],tm1SDigit[1],tm2SDigit[0],tm2SDigit[1]);
}

void turncateTime(){
  if (_time[minute] != 0){
      timeTemp = _time[seconds];
    for (int i = 1; i >= 0; i--){
      timeDigit[i+2] = timeTemp % 10;
      timeTemp = timeTemp / 10;
    }
      timeTemp = _time[minute];
    for (int i = 1; i >= 0; i--){
      timeDigit[i] = timeTemp % 10;
      timeTemp = timeTemp / 10;
    }
  } else {
      timeTemp = _time[mili];
    for (int i = 1; i >= 0; i--){
      timeDigit[i+2] = timeTemp % 10;
      timeTemp = timeTemp / 10;
    }
      timeTemp = _time[seconds];
    for (int i = 1; i >= 0; i--){
      timeDigit[i] = timeTemp % 10;
      timeTemp = timeTemp / 10;
    }
  }  
  displayTime(timeDigit[0],timeDigit[1],timeDigit[2],timeDigit[3]);
}

void teamScore(int team, int val){
  if (team == 1)
    tm1Score = (add ? tm1Score + val:tm1Score - val);
  else if (team == 2)
    tm2Score = (add ? tm2Score + val:tm2Score - val);
  if (tm1Score < 0) tm1Score = 0;
  if (tm2Score < 0) tm2Score = 0;
}

void _Time(TimerHandle_t timer){
  _time[mili]--;
  if (_time[mili] < 0) {
    _time[mili] = 99;
    _time[seconds]--;
    if (_time[seconds] < 0){
      _time[seconds] = 59;
      _time[minute]--;
      if (_time[minute] < 0){
        _time[minute] = 0;
        _time[seconds] = 0;
        _time[mili] = 0;
        stopTime = true;
        done = true;
        xTimerStop(timeCountDown,portMAX_DELAY);
      }
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < sizeof(button)/sizeof(button[0]); i++){    
    pinMode(button[i], INPUT_PULLDOWN);
  }
  pinMode(srScore.clock, OUTPUT);
  pinMode(srScore.latch, OUTPUT);
  pinMode(srScore.data, OUTPUT);
  pinMode(srTime.clock, OUTPUT);
  pinMode(srTime.latch, OUTPUT);
  pinMode(srTime.data, OUTPUT);
  timeCountDown = xTimerCreate("Time",timeDelay,pdTRUE,(void*)0,_Time);
}

unsigned long t4,t3,t2, t1;

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 6; i++)
    prev[i] = val[i];
  for (int i = 0; i < 6; i++)
    val[i] = digitalRead(button[i]);
  for (int i = 0; i < 6; i++){
    if (i == 0){
      if (val[i] == HIGH && prev[i] == LOW){
        t1 = millis();
        add = (add ? false : true);
      } else if (val[i] == HIGH && prev[i] == HIGH){
        t2 = millis();
        if (t2-t1 >=1500){
          tm1Score = tm2Score = 0;
          add = true;
        }
      } 
    } else if (i == 1){
      if (val[i] == HIGH && prev[i] == LOW)
        teamScore(1,2);
    } else if (i == 2){
      if (val[i] == HIGH && prev[i] == LOW)
        teamScore(1,3);
    } else if (i == 3){
      if (val[i] == HIGH && prev[i] == LOW)
        teamScore(2,2);
    } else if (i == 4){
      if (val[i] == HIGH && prev[i] == LOW)
        teamScore(2,3);
    } else if (i == 5){
      if (val[i] == HIGH && prev[i] == LOW){
        t3 = millis();
        if (stopTime){
          if (done){
            _time[minute] = 12;
            done = false;
          }
          stopTime = false;
          xTimerStart(timeCountDown,portMAX_DELAY);
          Serial.println("Timer start");
        } else {
          stopTime = true;
          xTimerStop(timeCountDown,portMAX_DELAY);
          Serial.println("Timer stop");
        }
      } else if((val[i] == LOW && prev[i] == HIGH)) {
        t4 = millis();
        if (t4 - t3 >= 1000){
          _time[minute] = 1;
          _time[seconds] = 0;
          _time[mili] = 0;
        }      
      }
    }
  }
  delay(5);
}

void setup1(){
  Serial.begin(115200);
}

void loop1(){
  while (done){
    unsigned long curr = millis();
    if (curr - prevT >=250){
      prevT = curr;
      blinkA = (blinkA ? false : true);
    }
    if (blinkA){
      turncateTime();      
    } else { 
      for( int i = 0; i < 16; i++){
        digitalWrite(srTime.data, LOW);
        digitalWrite(srTime.clock, HIGH);
        digitalWrite(srTime.clock, LOW);
      }
      digitalWrite(srTime.latch, HIGH);
      digitalWrite(srTime.latch, LOW);
    }
    turncateScores();
  }
  turncateTime();
  turncateScores();
}
