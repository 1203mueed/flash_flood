#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>

const char* ssid = "Mueed"; 
const char* password = "1234567890";


ESP8266WebServer server(80);

const char* host = "script.google.com";
const int httpsPort = 443;

WiFiClientSecure client; //--> Create a WiFiClientSecure object.

String GAS_ID = "AKfycbz10GBIvo7GLs91FydSvZF4UQcSHI6utGy1v7UXLzCt7wRSy0nJQPi1enWN2TgAl4AS"; 

int trigpin = 13;
int echopin =12;
float distance;
float duration;
bool start = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  Serial.begin(9600);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check NodeMCU is connected to Wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());

  client.setInsecure();

  server.on("/", handle_OnConnect);
  server.on("/start", handle_start);
  server.on("/pause", handle_pause);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP Server Started");
  sendData(WiFi.localIP().toString());
}

void loop() {
  server.handleClient();

  if(start){
    digitalWrite(trigpin,LOW);
    delay(2);
    digitalWrite(trigpin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigpin, LOW);
    duration = pulseIn(echopin, HIGH);
    distance = (duration*0.034)/2;
    Serial.print(distance);
    Serial.print("cm\n");
    delay(1000);
    String string_distance =  String(distance);
    sendData(string_distance);
  }else {
    
  }
}

  void handle_OnConnect() {
  start = LOW;
  Serial.println("Device: OFF");
  server.send(200, "text/html", updateWebpage(start)); 
}

void handle_start() {
  start = HIGH;
  Serial.println("Device Started");
  server.send(200, "text/html", updateWebpage(start)); 
}

void handle_pause() {
  start = LOW;
  Serial.println("Devise Paused");
  server.send(200, "text/html", updateWebpage(start)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String updateWebpage(uint8_t start){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Water Level Measurement</title>\n";
  ptr +="<style>html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #3498db;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Water Level Measurement</h1>\n";
  
  
   if(start){
    ptr +="<p>Device Started</p><a class=\"button button-off\" href=\"/pause\">Pause</a>\n";
   }else{
    ptr +="<p>Device Paused</p><a class=\"button button-on\" href=\"/start\">Start</a>\n";
   }

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void sendData(String distance) {
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/macros/s/" + GAS_ID + "/exec?distance=" + distance;



  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");


  if(!client.connected()){
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  }
} 








