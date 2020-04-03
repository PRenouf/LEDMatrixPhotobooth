#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FastLED.h>

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define LED_PIN  4
#define IndicatorLED 5                //D1
#define TriggerLED 15
#define focus 12
#define shutter 14


// LED Matrix Parameters
const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

DynamicJsonBuffer jsonBuffer;

const char *ssid      = "PR001";
const char *password  = "213456789";
       
int command = 0;              
String sensor_values;


unsigned long previousMillis = 0;
unsigned long currentMillis = 0;


ESP8266WebServer server(80);

void handleSentVar() {

  if (server.hasArg("sensor_reading"))
  {
    sensor_values = server.arg("sensor_reading");
    Serial.print(sensor_values);
    currentMillis = millis();
  }
  else
  {
    return;
  }
  
  JsonObject& root = jsonBuffer.parseObject(sensor_values);
  command = root["sensor1_reading"].as<int>();
  Serial.print("   Command: ");
  Serial.print(command);
  Serial.println(root["sensor1_reading"].as<int>());
  
  // IDLE Mode - Flash LED
  if(command == 0){
    //Toggle the state of the LED
    if (currentMillis > previousMillis + 500){
      digitalWrite(IndicatorLED, !digitalRead(IndicatorLED));
      previousMillis = currentMillis;
    }
  }
  else if(command == 1){
    takePhoto();    
  }

  server.send(200, "text/html", "Data received");
}

void takePhoto(){
  digitalWrite(TriggerLED, HIGH);
  count(3, false); //Count down 3,2,1
  flash();
  digitalWrite(shutter, HIGH);
  Serial.println("trigger active");
  delay(150);
  digitalWrite(shutter, LOW);
  Serial.println("trigger off");
  digitalWrite(TriggerLED, LOW);  
}


void setup() {
  Serial.begin(9600);

  Serial.println();
  Serial.println("Booting.... ");
  Serial.print(ssid);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  pinMode(IndicatorLED, OUTPUT);
  pinMode(TriggerLED, OUTPUT);
  pinMode(shutter, OUTPUT);
  pinMode(focus, OUTPUT);

  digitalWrite(shutter, LOW);
  digitalWrite(focus, LOW);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  //FastLED.setBrightness( BRIGHTNESS );

  uint32_t ST = millis();
  uint32_t ms = 0;
  while (ms < ST + 5000) {
    ms = millis();
    createFrame(ms, true);
    drawSmallOne(1);
    drawSmallTwo(2);
    drawSmallThree(3);
    drawSmallFour(4);
    FastLED.show();
    delay(10);
  }

  server.on("/data/", HTTP_GET, handleSentVar); // when the server receives a request with /data/ in the string then run the handleSentVar function
  server.begin();

  Serial.println("Setup Complete.");
}

void loop() {
  server.handleClient();

  uint32_t ms = millis();
  createFrame(ms, true);

  FastLED.show();
  delay(10);
}
