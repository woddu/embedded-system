const int  pirSensorPin = 14;
const int relayCpin = 16;
const int relayMpin = 17;

bool switchState = false;

int stateCurrent;
int statePrevious;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pirSensorPin, INPUT);
  pinMode(relayMpin, INPUT);
  pinMode(relayCpin, OUTPUT);
  int stateCurrent = LOW;
  int statePrevious = LOW;
}

void loop() {
  // put your main code here, to run repeatedly:
  statePrevious = stateCurrent;
  stateCurrent = digitalRead(pirSensorPin);

  if (statePrevious == LOW && stateCurrent == HIGH){
    //motion detected
    if (!switchState){
      turnOnRelay();      
      switchState = true;
    }
  } else if (statePrevious == HIGH && stateCurrent == LOW) {
    //motion stopped       
    if (switchState){
      turnOffRelay();
      switchState = false;
    }
  }

}

void turnOnRelay(){
  if (digitalRead(relayMpin) == LOW){
    digitalWrite(relayCpin, HIGH);
    digitalWrite(relayCpin, LOW);
  }
}
void turnOffRelay(){
  if (digitalRead(relayMpin) == HIGH){
    digitalWrite(relayCpin, HIGH);
    digitalWrite(relayCpin, LOW);
  }
}
