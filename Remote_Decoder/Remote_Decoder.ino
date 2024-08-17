#include <IRremote.hpp>

unsigned long lastTime;
unsigned long debounceTime = 500;
uint16_t ir;

void setup() {
  // put your setup code here, to run once:
  IrReceiver.begin(23, ENABLE_LED_FEEDBACK);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  ir = decodedIR();
  if (ir != 0){
    Serial.println(ir);
    delay(500);
  }
  //switch (ir){
  //  case 21:
  //  Serial.println("21");
  //  Serial.println(ir);
  //  break;
  //}
}
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


