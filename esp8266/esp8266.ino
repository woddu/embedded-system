const int led[] = {D0, D1, D2, D3, D4, D5, D6, D7};
const int button = D8;

int mode;

ICACHE_RAM_ATTR void changeMode() {
  mode++;
  if(mode > 3){
    mode = 0;
  }
}

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < 8; i++){
    pinMode(led[i], OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(button), changeMode, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch(mode){
    case 0:
      for(int i = 1; i < 9;i++){
        lightLed(i);
        delay(125);
      }
    
      for(int i = 9; i > 0;i--){
        lightLed(i);
        delay(125);
      }
      break;
    case 1:
      lightLed(1);
      delay(125);
      lightLed(2);
      delay(125);
      lightLed(3);
      delay(125);
      lightLed(4);
      delay(125);
      lightLed(5);
      digitalWrite(led[3], HIGH);
      delay(125);
      lightLed(6);
      digitalWrite(led[2], HIGH);
      delay(125);
      lightLed(7);
      digitalWrite(led[1], HIGH);
      delay(125);
      lightLed(8);
      digitalWrite(led[0], HIGH);
      delay(125);

      lightLed(8);
      digitalWrite(led[0], HIGH);
      delay(125);
      lightLed(7);
      digitalWrite(led[1], HIGH);
      delay(125);
      lightLed(6);
      digitalWrite(led[2], HIGH);
      delay(125);
      lightLed(5);
      digitalWrite(led[3], HIGH);
      delay(125);  
      break;
    case 2:
      lightLed(2);
      delay(250);  
      lightLed(4);
      delay(250);
      lightLed(6);  
      delay(250);  
      lightLed(8); 
      delay(250);  
      lightLed(2);
      delay(250);  
      lightLed(4);
      delay(250);
      lightLed(6);  
      delay(250);  
      lightLed(8); 
      delay(250);  
      lightLed(1);
      delay(250);  
      lightLed(3);
      delay(250);
      lightLed(5);  
      delay(250);  
      lightLed(7); 
      delay(250);  
      lightLed(1);
      delay(250);  
      lightLed(3);
      delay(250);
      lightLed(5);  
      delay(250);  
      lightLed(7); 
      delay(250);
      break;
    case 3:
      digitalWrite(led[0], HIGH);
      digitalWrite(led[2], HIGH);
      digitalWrite(led[4], HIGH);
      digitalWrite(led[6], HIGH);
      delay(250);
      for (int i = 0; i < 8;i++){
          digitalWrite(led[i], LOW);
      }
      digitalWrite(led[1], HIGH);
      digitalWrite(led[3], HIGH);
      digitalWrite(led[5], HIGH);
      digitalWrite(led[7], HIGH);
      delay(250);
      for (int i = 0; i < 8;i++){
          digitalWrite(led[i], LOW);
      }
      digitalWrite(led[0], HIGH);
      digitalWrite(led[2], HIGH);
      digitalWrite(led[4], HIGH);
      digitalWrite(led[6], HIGH);
      delay(250);
      for (int i = 0; i < 8;i++){
          digitalWrite(led[i], LOW);
      }
      digitalWrite(led[1], HIGH);
      digitalWrite(led[3], HIGH);
      digitalWrite(led[5], HIGH);
      digitalWrite(led[7], HIGH);
      delay(250);
      for (int i = 0; i < 8;i++){
          digitalWrite(led[i], LOW);
      }
      digitalWrite(led[0], HIGH);
      digitalWrite(led[2], HIGH);
      digitalWrite(led[4], HIGH);
      digitalWrite(led[6], HIGH);
      delay(250);
      for (int i = 0; i < 8;i++){
          digitalWrite(led[i], LOW);
      }
      digitalWrite(led[1], HIGH);
      digitalWrite(led[3], HIGH);
      digitalWrite(led[5], HIGH);
      digitalWrite(led[7], HIGH);
      delay(250);
      for (int i = 0; i < 8;i++){
          digitalWrite(led[i], LOW);
      }
      break;
    default:
      break;
  }
  

  

  
  

  



}

void lightLed(int num){
  for (int i = 0; i < 8;i++){
    if(num == (i+1)){
      digitalWrite(led[i], HIGH);
    } else {
      digitalWrite(led[i], LOW);
    }
  }
}
