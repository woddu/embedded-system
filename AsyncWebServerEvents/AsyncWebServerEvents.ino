#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Cha-Choi";
const char* password = "joselynparin";

const char* PARAM_A = "a";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

int a, b, c;

String processor(const String& var){

  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(a);
  }
  else if(var == "HUMIDITY"){
    return String(b);
  }
  else if(var == "PRESSURE"){
    return String(c);
  }
  return String();
}

const char get_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
  </style>
</head>
<body>
  <p><span id="temp">%TEMPERATURE%</span> </span>
  <span id="hum">%HUMIDITY%</span> </span>
  <span id="pres">%PRESSURE%</span> </span></p>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('humidity', function(e) {
  console.log("humidity", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
 
 source.addEventListener('pressure', function(e) {
  console.log("pressure", e.data);
  document.getElementById("pres").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";

void setup() {
  Serial.begin(115200);
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while(1);
  }
  
  // Print ESP Local IP Address
  Serial.println( WiFi.softAPIP());

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "ola");
  });

  // Send a GET request to <IP>/get?a=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_A)) {
            message = request->getParam(PARAM_A)->value();
        } else {
            message = "No message sent";
        }
        if(message == "abc"){
          request->send_P(200, "text/html", get_html, processor);          
        } else {
          request->send(200, "text/plain", "what");
        }
    });
  
  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}
unsigned long t1, t2, t3;
void loop() {    
  if (millis() - t1 >= 250){
    events.send("ping",NULL,millis());
    t1 = millis();
    a = random(0,10);
    events.send(String(a).c_str(),"temperature",millis());
  }
  if (millis() - t2 >= 500){
    t2 = millis();
    b = random(11,20);
    events.send(String(b).c_str(),"humidity",millis());
  }
  if (millis() - t3 >= 750){
    t3 = millis();
    c = random(21,30);  
    events.send(String(c).c_str(),"pressure",millis());  
  }
}
