#include <IRremote.hpp>

struct Relay{
  uint8_t controlPin, monitorPin;
  bool isOn;
}

bool IsOn(Relay relay){
  if (digitalRead(relay.monitorPin) == HIGH){
    relay.isOn = true;
  } else {
    relay.isOn = false;
  }
}

void TurnOn(Relay relay){
  if (!relay.isOn){
    digitalWrite(relay.controlPin, HIGH);
    digitalWrite(relay.controlPin, LOW);
    IsOn(relay);
  }
}

void TurnOff(Relay relay){
  if (relay.isOn){
    digitalWrite(relay.controlPin, HIGH);
    digitalWrite(relay.controlPin, LOW);
    IsOn(relay);
  }
}

// IR remote variables
const uint16_t PWR =  8;	const uint16_t KEY_1 =  17;	const uint16_t KEY_2 =  18;	const uint16_t KEY_3 =  19;	const uint16_t KEY_4 =  20;	
const uint16_t KEY_5 =  21;	const uint16_t KEY_6 =  22;	const uint16_t KEY_7 =  23;	const uint16_t KEY_8 =  24;	const uint16_t KEY_9 =  25;	
const uint16_t KEY_0 =  16;	const uint16_t CH_PLUS =  2;const uint16_t CH_MINUS =  3;const uint16_t MUTE =  9;	const uint16_t KEY_OK =  68;	
const uint16_t UP =  64;	const uint16_t LEFT =  7;	const uint16_t DOWN =  65;	const uint16_t RIGHT =  6;	const uint16_t BACK =  40;	
const uint16_t EXIT =  91;	const uint16_t STOP =  177;	const uint16_t PLAY =  176;	const uint16_t PAUSE =  186;	const uint16_t BACKWARD = 143;	
const uint16_t FORWARD = 142;	const uint16_t SETTINGS =  67; 	const uint16_t RED =  114;	const uint16_t GREEN =  113;	const uint16_t YELLOW =  99;	
const uint16_t BLUE =  97;	
unsigned long lastTime;
unsigned long debounceTime = 250;
uint16_t key;

// Relays
Relay relay1(4, 27, IsOn(relay1));
Relay relay2(2, 14, IsOn(relay2));
Relay relay3(15 ,12, IsOn(relay3));
Relay relay4(0, 13, IsOn(relay4));

// IR remote Decode
uint16_t decodedIR(){
  uint16_t command = 0;
  if (IrReceiver.decode()) {
    IrReceiver.resume(); // Enable receiving of the next value
    unsigned long currentTime = millis();
    if ((currentTime - lastTime) >= debounceTime) {
      command = IrReceiver.decodedIRData.command;
      lastTime = currentTime;
    }
  }
  return command;
}

void RemoteKeys(void *parameter){
  key = decodedIR();
  if (key != 0){
    switch (key){
      case PWR:
      break;
      case KEY_1:
      break;
      case KEY_2:
      break;
      case KEY_3:
      break;
      case KEY_4:
      break;
      case KEY_5:
      break;
      case KEY_6:
      break;
      case KEY_7:
      break;
      case KEY_8:
      break;
      case KEY_9:
      break;
      case KEY_0:
      break;
      case KEY_OK:
      break;
      case UP:
      break;
      case LEFT:
      break;
      case RIGHT:
      break;
      case DOWN:
      break;
      case EXIT:
      break;
      case BACK:
      break;
      default:
      break;
    }
  }
}

void TFTdisplay (void *parameter){

}

//void TurnOn

void setup() {
  // Remote init
  IrReceiver.begin(23, ENABLE_LED_FEEDBACK);
  //Relay Controllers pins 
  pinMode(relay1.controlPin, OUTPUT);
  pinMode(relay2.controlPin, OUTPUT);
  pinMode(relay3.controlPin, OUTPUT);
  pinMode(relay4.controlPin, OUTPUT);
  // Thread
  xTaskCreatePinnedToCore(
            RemoteKeys,
            "Remote Keys",
            5000,
            NULL,
            2,
            NULL,
            1);
}

void loop() {
  // put your main code here, to run repeatedly:

}
