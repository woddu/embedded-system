#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiAP.h>
AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
// const char* ssid = "S2 Mini";
// const char* password = "12345678";

const char* ssid = "Cha-Choi";
const char* password = "joselynparin";

String input1;
const char* PARAM_INPUT_1 = "input1";

// HTML web page to handle 3 input fields (input1, input2, input3)       
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>ESP Input Form</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      html { font-family: Helvetica; display: inline-block; text-align: center;}
      body {max-width: 600px; max-height: 600px; margin:0px auto; padding-bottom: 25px;}
      .forme { width: 250px; height: 380px; position: absolute; top: 51%; left: 50%; transform: translate(0%,-5%); border-radius: 10px; padding: 25px; }
      .forme input { width: 240px; height: 35px; background: transparent; border-bottom: 1px solid #ff7200; border-top: none; border-right: none; border-left: none; color: #343434; font-size: 15px; letter-spacing: 1px; margin-top: 30px; font-family: sans-serif; }
      .forme .formsubmit {  width: 240px; height: 40px; background: #ff7200; border: none; margin-top: 30px; font-size: 18px; border-radius: 10px; color: #fff; }
    </style>
    <script>
      function submitMessage(){
        alert("Sent Value to ESP");
        setTimeout(function(){ document.location.reload(false);},500);
      }
    </script>
  </head>
  <body>
    <form class="forme" action="/get" target="hidden-form">
      <input type="text" name="input1" placeholder="%input1%">
      <input class="formsubmit" type="submit" value="Submit" onClick="submitMessage()">
    </form>
    <iframe style="display:none" name="hidden-form"></iframe>
  </body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String sendStrings(const String& var){
  if (var == "input1"){
    return input1;
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while(1);
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, sendStrings);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      input1 = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    //request->send(200, "text/html", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  
}