#include <ESP8266WiFi.h>


#define btn 13
#define LEDr 0
#define LEDb 2

const char *ssid      = "PR001";
const char *password  = "213456789";

//sensor value, I'm usingg 0/1 button state
int sensorVal = 0;        
int prevSensorVal = 0;

unsigned long lastT = 0;
unsigned long currentT = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\rBooting:");
  delay(10);

  pinMode(btn, INPUT_PULLUP);
  pinMode(LEDr, OUTPUT);
  pinMode(LEDb, OUTPUT);
  digitalWrite(LEDr, LOW); // Turn on RED LED
  digitalWrite(LEDb, HIGH);

  // set the ESP8266 to be a WiFi-client
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\t Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("\t WIFI CONNECTED");
  digitalWrite(LEDr, HIGH);
  
  Serial.println("SETUP COMPLETE!");
  delay(2500);
}

void loop() {
  if(digitalRead(btn) == LOW && prevSensorVal == 0){
    sensorVal = 1;
    digitalWrite(LEDb, LOW); // Turn on Blue
    digitalWrite(LEDr, LOW); // Turn on RED LED
  }
  else if(digitalRead(btn) == HIGH){
    sensorVal=0;
    digitalWrite(LEDb, HIGH); // Turn off Blue
  }
  else{
    sensorVal=0;
    digitalWrite(LEDb, HIGH); // Turn on Blue
  }

  // Only Send a 0 every 1 second..
  currentT = millis();
  Serial.print("\n\r T: ");
  Serial.print((currentT - lastT));
  Serial.print(" LAST: ");
  Serial.print(prevSensorVal);
  Serial.print(" NOW: ");
  Serial.print(sensorVal);
  if(sensorVal == 0 && prevSensorVal == 0 && currentT < (lastT + 1000)){
    Serial.print("           Spam!");
    return; 
  }
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const char * host = "192.168.4.1";            //default IP address
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    digitalWrite(LEDr, LOW); // Turn on RED LED
    return;
  }

  prevSensorVal = sensorVal;
  lastT = millis();

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
      digitalWrite(LEDr, LOW); // Turn on RED LED
      Serial.println(">>> Client Timeout !");
      client.stop(); 
      return;
    }
  }

  digitalWrite(LEDr, HIGH); // Turn off Red
}
