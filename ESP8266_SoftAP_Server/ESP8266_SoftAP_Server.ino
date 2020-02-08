#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define led1 5                //D1


DynamicJsonBuffer jsonBuffer;

const char *ssid      = "PR001";
const char *password  = "213456789";
       
int sensorValue1 = 0;              
String sensor_values;

ESP8266WebServer server(80);

void handleSentVar() {

  if (server.hasArg("sensor_reading"))
  {
    sensor_values = server.arg("sensor_reading");
    Serial.println(sensor_values);
  }
  JsonObject& root = jsonBuffer.parseObject(sensor_values);
//  if (!root.success()) {
//    Serial.println("parseObject() failed");
//    return;
//  }
//  if (root.success())
//  {
    //sensorValue0          = root["sensor0_reading"].as<int>();
    sensorValue1          = root["sensor1_reading"].as<int>();
    //sensorValue2          = root["sensor2_reading"].as<int>();
   // sensorValue3          = root["sensor3_reading"].as<int>();

//  }

  Serial.println(sensorValue1);

  toggle_leds();

  server.send(200, "text/html", "Data received");
}


void setup() {
  Serial.begin(9600);

  Serial.println();
  Serial.println("Booting.... ");
  Serial.print(ssid);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  pinMode(led1, OUTPUT);
  
  //toggle_leds();                 //turn off all leds as all the sensor values are zero
  
  server.on("/data/", HTTP_GET, handleSentVar); // when the server receives a request with /data/ in the string then run the handleSentVar function
  server.begin();
}

void loop() {
  server.handleClient();
}

void toggle_leds()
{
  if (sensorValue1 == 0)  digitalWrite(led1, LOW);

  if (sensorValue1 == 1)  digitalWrite(led1, HIGH);
}
