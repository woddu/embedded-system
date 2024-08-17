// #include <FreeRTOS.h>
// #include <task.h>
// #include <timers.h>
// //YX9TTFMMFXWL
// int button[] = {2, 3, 4, 5};
// int relay[] = {6, 7, 8};
// int spinMotor = 6;
// int clockWise = 7;
// int counterClockWise = 8;

// int prev[4], val[4];

// bool drain = false, pSpin = false, pWash  = false, spin = false, wash  = false;

// static TickType_t timerDuration;
// static TimerHandle_t timer = NULL;
// static TaskHandle_t washingMachine;
// void OFF(){
//   for (int i = 0; i < 3; i++)
//     digitalWrite(relay[i], LOW);
// }

// void _pause(){
//   OFF();
//   vTaskSuspend(washingMachine);
// }

// void finished(TimerHandle_t timer){
//   wash = false;
//   pWash = false;
//   OFF();
//   vTaskSuspend(washingMachine);
// }

// void turn(int direction, int duration){
//   digitalWrite(direction, HIGH);
//   vTaskDelay(duration / portTICK_PERIOD_MS);
//   digitalWrite(direction, LOW);
//   vTaskDelay(1500 / portTICK_PERIOD_MS);
// }

// void turn(int direction, int duration, int delay){
//   digitalWrite(direction, HIGH);
//   vTaskDelay(duration / portTICK_PERIOD_MS);
//   digitalWrite(direction, LOW);
//   vTaskDelay(delay / portTICK_PERIOD_MS);
// }

// void Relays(void *param){
//   (void) param;
//   while (1){
//     if (!spin && wash){
//       while (wash){
//         turn(clockWise, 4750);
//         turn(counterClockWise, 4750);
//       }
//     } else if (spin && !wash){
//       while (spin){
//         digitalWrite(spinMotor, HIGH);
//         vTaskDelay(5000 / portTICK_PERIOD_MS);
//         turn(clockWise, 2500, 1250);
//         turn(clockWise, 3125, 1250);
//         turn(clockWise, ((60000 * 8) + 20000), 10);
//         spin = false;
//         pSpin = false;
//         OFF();
//       }
//     } 
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//   }
// }

// void startTimer(int duration){
//   timerDuration = duration / portTICK_PERIOD_MS; 
//   timer = xTimerCreate("Timer", timerDuration, pdFALSE,(void*)0, finished);
//   xTimerStart(timer, portMAX_DELAY);
// }

// void setup() {
//   // put your setup code here, to run once:
//   for(int i = 0; i < 4; i++)
//     pinMode(button[i], INPUT_PULLUP);
//   for(int i = 0; i < 3; i++)
//     pinMode(relay[i], OUTPUT);
//   xTaskCreate(Relays, "Relays", 128, nullptr, 2, &washingMachine);
//   vTaskSuspend(washingMachine);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   for(int i = 0; i < 4; i++)
//     prev[i] = val[i];
//   for(int i = 0; i < 4; i++)
//     val[i] = digitalRead(button[i]);
//   for(int i = 0; i < 4; i++){
//     if (val[i] == LOW && prev[i] == HIGH){
//       if (i == 0){
//         spin = (wash ? false : true);
//         if (spin)
//           pSpin =  false;
//           vTaskResume(washingMachine);
//       } else if (i == 1){
//         wash = (spin ? false : true);
//         if (wash)
//           pWash =  false;
//           startTimer(((60000) * 20));
//           vTaskResume(washingMachine);
//       } else if (i == 2){        
//         if (wash && !pWash){
//           pWash = true;
//           _pause();
//           xTimerStop(timer, portMAX_DELAY);
//         } else if (spin && !pSpin){
//           pSpin = true;
//           _pause();
//         } else if (wash && pWash){
//           pWash = false;
//           vTaskResume(washingMachine);
//           xTimerStart(timer, portMAX_DELAY);
//         } else if (spin && pSpin){
//           pSpin = false;
//           vTaskResume(washingMachine);
//         }
//       } else if (i == 3){
//         drain = (wash || spin || pSpin || pWash || drain ? false : true);
//         digitalWrite(spinMotor, (drain ? HIGH : LOW));
//       }
//     }
//   }
//   vTaskDelay(15 / portTICK_PERIOD_MS);
// }

static TickType_t timerDuration;
static TimerHandle_t timer = NULL;
static TaskHandle_t h_CounterClockWise;
static TaskHandle_t h_ClockWise;
static TaskHandle_t h_Dryer;

const int  dryer = 32, clockWise = 33, counterClockWise = 25;// Gray Wire = dyer, Green Wire = clockWise , Black Wire = counterClockWise

const int button[] = {26, 27, 14, 12};
bool prev[4], curr[4], dry, wash, drain, _pause;

bool idle, isCounterClockWise; int spinCount;

unsigned long t2, t1;

bool NoProgram(){
  return !wash && !dry && !drain;
}

bool Draining(){
  return !wash && !dry && drain;
}

bool Drying(){
  return !wash && dry && !drain;
}

bool Washing(){
  return wash && !dry && !drain;
}

void StartDryer(int duration){
  timerDuration = duration / portTICK_PERIOD_MS; 
  timer = xTimerCreate("Timer", timerDuration, pdFALSE,(void*)0, tDryer);
  t1 = millis();
  xTimerStart(timer, portMAX_DELAY);
  spinCount++;
}

void AlternateDirection(int duration, bool idling){
  idle = idling;
  timerDuration = duration / portTICK_PERIOD_MS; 
  timer = xTimerCreate("Timer", timerDuration, pdFALSE,(void*)0, (isCounterClockWise ? tClockWise : tCounterClockWise));
  xTimerStart(timer, portMAX_DELAY);
}

void OFF(){
  digitalWrite(clockWise, LOW);
  digitalWrite(counterClockWise, LOW);
  digitalWrite(dryer, LOW);
}

void tClockWise(TimerHandle_t timer){
  if (idle){
    spinCount++;
    isCounterClockWise = false;  
    digitalWrite(clockWise, HIGH);
    // Serial.print(spinCount);
    // Serial.println("Turning CounterClockWise");    
    ((spinCount <= 6) ? AlternateDirection(1500, false) : AlternateDirection(3750, false));
  } else {
    OFF();
    if (spinCount >= 172) {
      // Serial.println("Finished");
      spinCount = 0;
      wash = idle = false;
      wash = dry = drain = false;
      xTimerStop(timer, portMAX_DELAY);
    } else{      
      ((spinCount <= 6) ? AlternateDirection(800, true) : AlternateDirection(1125, true));    
      // Serial.println("Idling...");
    }
  }
}

void tCounterClockWise(TimerHandle_t timer){
  if (idle){
    spinCount++;
    isCounterClockWise = true;  
    digitalWrite(counterClockWise, HIGH);
    // Serial.print(spinCount);
    // Serial.println("Turning ClockWise");
    (((spinCount <= 4) || (spinCount >= 168)) ? AlternateDirection(1500, false) : AlternateDirection(3750, false));    
  } else {
    OFF();
    if (spinCount >= 172) {
      // Serial.println("Finished");
      spinCount = 0;
      wash = idle = false;
      wash = dry = drain = false;
      xTimerStop(timer, portMAX_DELAY);
    } else {      
      (((spinCount <= 4) || (spinCount >= 168)) ? AlternateDirection(800, true) : AlternateDirection(1125, true));        
      // Serial.println("Idling...");
    }

  }
}

void tDryer(TimerHandle_t timer){
  
    switch (spinCount)
    {
    case 0:
      digitalWrite(clockWise, HIGH);
      StartDryer(1500);
      break;
    case 1:
      digitalWrite(clockWise, LOW);
      vTaskDelay(1250 / portTICK_PERIOD_MS);
      digitalWrite(clockWise, HIGH);
      StartDryer(2125);
      break;
    case 2:
      digitalWrite(clockWise, LOW);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      digitalWrite(clockWise, HIGH);
      StartDryer(3000);
      break;
    case 3:
      digitalWrite(clockWise, LOW);
      vTaskDelay(800 / portTICK_PERIOD_MS);
      digitalWrite(clockWise, HIGH);
      StartDryer(480000);
      break;    
    case 4:
      digitalWrite(clockWise, LOW);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      digitalWrite(dryer, LOW);
      spinCount = 0;
      wash = dry = drain = false;
      xTimerStop(timer, portMAX_DELAY);
      break;
    default:
      break;
    
    }
  
}

void Dryer(void *param){
  while (1){
    while (dry){

      dry = false;
    }
    vTaskDelay(15/portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(115200);
  pinMode(clockWise, OUTPUT);
  pinMode(counterClockWise, OUTPUT);
  pinMode(dryer, OUTPUT);
  for(int i = 0; i < 4; i++)
    pinMode(button[i], INPUT_PULLDOWN);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 4; i++)
    prev[i] = curr[i];
  for(int i = 0; i < 4; i++)
    curr[i] = digitalRead(button[i]);
  for(int i = 0; i < 4; i++){
    if (curr[i] == HIGH && prev[i] == LOW){
      switch (i){
        case 0:
          if (NoProgram()){
            spinCount = 0;
            dry = true;
            digitalWrite(dryer, HIGH);
            StartDryer(5000);
          }
          break;
        case 1:
          if (NoProgram()){
            spinCount = 0;          
            wash = true;
            AlternateDirection(1, true);
          }
          break;
        case 2:          
          if (!_pause){
            if (Drying()){ 
              digitalWrite(clockWise, LOW);
              if (spinCount == 4){
                t2 = millis();
                t2 = t2 - t1;
              }
              xTimerStop(timer, portMAX_DELAY);              
            } else if (Washing()){
              OFF();       
              xTimerStop(timer, portMAX_DELAY);
            }
            _pause = true;
          } else {
            if (Drying()){
              if (spinCount < 4){
                spinCount = 2;
                digitalWrite(clockWise, HIGH);
                StartDryer(2125);
              } else if (spinCount == 4){
                digitalWrite(clockWise, HIGH);
                vTaskDelay(1500 / portTICK_PERIOD_MS);
                digitalWrite(clockWise, LOW);
                vTaskDelay(1250 / portTICK_PERIOD_MS);
                digitalWrite(clockWise, HIGH);
                StartDryer(480000 - t2);
              }
            } else if (Washing()){
              AlternateDirection(5,true);
            }
            _pause = false;
          }
          break;
        case 3:
          if (NoProgram()){
            drain = true;
            digitalWrite(dryer, HIGH);
          } else if (Draining()){
            digitalWrite(dryer, LOW);
            wash = dry = drain = false;
          }
          break;
        default:
        
          break;
      }
    }
  }
 
}