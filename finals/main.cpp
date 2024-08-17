#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiSTA.h>
#include <Preferences.h>
#include <time.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 15

#define DHTTYPE    DHT11

struct tm timeinfo;

AsyncWebServer server(80);

AsyncEventSource events("/events");

Preferences preferences;

DHT_Unified dht(DHTPIN, DHTTYPE);

const char* ssid = "Redmi 9T";
const char* password = "987654321";

const char* PARAM_MOVE = "move";
const char* PARAM_DATE = "date";
const char* PARAM_STOP = "stop";

const String month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};
const String day[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const char index_html[] PROGMEM = R"rawliteral(
<!DOCtype html><html>
<head>
	<meta charset="utf-8">
	<title> SOPHISTICATED </title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { border-radius: 8px; background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); position: relative; padding-right: 80px;}
	  .hep { position: absolute; right: 10px; display: flex; flex-direction: column; justify-content: center; margin-top: 20px;}	      
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
	  .ps { margin-top: 5px; margin-bottom: 5px;}
	  .rightfloat{	position:fixed;	width:60px;	height:60px; bottom:30px; right:40px; background-color:#0C9; color:#FFF;	border-radius:50px;	text-align:center;	box-shadow: 2px 2px 3px #999; }
	  .leftfloat{	position:fixed;	width:60px;	height:60px; bottom:30px; left:40px; background-color:#0C9; color:#FFF;	border-radius:50px;	text-align:center;	box-shadow: 2px 2px 3px #999; }
	  .arrow { border: solid black; border-width: 0 8px 8px 0; display: inline-block; padding: 8px; }
	  .right { transform: rotate(-45deg); -webkit-transform: rotate(-45deg); margin-right: 5px;margin-top: 18px;}
	  .left { transform: rotate(135deg); -webkit-transform: rotate(135deg); margin-left: 5px;margin-top: 18px;}
    .modal { display: none; position: fixed; z-index: 1; padding-top: 70px;  left: 0; top: 0; width: 412px; height: 1650px; overflow: auto; background-color: rgb(0,0,0); background-color: rgba(0,0,0,0.4); }  
    .modal-content { background-color: #fefefe; margin: auto; padding: 20px; border: 1px solid #888; width: 320px; border-radius: 24px;}
    .close { color: #aaaaaa; float: right; font-size: 28px; font-weight: bold; }
  </style>
</head>
<body>
	<div class="topnav">
    <h1>SOPHISTICATED SAES</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <div class="hep">
		  		<p class="ps">Highest</p>
		  		<p id="Htemp" class="ps">%TEMPERATUREST% &deg;C</p>
		  	</div>
        <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> TEMPERATURE</p>
        <p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card">
        <div class="hep">
		  		<p class="ps">Highest</p>
		  		<p id="Hhum" class="ps">%HUMIDITYST% &percnt;</p>
		  	</div>
        <p><i class="fas fa-tint" style="color:#00add6;"></i> HUMIDITY</p>
        <p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></p>
      </div>
      <div class="card">
        <div class="hep">
		  		<p class="ps">Highest</p>
		  		<p id="Hpeop" class="ps">%PEOPLEST%</p>
		  	</div>
        <p><i 	class="fas fa-male" style="color:#0078D7;"></i> PEOPLE</p>
				<p><span class="reading"><span id="peop">%PEOPLE%</span></span></p>
      </div>      
    </div>
  </div>
  <div id="ModalT" class="modal">
    <!-- Modal content -->
    <div class="modal-content">
      <span id="closeT" class="close">&times;</span>
      <p><span class="reading">Temperature History</span></p>
      <p><span class="reading">%TDATE%</span></p>
      <p>%THISTORY%</p>
    </div>
  </div>
  <div id="ModalH" class="modal">
    <!-- Modal content -->
    <div class="modal-content">
      <span id="closeH" class="close">&times;</span>
      <p><span class="reading">Humidity History</span></p>
      <p><span class="reading">%HDATE%</span></p>
      <p>%HHISTORY%</p>
    </div>
  </div>
  <div id="BtnH" class="leftfloat">
    <h1 style="margin-top: 14px; margin-right:1px;">H</h1>    
  </div>
  <div id="BtnT" class="rightfloat">    
    <h1 style="margin-top: 14px; margin-right:1px;">T</h1>      
  </div>
	<script>
    var d;
    var modalT = document.getElementById("ModalT");
    var btnT = document.getElementById("BtnT");
    var spanT = document.getElementById("closeT");
    var modalH = document.getElementById("ModalH");
    var btnH = document.getElementById("BtnH");
    var spanH = document.getElementById("closeH");
    btnT.onclick = function() {
      modalT.style.display = "block";
    }
    spanT.onclick = function() {
      modalT.style.display = "none";
    }
    btnH.onclick = function() {
      modalH.style.display = "block";
    }
    spanH.onclick = function() {
      modalH.style.display = "none";
    }
    window.onclick = function(event) {
      if (event.target == modalT) {
        modalT.style.display = "none";
      } else if (event.target == modalH){
        modalH.style.display = "none";
      }
    }
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

      source.addEventListener('people', function(e) {
        console.log("humidity", e.data);
        document.getElementById("peop").innerHTML = e.data;
      }, false);

      source.addEventListener('Htemperature', function(e) {
        console.log("Htemperature", e.data);
        document.getElementById("Htemp").innerHTML = e.data;
      }, false);

      source.addEventListener('Hhumidity', function(e) {
        console.log("Hhumidity", e.data);
        document.getElementById("Hhum").innerHTML = e.data;
      }, false);

      source.addEventListener('Hpeople', function(e) {
        console.log("Hhumidity", e.data);
        document.getElementById("Hpeop").innerHTML = e.data;
      }, false);

    }
    function updateDate(){
      d = new Date();
      let year = d.getFullYear();
      let month = d.getMonth();
      let date = d.getDate();
      let day = d.getDay();
      let hour = d.getHours();
      let min = d.getMinutes();
      let sec = d.getSeconds();
      let dateFormat = `${month} ${date} ${year} ${day} ${hour}:${min}:${sec}`
      var xhr = new XMLHttpRequest();
			xhr.open("GET", "/date?date="+ dateFormat, true);
			xhr.send();
    }
    window.onload = updateDate;
	</script>
</body></html>
)rawliteral";

uint32_t delayMS;

bool updated;

const int ir[] = {26, 27};
const int fan[] = {14, 12};

float highesttemperature, highesthumidity;
float temperature, humidity;

int highestperson;
int person;

int currentHour;
int currentMin;
void IR(int curr, int prev, void (*f1)(), void (*f2)()){
  if (curr == 0 && prev == 1){
    f1();
  } else if (curr == 1 && prev == 0){
    f2();
  }
}

void appendHistoryT(String temp){
  preferences.begin("Temperature", false);
  preferences.putString("h0",preferences.getString("h1"));
  preferences.putString("h1",preferences.getString("h2"));
  preferences.putString("h2",preferences.getString("h3"));
  preferences.putString("h3",preferences.getString("h4"));
  preferences.putString("h4",preferences.getString("h5"));
  preferences.putString("h5",preferences.getString("h6"));
  preferences.putString("h6",preferences.getString("h7"));
  preferences.putString("h7",preferences.getString("h8"));
  preferences.putString("h8",temp);
  preferences.end();
}

void appendHistoryH(String humid){
  preferences.begin("Humidity", false);
  preferences.putString("h0",preferences.getString("h1"));
  preferences.putString("h1",preferences.getString("h2"));
  preferences.putString("h2",preferences.getString("h3"));
  preferences.putString("h3",preferences.getString("h4"));
  preferences.putString("h4",preferences.getString("h5"));
  preferences.putString("h5",preferences.getString("h6"));
  preferences.putString("h6",preferences.getString("h7"));
  preferences.putString("h7",preferences.getString("h8"));
  preferences.putString("h8",humid);
  preferences.end();  
}

void appendHistoryP(String count){
  preferences.begin("People", false);
  preferences.putString("h0",preferences.getString("h1"));
  preferences.putString("h1",preferences.getString("h2"));
  preferences.putString("h2",preferences.getString("h3"));
  preferences.putString("h3",preferences.getString("h4"));
  preferences.putString("h4",preferences.getString("h5"));
  preferences.putString("h5",count);  
  preferences.end();
}

String currentTime(){
  //getLocalTime(&timeinfo);
  String times;
  times += month[timeinfo.tm_mon];
  times += " ";
  times += timeinfo.tm_mday;
  times += ", ";
  times += timeinfo.tm_year + 1900;
  times += " - ";
  times += day[timeinfo.tm_wday];
  times += " ";
  times += (timeinfo.tm_hour > 12 ? timeinfo.tm_hour - 12 : timeinfo.tm_hour);
  times += ":";
  times += timeinfo.tm_min;
  times += " ";
  times += (timeinfo.tm_hour >= 12 ? "PM" : "AM");
  return times;
}

String processor(const String& var){
  //Serial.println(var);
  if(var == "DATETIME"){
    String time = "";
    time += "<p>"+ (updated ? "Relaod pls" : currentTime()) +"</p>";    
    return time;
  } else if(var == "THISTORY"){
    preferences.begin("Temperature", false);
    String history = "";    
    history += preferences.getString("h8") + "<br>";    
    history += preferences.getString("h7") + "<br>";    
    history += preferences.getString("h6") + "<br>"; 
    history += preferences.getString("h5") + "<br>";    
    history += preferences.getString("h4") + "<br>";    
    history += preferences.getString("h3") + "<br>";    
    history += preferences.getString("h2") + "<br>";    
    history += preferences.getString("h1") + "<br>";    
    history += preferences.getString("h0");    
    preferences.end();
    return history;
  } else if(var == "HHISTORY"){
    preferences.begin("Humidity", false);
    String history = "";
    history += preferences.getString("h8") + "<br>";    
    history += preferences.getString("h7") + "<br>";    
    history += preferences.getString("h6") + "<br>";
    history += preferences.getString("h5") + "<br>";    
    history += preferences.getString("h4") + "<br>";    
    history += preferences.getString("h3") + "<br>";    
    history += preferences.getString("h2") + "<br>";    
    history += preferences.getString("h1") + "<br>";    
    history += preferences.getString("h0");
    preferences.end();    
    return history;
  } else if(var == "PHISTORY"){
    preferences.begin("People", false);
    String history = "";
    history += "<a>" + preferences.getString("h5") + "<br>";    
    history += preferences.getString("h4") + "<br>";    
    history += preferences.getString("h3") + "<br>";    
    history += preferences.getString("h2") + "<br>";    
    history += preferences.getString("h1") + "<br>";    
    history += preferences.getString("h0") +"</a>";    
    preferences.end();
    return history;
  } else if(var == "TEMPERATURE"){
    return String(temperature);
  } else if(var == "HUMIDITY"){
    return String(humidity);
  } else if(var == "PEOPLE"){
    return String(person);
  } else if(var == "TEMPERATUREST"){
    return String(highesttemperature);
  } else if(var == "HUMIDITYST"){
    return String(highesthumidity);
  } else if(var == "PEOPLEST"){
    return String(highestperson);
  } else if(var == "TDATE"){
    String hist = "";
    preferences.begin("Temperature", false);
    hist += preferences.getString("Highest_Date");        
    preferences.end();
    return hist;
  } else if(var == "HDATE"){
    String hist = "";
    preferences.begin("Humidity", false);
    hist += preferences.getString("Highest_Date");    
    preferences.end();
    return hist;
  }
  return String();  
}

bool right, left;
unsigned long t1, t2, tOut;
void IRSensor(void * param){
  int prev[2], curr[2];
  while(1){
    for(int i = 0; i < 2;i++)
      prev[i] = curr[i];
    for(int i = 0; i < 2;i++)
      curr[i] = digitalRead(ir[i]);    
    IR(curr[0],prev[0],
      [](){
        if(!right && !left){
          t1 = millis();
          left = true;
          right = false;
        } else if (right && !left){
          t2 = millis();
          if(t2 - t1 <= 1000){
            person++;
            Serial.println(highestperson);
            events.send("ping",NULL,millis());
            events.send(String(person).c_str(),"people",millis());
          }
          right = left = false;
          t2 = t1 = 0;
        }
      },
      [](){
        
      }
    );
    IR(curr[1],prev[1],
      [](){
        if(!right && !left){
          t1 = millis();
          left = false;
          right = true;
        } else if (!right && left){
          t2 = millis();
          if(t2 - t1 <= 1000){
            person--;
            Serial.println(person);
            if(person < 0) person = 0;
            events.send("ping",NULL,millis());
            events.send(String(person).c_str(),"people",millis());
          }
          right = left = false;
          t2 = t1 = 0;
        }
      },
      [](){
        
      }
    );
    tOut = millis();
    if (tOut - t1 >= 1250){
      right = left = false;
      t1 = t2 = tOut = 0;
    }
    if(highestperson < person){
      preferences.begin("People", false);
      preferences.putInt("Highest", person);
      preferences.putString("Highest_Date", String(person) + "  " + currentTime());
      highestperson = preferences.getInt("Highest");
      events.send(String(highestperson).c_str(),"Hpeople",millis());
      preferences.end();
    }
    if(person > 0 && person < 8){
      digitalWrite(fan[0], HIGH);
      digitalWrite(fan[1], LOW);
    } else if (person >= 8 || temperature >= 36.00){
      digitalWrite(fan[0], HIGH);
      digitalWrite(fan[1], HIGH);
    } else if (person <= 0 ){
      digitalWrite(fan[0], LOW);
      digitalWrite(fan[1], LOW);
    }
    delay(125);
  }
}

void Time(void * param){
  
  while(1){
    getLocalTime(&timeinfo,1000);
    Serial.printf("currentMin = %d, tm_min = %d  t - c = %d\n",currentMin, timeinfo.tm_min, timeinfo.tm_min - currentMin);
    if(timeinfo.tm_min - currentMin >= 5 || timeinfo.tm_min - currentMin == -55){
      Serial.println("appending...");
      preferences.begin("Time", false);
      preferences.putInt("CurrMin", timeinfo.tm_min);
      currentMin = preferences.getInt("CurrMin");
      preferences.end();
      appendHistoryT(String(temperature) + " °C " + currentTime());
      appendHistoryH(String(humidity) + " &percnt; " + currentTime());
      appendHistoryP(String(person) + " " + currentTime());
    }    
    delay(1000);
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  for(int i = 0; i < 2;i++)
    pinMode(ir[i], INPUT);
  for(int i = 0; i < 2;i++)
    pinMode(fan[i], OUTPUT); 

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFi Failed!\n");
      return;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/date", HTTP_GET, 
    [](AsyncWebServerRequest *request) {
      String date;    
      if (request->hasParam(PARAM_DATE) ) {
        date = request->getParam(PARAM_DATE)->value();
        Serial.println(date+"  min ->" + date.substring(14, 16));
        if(!updated){ 
          timeinfo.tm_year = date.substring(4, 8).toInt() - 1900;
          timeinfo.tm_mon = date.substring(0, 1).toInt();
          timeinfo.tm_mday = date.substring(2, 3).toInt();
          timeinfo.tm_wday = date.substring(9, 11).toInt();
          timeinfo.tm_hour = date.substring(11, 13).toInt();
          timeinfo.tm_min = date.substring(14, 16).toInt();
          timeinfo.tm_sec = date.substring(17, 19).toInt();
          Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
          time_t t = mktime(&timeinfo);
          printf("Setting time: %s", asctime(&timeinfo));
          struct timeval now = { .tv_sec = t };
          settimeofday(&now, NULL);     
          updated = true;
          preferences.begin("Time", false);
          preferences.putInt("CurrMin", timeinfo.tm_min);
          currentMin = preferences.getInt("CurrMin");
          preferences.end();
        }   
      }
    }
  );
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
  // Start server
  server.begin();

  xTaskCreatePinnedToCore(
    IRSensor,
    "IR Sensor",
    5000,
    NULL,
    1,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    Time,
    "Time",
    5000,
    NULL,
    1,
    NULL,
    1
  );
  
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  preferences.begin("Temperature", false);    
  highesttemperature = preferences.getFloat("Highest");
  preferences.end();
  preferences.begin("Humidity", false);    
  highesthumidity = preferences.getFloat("Highest");
  preferences.end();
  preferences.begin("People", false);
  highestperson = preferences.getInt("Highest");
  preferences.end();
  
  Serial.print("IP Address: ");
  
  Serial.println(WiFi.localIP());


}

void loop() {
  delay(delayMS);
  events.send("ping",NULL,millis());
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    events.send("Error!","temperature",millis());
  } else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    temperature = event.temperature;
    Serial.println(F("°C"));
    events.send(String(temperature).c_str(),"temperature",millis());
    if(temperature > highesttemperature){
      preferences.begin("Temperature", false);
      preferences.putFloat("Highest", temperature);
      preferences.putString("Highest_Date", String(temperature) +" °C  " + currentTime());
      highesttemperature = preferences.getFloat("Highest");
    events.send(String(highesttemperature).c_str(),"Htemperature",millis());
      preferences.end();
    }
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    events.send("Error!","humidity",millis());
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    humidity = event.relative_humidity;
    Serial.println(F("%"));
    events.send(String(humidity).c_str(),"humidity",millis());
    if(humidity > highesthumidity){
      preferences.begin("Humidity", false);
      preferences.putFloat("Highest", humidity);
      preferences.putString("Highest_Date", String(humidity) + " &percnt;  " + currentTime());
      highesthumidity = preferences.getFloat("Highest");
      events.send(String(highesthumidity).c_str(),"Hhumidity",millis());
      preferences.end();
    }
  }
}