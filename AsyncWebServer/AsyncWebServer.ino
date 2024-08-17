#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiSTA.h>
#include <Preferences.h>

#define SOUND_SPEED 0.034

// Replace with your network credentials
const char* ssid = "OPPO A12";
const char* password = "12345678";

Preferences preferences;
AsyncWebServer server(80);

const char* PARAM_A = "a";
const char* PARAM_K = "type";
const char* PARAM_L = "l";
const char* PARAM_M = "move";
const char* PARAM_T = "t";
const char* PARAM_S = "s";

const uint8_t trigger = 5;
const uint8_t echo = 18;

int soil, water;
int soilLevel, waterLevel;

long duration;
double distanceCm;

void UDSGetDistance(){
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}
bool s1 = false, s2 = false;
void setup() {
  Serial.begin(115200);
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);

  preferences.begin("History", false);

  WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }


    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  /*
  // Print ESP Local IP Address
  Serial.println( WiFi.softAPIP());
  server.on("/m", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String move;
    if (request->hasParam(PARAM_M)) {
      move = request->getParam(PARAM_M)->value();
      if(move == "left") {
        Serial.println(move);
      } else if(move == "right"){
       Serial.println(move);
      } else if(move == "up") {
        Serial.println(move);
      } else if(move == "down") {
        Serial.println(move);
      } else if(move == "click") {
        Serial.println(move);
      } 

    }          
  });

  server.on("/k", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String type;
    if (request->hasParam(PARAM_K)) {
      type = request->getParam(PARAM_K)->value();
      Serial.println(type);
    }              
  });
  server.on("/s", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String shutdown;
    if (request->hasParam(PARAM_S)) {
      shutdown = request->getParam(PARAM_S)->value();
      if (shutdown == "true"){
        Serial.println(shutdown);
      }
    }              
  });
  */
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", " ");
    });
    
    server.on("/a", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String a;
        if (request->hasParam(PARAM_A)) {
            a = request->getParam(PARAM_A)->value();
            int b = a.toInt();
            Serial.println(b);
            switch (b){
            case 1:
              request->send(200, "text/plain", String(soilLevel));
              break;
            case 2:
              request->send(200, "text/plain", String(waterLevel));
              break;
            case 3:
              
              s1 = !s1;
              digitalWrite(13, s1);
              request->send(200, "text/plain", (s1 ? "ON" : "OFF"));
              break;
            case 4:
              
              s2 = !s2;                
              digitalWrite(12, s2);
              request->send(200, "text/plain", (s2 ? "ON" : "OFF"));
              break;
            case 5:
              request->send(200, "text/plain", (s1 ? "ON" : "OFF"));
              break;
            case 6:
              request->send(200, "text/plain", (s2 ? "ON" : "OFF"));
              break;
            default:

              break;
          }
        } else {
            a = "No message sent";
        }              
    });
    
    // Send a GET request to <IP>/get?message=<message>
    server.on("/t1", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_T)) {
          message = request->getParam(PARAM_T)->value();           
          preferences.putString("a",message);
          message = preferences.getString("a","None");
          request->send(200, "text/plain", message + "Dilig");
        } else {
          message = "No message sent";
        }                
    });
    server.on("/t2", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_T)) {
          message = request->getParam(PARAM_T)->value();
          preferences.putString("b",message);
          message = preferences.getString("b","None");
          request->send(200, "text/plain", message + "Igib");
        } else {
          message = "No message sent";
        }                
    });
    server.on("/l1", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_L)) {
          message = request->getParam(PARAM_L)->value();
          message = preferences.getString("a","None");
          request->send(200, "text/plain", message + " Soil Valve");          
        } else {
          message = "No message sent";
        }                
    });
    server.on("/l2", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_L)) {
          message = request->getParam(PARAM_L)->value();
          message = preferences.getString("b","None");
          request->send(200, "text/plain", message + " Tank Valve");
        } else {
          message = "No message sent";
        }                
    });
    /*
    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });
    */
    server.onNotFound(notFound);
    server.begin();
}

void loop() {
  UDSGetDistance();
  soil = analogRead(A0);
  soilLevel = ( 100 - ( (soil/4095.00) * 100 ) );
  soilLevel = map(soilLevel, 0, 100, 0, 100);
  waterLevel = map(distanceCm, 9.6, 4, 0, 100);
  if (waterLevel < 0){ waterLevel = 0;} else if (waterLevel > 100){ waterLevel = 100;}
  delay(250);
}