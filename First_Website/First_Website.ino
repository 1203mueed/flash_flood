#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Wire.h>

// Replace with your network credentials
const char* ssid = "Mueed"; 
const char* password = "1234567890";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

int trigpin = 13;
int echopin =12;
float distance;
float duration;
bool start = LOW;

String getWaterLevel() {
  digitalWrite(trigpin,LOW);
  delay(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH);
  distance = (duration*0.034)/2;
  delay(1000);
  //String string_distance =  String(distance);
  if (isnan(distance)) {    
    Serial.println("Failed to read from sensor!");
    return "";
  }
  else {
    Serial.print(distance);
    Serial.print("cm\n");
    return String(distance);
  }
}

void setup(){
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  //Serial.begin(9600);
  
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  
  // Initialize SPIFFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html");
  });
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getWaterLevel().c_str());
  });
//  server.on("/pause", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/plain", readBME280Humidity().c_str());
//  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
