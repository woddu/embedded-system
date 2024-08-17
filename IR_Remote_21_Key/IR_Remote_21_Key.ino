#include <DIYables_IRcontroller.h> // DIYables_IRcontroller library
#define IR_RECEIVER_PIN 23 // The ESP32 pin GPIO19 connected to IR controller
#define LATCH_SWITCH_PIN 4

DIYables_IRcontroller_21 irController(IR_RECEIVER_PIN, 200); // debounce time is 200ms

void setup() {
  Serial.begin(115200);
  Serial.println("Use the remote");
  irController.begin();
  pinMode(LATCH_SWITCH_PIN, OUTPUT);

}

void loop() {
  Key21 key = irController.getKey();
  if (key != Key21::NONE) {
    switch (key) {
      case Key21::KEY_CH_MINUS:
        Serial.println("CH-");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_CH:
        Serial.println("CH");
        // TODO: YOUR CONTROL

        break;

      case Key21::KEY_CH_PLUS:
        Serial.println("CH+");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_PREV:
        Serial.println("<<");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_NEXT:
        Serial.println(">>");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_PLAY_PAUSE:
        Serial.println(">||");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_VOL_MINUS:
        Serial.println("–");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_VOL_PLUS:
        Serial.println("+");
        // TODO: YOUR CONTROL
        digitalWrite(LATCH_SWITCH_PIN, HIGH);
        //delay(15);
        digitalWrite(LATCH_SWITCH_PIN, LOW);
        
        break;

      case Key21::KEY_EQ:
        Serial.println("EQ");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_100_PLUS:
        Serial.println("100+");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_200_PLUS:
        Serial.println("200+");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_0:
        Serial.println("0");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_1:
        Serial.println("1");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_2:
        Serial.println("2");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_3:
        Serial.println("3");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_4:
        Serial.println("4");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_5:
        Serial.println("5");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_6:
        Serial.println("6");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_7:
        Serial.println("7");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_8:
        Serial.println("8");
        // TODO: YOUR CONTROL
        break;

      case Key21::KEY_9:
        Serial.println("9");
        // TODO: YOUR CONTROL
        break;

      default:
        Serial.println("WARNING: undefined key:");
        break;
    }
  }
  int buttonState = digitalRead(2);
  // print out the state of the button:
  Serial.println(buttonState);
  delay(500);  // delay in between reads for stability
}

