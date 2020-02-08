#include <ESP8266WiFi.h>


#define btn1 5

const char *ssid      = "PR001";
const char *password  = "213456789";

//sensor value, I'm usingg 0/1 button state
int sensorVal = 0;        
int prevSensorVal = 0;

unsigned long lastSend = 0;

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(btn1, INPUT);


  // set the ESP8266 to be a WiFi-client
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println();
  Serial.print("WIFI CONNECTED");

}

void loop() {
  /*
  if(digitalRead(btn1) == HIGH && prevSensorVal == 0){
    prevSensorVal = sensorVal;
    sensorVal = 1;
  }
  else if(digitalRead(btn1) == LOW){
    prevSensorVal = sensorVal;
    sensorVal=0;
  }
  else{
    sensorVal=0;
  }
  */
  unsigned long currentTime = millis();
  if(currentTime > lastSend + 3500){
    sensorVal = 1;
    lastSend = currentTime;
  }
  else{
    sensorVal = 0;
  }

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const char * host = "192.168.4.1";            //default IP address
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request. Something like /data/?sensor_reading=123
  String url = "/data/";
  url += "?sensor_reading=";
  url +=  "{\"sensor1_reading\":\"sensor1_value\"}";

  url.replace("sensor1_value", String(sensorVal));

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
               
  Serial.println(url);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop(); 
      return;
    }
  }
}
