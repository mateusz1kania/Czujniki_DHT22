

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WebServer.h>
#include <espnow.h>
#include <Arduino_JSON.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int id;
    float temp;
    float hum;
    String room_name;
} struct_message;

// Create a struct_message called myData
struct_message myData;

JSONVar AllboardsJSON;
//JSONVar board;
String jsonString;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;
struct_message board4;
struct_message board5;
struct_message board6;
struct_message board7;
struct_message board8;
struct_message board9;
struct_message board10;
struct_message board11;

//board1.room_name= 'zielony';
//board2.room_name = 'hol_pietro';
//board3.room_name = "oliwii";
//board4.room_name = "sypialnia";
//board5.room_name = "prysznic";
//board6.room_name = "salon";
//board7.room_name = "hol_parter";
//board8.room_name = "poddasze";
//board9.room_name = "pralnia";
//board10.room_name = "kotlowania";
//board11.room_name = "garaz";

// Create an array with all the structures
struct_message boardsStruct[11] = {board1, board2, board3, board4, board5, board6, board7, board8, board9, board10, board11};


// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  
//  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);

  // Update the structures with the new incoming data
  boardsStruct[myData.id].id = myData.id;
  boardsStruct[myData.id].temp = myData.temp;
  boardsStruct[myData.id].hum = myData.hum;
  
//  Serial.printf("temp value: %g \n", boardsStruct[myData.id].temp);
//  Serial.printf("hum value: %g \n", boardsStruct[myData.id].hum);
//  Serial.println();

  //board["id"] = myData.id;
  //board["temperature"] = myData.temp;
  //board["humidity"] = myData.hum;
  //jsonString = JSON.stringify(AllboardsJSON);  
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  Serial.printf("t value: %g \n", myData.temp);
  Serial.printf("h value: %g \n", myData.hum);
  Serial.println();
}

// Replace with your network credentials
const char* ssid = "37a";
const char* password = "87112602250$$a3r0plane";

#define DHTPIN 5     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// Create AsyncWebServer object on port 80
ESP8266WebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 1,5 minute
const long interval = 100000;  

//// HTML PAGE
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto0;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>MAIN serwer ESP8266 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("allreadings").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/allreadings", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}
//// APIs
void handleAllReadings() { 
  // compile JSON from all readings
    int arrSize = sizeof(boardsStruct)/sizeof(boardsStruct[0]);
    for (int x = 0; x <arrSize; x++) {
        AllboardsJSON[x]["id"] =  boardsStruct[x+1].id;
        AllboardsJSON[x]["temperature"] =  boardsStruct[x+1].temp;
        AllboardsJSON[x]["humidity"] =  boardsStruct[x+1].hum;             
    }
    jsonString = JSON.stringify(AllboardsJSON); 
    server.send(200, "text/plain", jsonString);
    
}

void handleTemperature() { 
  
    server.send(200, "text/plain", String(t).c_str());
    
}
 
void handlehumidity() { 
  
     server.send(200, "text/plain", String(h).c_str());
     
}
void handleindex() { 
  
     server.send(200, "text/html",index_html);
     
}
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
  
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Route for root / web page
//  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/html", index_html, processor);
//  });
//  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/plain", String(t).c_str());
//  });
//  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/plain", String(h).c_str());
//  });
//
//  // Start server
//  server.begin();
    server.on("/", handleindex);
    server.on("/temperature", handleTemperature);
    server.on("/humidity", handlehumidity);
    server.on("/allreadings", handleAllReadings);
 
    server.begin(); //Start the server
    Serial.println("Server listening");
}
 
void loop(){  
  server.handleClient(); //Handling of incoming requests
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    float newT = dht.readTemperature();
    //set value for JSON
    boardsStruct[1].id = 1;

    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
      boardsStruct[1].temp = t;
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
      boardsStruct[1].hum = h;
    }
  }

}
