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
int photoNumber = 0;

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long prevLEDUpdate = 0;
unsigned long currentLEDUpdate = 0;


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


void setup() {
  digitalWrite(16, HIGH);
  pinMode(16, OUTPUT);
  digitalWrite(16, HIGH);
  
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
  pinMode(13, INPUT_PULLUP);

  digitalWrite(shutter, LOW);
  digitalWrite(focus, LOW);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  //FastLED.setBrightness( BRIGHTNESS );

  server.on("/data/", HTTP_GET, handleSentVar); // when the server receives a request with /data/ in the string then run the handleSentVar function
  server.begin();

  Serial.println("Setup Complete.");

  photoNumber = 24;
      
}

void loop() {
  server.handleClient();
  
  currentLEDUpdate = millis();
  if(currentLEDUpdate > prevLEDUpdate + 500){

    uint32_t ms = millis();
    createFrame(ms, true);

    FastLED.show();
    delay(10);
  }
  
  //takePhoto();
  if (digitalRead(13) == LOW) {
    currentMillis = millis();
    takePhoto();
  }


  // 35 Seconds since the last photo
  if(currentLEDUpdate > currentMillis + 35000){
    //Serial.println("INACTIVE - REBOOTING");
    digitalWrite(16, LOW); // REBOOT  
  }

  if(photoNumber == 25){
    fuckYou();
    photoNumber = 0;
  }
}


///
unsigned long startTime = 0;
unsigned long currentTime = 0;

  
void takePhoto(){
  digitalWrite(TriggerLED, HIGH);
  
  startTime = millis();

  Serial.print("\t Counting... 3...");
  while(currentTime < startTime + 1000){
    createFrame(currentTime, false);
    drawBigThree();
        FastLED.show();
    delay(10);
    currentTime = millis();
  }

  Serial.print(" 2...");
  while(currentTime < startTime + 2000){
    createFrame(currentTime, false);
    drawBigTwo();
        FastLED.show();
    delay(10);
    currentTime = millis();
  }

  Serial.print(" 1...");
  while(currentTime < startTime + 3000){
    createFrame(currentTime, false);
    drawBigOne();
        FastLED.show();
    delay(10);
    currentTime = millis();
  }

  Serial.print(" ...SMILE");
  createFrame(currentTime, false);
  smile();
  FastLED.show();
  delay(10);
  digitalWrite(focus, HIGH);
  delay(200);
  digitalWrite(focus, LOW);
  digitalWrite(shutter, HIGH);
  delay(200);
  digitalWrite(shutter, LOW);
  delay(100);

  photoNumber = photoNumber + 1;
  
  Serial.println("  ... Photo taken!");
  digitalWrite(TriggerLED, LOW);
  digitalWrite(16, LOW);
}

void smile(){
  drawSquare2(2,2);
  drawSquare2(3,2);
  drawSquare2(6,2);
  drawSquare2(7,2);
  drawSquare2(2,3);
  drawSquare2(3,3);
  drawSquare2(6,3);
  drawSquare2(7,3);

  drawSquare2(1,5);
  drawSquare2(2,6);
  drawSquare2(3,7);
  drawSquare2(4,7);
  drawSquare2(5,7);
  drawSquare2(6,7);
  drawSquare2(7,6);
  drawSquare2(8,5);
}
void fuckYou(){
  for(int x = 0; x < 16; x++){
    for(int y = 0; y < 16; y++){
      leds[ XY(x,y)] = CRGB( 0, 0, 0);
    }
  }

  // F
  leds[ XY(14,2)] = CRGB(255, 0, 0);
  leds[ XY(14,3)] = CRGB(255, 0, 0);
  leds[ XY(14,4)] = CRGB(255, 0, 0);
  leds[ XY(14,5)] = CRGB(255, 0, 0);
  leds[ XY(14,6)] = CRGB(255, 0, 0);
  leds[ XY(13,2)] = CRGB(255, 0, 0);
  leds[ XY(13,4)] = CRGB(255, 0, 0);
  leds[ XY(12,2)] = CRGB(255, 0, 0);
  leds[ XY(12,4)] = CRGB(255, 0, 0);
  // U
  leds[ XY(11,4)] = CRGB(0, 0, 255);
  leds[ XY(11,5)] = CRGB(0, 0, 255);
  leds[ XY(10,6)] = CRGB(0, 0, 255);
  leds[ XY(9,6)] = CRGB(0, 0, 255);
  leds[ XY(8,6)] = CRGB(0, 0, 255);
  leds[ XY(8,5)] = CRGB(0, 0, 255);
  leds[ XY(8,4)] = CRGB(0, 0, 255);
  // C
  leds[ XY(7,4)] = CRGB(255, 0, 0);
  leds[ XY(7,5)] = CRGB(255, 0, 0);
  leds[ XY(6,3)] = CRGB(255, 0, 0);
  leds[ XY(5,3)] = CRGB(255, 0, 0);
  leds[ XY(6,6)] = CRGB(255, 0, 0);
  leds[ XY(5,6)] = CRGB(255, 0, 0);
  // K
  leds[ XY(4,2)] = CRGB(0, 0, 255);
  leds[ XY(4,3)] = CRGB(0, 0, 255);
  leds[ XY(4,4)] = CRGB(0, 0, 255);
  leds[ XY(4,5)] = CRGB(0, 0, 255);
  leds[ XY(4,6)] = CRGB(0, 0, 255);
  leds[ XY(3,5)] = CRGB(0, 0, 255);
  leds[ XY(2,5)] = CRGB(0, 0, 255);
  leds[ XY(2,4)] = CRGB(0, 0, 255);
  leds[ XY(1,3)] = CRGB(0, 0, 255);
  leds[ XY(1,6)] = CRGB(0, 0, 255);

  // Y
  leds[ XY(14,8)] = CRGB(255, 0, 0);
  leds[ XY(14,9)] = CRGB(255, 0, 0);
  leds[ XY(13,10)] = CRGB(255, 0, 0);
  leds[ XY(12,11)] = CRGB(255, 0, 0);
  leds[ XY(12,12)] = CRGB(255, 0, 0);
  leds[ XY(12,13)] = CRGB(255, 0, 0);
  leds[ XY(11,10)] = CRGB(255, 0, 0);
  leds[ XY(10,9)] = CRGB(255, 0, 0);
  leds[ XY(10,8)] = CRGB(255, 0, 0);
  // O
  leds[ XY(8,9)] = CRGB(0, 0, 255);
  leds[ XY(7,9)] = CRGB(0, 0, 255);
  leds[ XY(9,10)] = CRGB(0, 0, 255);
  leds[ XY(9,11)] = CRGB(0, 0, 255);
  leds[ XY(9,12)] = CRGB(0, 0, 255);
  leds[ XY(8,13)] = CRGB(0, 0, 255);
  leds[ XY(7,13)] = CRGB(0, 0, 255);
  leds[ XY(6,12)] = CRGB(0, 0, 255);
  leds[ XY(6,11)] = CRGB(0, 0, 255);
  leds[ XY(6,10)] = CRGB(0, 0, 255);
  // U
  leds[ XY(5,11)] = CRGB(255, 0, 0);
  leds[ XY(5,12)] = CRGB(255, 0, 0);
  leds[ XY(4,13)] = CRGB(255, 0, 0);
  leds[ XY(3,13)] = CRGB(255, 0, 0);
  leds[ XY(2,13)] = CRGB(255, 0, 0);
  leds[ XY(2,12)] = CRGB(255, 0, 0);
  leds[ XY(2,11)] = CRGB(255, 0, 0);
  FastLED.show();
  delay(800);
  
  for(int x = 0; x < 16; x++){
    for(int y = 0; y < 16; y++){
      leds[ XY(x,y)] = CRGB( 0, 0, 0);
    }
  }

  // Fade
  leds[ XY(14,2)] = CRGB(255, 0, 0);
  leds[ XY(14,3)] = CRGB(255, 0, 0);
  leds[ XY(14,5)] = CRGB(255, 0, 0);
  leds[ XY(14,6)] = CRGB(255, 0, 0);
  leds[ XY(12,2)] = CRGB(255, 0, 0);
  leds[ XY(12,4)] = CRGB(255, 0, 0);
  leds[ XY(11,4)] = CRGB(0, 0, 255);
  leds[ XY(7,5)] = CRGB(0, 0, 255);
  leds[ XY(7,4)] = CRGB(255, 0, 0);
  leds[ XY(7,5)] = CRGB(0, 0, 255);
  leds[ XY(6,3)] = CRGB(255, 0, 0);
  leds[ XY(5,3)] = CRGB(255, 0, 0);
  leds[ XY(6,6)] = CRGB(255, 0, 0);
  leds[ XY(5,6)] = CRGB(255, 0, 0);
  leds[ XY(4,2)] = CRGB(0, 0, 255);
  leds[ XY(4,4)] = CRGB(0, 0, 255);
  leds[ XY(4,5)] = CRGB(0, 0, 255);
  leds[ XY(4,6)] = CRGB(0, 0, 255);
  leds[ XY(3,5)] = CRGB(0, 0, 255);
  leds[ XY(2,4)] = CRGB(0, 0, 255);
  leds[ XY(1,6)] = CRGB(0, 0, 255);
  leds[ XY(14,8)] = CRGB(255, 0, 0);
  leds[ XY(13,10)] = CRGB(255, 0, 0);
  leds[ XY(12,11)] = CRGB(255, 0, 0);
  leds[ XY(12,13)] = CRGB(255, 0, 0);
  leds[ XY(11,10)] = CRGB(255, 0, 0);
  leds[ XY(10,8)] = CRGB(255, 0, 0);
  leds[ XY(8,9)] = CRGB(0, 0, 255);
  leds[ XY(7,9)] = CRGB(0, 0, 255);
  leds[ XY(9,10)] = CRGB(0, 0, 255);
  leds[ XY(8,13)] = CRGB(0, 0, 255);
  leds[ XY(7,13)] = CRGB(0, 0, 255);
  leds[ XY(6,11)] = CRGB(0, 0, 255);
  leds[ XY(6,10)] = CRGB(0, 0, 255);
  leds[ XY(5,11)] = CRGB(255, 0, 0);
  leds[ XY(3,13)] = CRGB(255, 0, 0);
  leds[ XY(2,13)] = CRGB(255, 0, 0);
  leds[ XY(2,11)] = CRGB(255, 0, 0);
  FastLED.show();
  delay(400);

    for(int x = 0; x < 16; x++){
    for(int y = 0; y < 16; y++){
      leds[ XY(x,y)] = CRGB( 0, 0, 0);
    }
  }

   // Fade
  leds[ XY(14,2)] = CRGB(255, 0, 0);
  leds[ XY(14,3)] = CRGB(255, 0, 0);
  leds[ XY(14,6)] = CRGB(255, 0, 0);
  leds[ XY(12,2)] = CRGB(255, 0, 0);
  leds[ XY(11,4)] = CRGB(0, 0, 255);
  leds[ XY(7,4)] = CRGB(255, 0, 0);
  leds[ XY(7,5)] = CRGB(0, 0, 255);
  leds[ XY(5,3)] = CRGB(255, 0, 0);
  leds[ XY(4,2)] = CRGB(0, 0, 255);
  leds[ XY(4,4)] = CRGB(0, 0, 255);
  leds[ XY(4,5)] = CRGB(0, 0, 255);
  leds[ XY(3,5)] = CRGB(0, 0, 255);
  leds[ XY(2,4)] = CRGB(0, 0, 255);
  leds[ XY(1,6)] = CRGB(0, 0, 255);
  leds[ XY(13,10)] = CRGB(255, 0, 0);
  leds[ XY(12,11)] = CRGB(255, 0, 0);
  leds[ XY(12,13)] = CRGB(255, 0, 0);
  leds[ XY(11,10)] = CRGB(255, 0, 0);
  leds[ XY(8,9)] = CRGB(0, 0, 255);
  leds[ XY(7,9)] = CRGB(0, 0, 255);
  leds[ XY(9,10)] = CRGB(0, 0, 255);
  leds[ XY(7,13)] = CRGB(0, 0, 255);
  leds[ XY(6,11)] = CRGB(0, 0, 255);
  leds[ XY(6,10)] = CRGB(0, 0, 255);
  leds[ XY(2,11)] = CRGB(255, 0, 0);
  FastLED.show();
  delay(300);

    for(int x = 0; x < 16; x++){
    for(int y = 0; y < 16; y++){
      leds[ XY(x,y)] = CRGB( 0, 0, 0);
    }
  }

     // Fade
  leds[ XY(14,2)] = CRGB(255, 0, 0);
  leds[ XY(14,6)] = CRGB(255, 0, 0);
  leds[ XY(11,4)] = CRGB(0, 0, 255);
  leds[ XY(7,4)] = CRGB(255, 0, 0);
  leds[ XY(5,3)] = CRGB(255, 0, 0);
  leds[ XY(4,2)] = CRGB(0, 0, 255);
  leds[ XY(4,5)] = CRGB(0, 0, 255);
  leds[ XY(3,5)] = CRGB(0, 0, 255);
  leds[ XY(13,10)] = CRGB(255, 0, 0);
  leds[ XY(12,11)] = CRGB(255, 0, 0);
  leds[ XY(11,10)] = CRGB(255, 0, 0);
  leds[ XY(8,9)] = CRGB(0, 0, 255);
  leds[ XY(7,9)] = CRGB(0, 0, 255);
  leds[ XY(6,11)] = CRGB(0, 0, 255);
  leds[ XY(2,11)] = CRGB(255, 0, 0);
  FastLED.show();
  delay(200);

    for(int x = 0; x < 16; x++){
    for(int y = 0; y < 16; y++){
      leds[ XY(x,y)] = CRGB( 0, 0, 0);
    }
  }

  // Fade
  leds[ XY(7,4)] = CRGB(255, 0, 0);
  leds[ XY(5,3)] = CRGB(255, 0, 0);
  leds[ XY(4,5)] = CRGB(0, 0, 255);
  leds[ XY(3,5)] = CRGB(0, 0, 255);
  leds[ XY(13,10)] = CRGB(255, 0, 0);
  leds[ XY(12,11)] = CRGB(255, 0, 0);
  leds[ XY(8,9)] = CRGB(0, 0, 255);
  leds[ XY(7,9)] = CRGB(0, 0, 255);
  leds[ XY(2,11)] = CRGB(255, 0, 0);
  FastLED.show();
  delay(100);

    for(int x = 0; x < 16; x++){
    for(int y = 0; y < 16; y++){
      leds[ XY(x,y)] = CRGB( 0, 0, 0);
    }
  }

  // Fade
  leds[ XY(7,4)] = CRGB(255, 0, 0);
  leds[ XY(4,5)] = CRGB(0, 0, 255);
  leds[ XY(13,10)] = CRGB(255, 0, 0);
  leds[ XY(2,11)] = CRGB(255, 0, 0);
  FastLED.show();
  delay(50);

    for(int x = 0; x < 16; x++){
    for(int y = 0; y < 16; y++){
      leds[ XY(x,y)] = CRGB( 0, 0, 0);
    }
  }
  
}



void drawSquare2(uint8_t x, uint8_t y) {
  leds[ XY( (2 * x) - 2, (2 * y)-2)] = CHSV( 0, 0, 255);
  leds[ XY( (2 * x) - 2, (2 * y)-1)]   = CHSV( 0, 0, 255);
  leds[ XY( (2 * x) - 1, (2 * y)-2)] = CHSV( 0, 0, 255);
  leds[ XY( (2 * x) - 1, (2 * y)-1)] = CHSV( 0, 0, 255);
}
void drawSquare(uint8_t x, uint8_t y) {
  leds[ XY( (2 * x) - 2, (2 * y) - 1)] = CHSV( 0, 0, 255);
  leds[ XY( (2 * x) - 2, (2 * y))]   = CHSV( 0, 0, 255);
  leds[ XY( (2 * x) - 1, (2 * y) - 1)] = CHSV( 0, 0, 255);
  leds[ XY( (2 * x) - 1, (2 * y))] = CHSV( 0, 0, 255);
}

void drawBigZero() {
  drawSquare(3, 1);
  drawSquare(4, 1);
  drawSquare(5, 1);
  drawSquare(6, 1);
  drawSquare(2, 2);
  drawSquare(3, 2);
  drawSquare(6, 2);
  drawSquare(7, 2);
  drawSquare(2, 3);
  drawSquare(3, 3);
  drawSquare(6, 3);
  drawSquare(7, 3);
  drawSquare(2, 4);
  drawSquare(3, 4);
  drawSquare(6, 4);
  drawSquare(7, 4);
  drawSquare(2, 5);
  drawSquare(3, 5);
  drawSquare(6, 5);
  drawSquare(7, 5);
  drawSquare(2, 6);
  drawSquare(3, 6);
  drawSquare(6, 6);
  drawSquare(7, 6);
  drawSquare(3, 7);
  drawSquare(4, 7);
  drawSquare(5, 7);
  drawSquare(6, 7);
}

void drawBigOne() {
  drawSquare(4, 1);
  drawSquare(5, 1);
  drawSquare(4, 2);
  drawSquare(5, 2);
  drawSquare(6, 2);
  drawSquare(4, 3);
  drawSquare(5, 3);
  drawSquare(4, 4);
  drawSquare(5, 4);
  drawSquare(4, 5);
  drawSquare(5, 5);
  drawSquare(4, 6);
  drawSquare(5, 6);
  drawSquare(4, 7);
  drawSquare(5, 7);
}

void drawBigTwo() {
  drawSquare(6, 1);
  drawSquare(5, 1);
  drawSquare(4, 1);
  drawSquare(3, 1);
  drawSquare(7, 2);
  drawSquare(6, 2);
  drawSquare(3, 2);
  drawSquare(2, 2);
  drawSquare(3, 3);
  drawSquare(2, 3);
  drawSquare(5, 4);
  drawSquare(4, 4);
  drawSquare(3, 4);
  drawSquare(6, 5);
  drawSquare(5, 5);
  drawSquare(7, 6);
  drawSquare(6, 6);
  drawSquare(7, 7);
  drawSquare(6, 7);
  drawSquare(5, 7);
  drawSquare(4, 7);
  drawSquare(3, 7);
  drawSquare(2, 7);
}

void drawBigThree() {
  drawSquare(6, 1);
  drawSquare(5, 1);
  drawSquare(4, 1);
  drawSquare(3, 1);
  drawSquare(7, 2);
  drawSquare(6, 2);
  drawSquare(3, 2);
  drawSquare(2, 2);
  drawSquare(3, 3);
  drawSquare(2, 3);
  drawSquare(5, 4);
  drawSquare(4, 4);
  drawSquare(3, 4);
  drawSquare(3, 5);
  drawSquare(2, 5);
  drawSquare(3, 6);
  drawSquare(2, 6);
  drawSquare(7, 6);
  drawSquare(6, 6);
  drawSquare(6, 7);
  drawSquare(5, 7);
  drawSquare(4, 7);
  drawSquare(3, 7);
}


// Small Numbers

void drawPixel(uint8_t x, uint8_t y, uint8_t pos) {
  int8_t offsetX = 0;
  int8_t offsetY = 0;

  if (pos == 1) {     // Top Left
    offsetX = 7;
    offsetY = 0;
  }
  else if (pos == 2) { // Top Right
    offsetX = -1;
    offsetY = 0;
  }
  else if (pos == 3) { // Bottom Left
    offsetX = 7;
    offsetY = 8;
  }
  else if (pos == 4) { // Bottom Right
    offsetX = -1;
    offsetY = 8;
  }
  else if (pos == 5) { // Mid Left
    offsetX = 7;
    offsetY = 4;
  }
  else if (pos == 6) { // Mid Right
    offsetX = -1;
    offsetY = 4;
  }

  leds[ XY( x + offsetX, y + offsetY)] = CHSV( 0, 0, 255);
}

void drawSmallZero(uint8_t pos) {
  drawPixel(3, 1, pos);
  drawPixel(4, 1, pos);
  drawPixel(5, 1, pos);
  drawPixel(6, 1, pos);
  drawPixel(2, 2, pos);
  drawPixel(3, 2, pos);
  drawPixel(6, 2, pos);
  drawPixel(7, 2, pos);
  drawPixel(2, 3, pos);
  drawPixel(3, 3, pos);
  drawPixel(6, 3, pos);
  drawPixel(7, 3, pos);
  drawPixel(2, 4, pos);
  drawPixel(3, 4, pos);
  drawPixel(6, 4, pos);
  drawPixel(7, 4, pos);
  drawPixel(2, 5, pos);
  drawPixel(3, 5, pos);
  drawPixel(6, 5, pos);
  drawPixel(7, 5, pos);
  drawPixel(2, 6, pos);
  drawPixel(3, 6, pos);
  drawPixel(6, 6, pos);
  drawPixel(7, 6, pos);
  drawPixel(3, 7, pos);
  drawPixel(4, 7, pos);
  drawPixel(5, 7, pos);
  drawPixel(6, 7, pos);
}


#define BRIGHTNESS_BACKGROUND 45
#define BRIGHTNESS_FOREGROUND 100


// Convert Grind X, Y to pixel ID
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if ( y & 0x01) {
    // Odd rows run backwards
    uint8_t reverseX = (kMatrixWidth - 1) - x;
    i = (y * kMatrixWidth) + reverseX;
  } else {
    // Even rows run forwards
    i = (y * kMatrixWidth) + x;
  }
  return i;
}

void createFrame(uint32_t ms, bool foreground) {

  uint8_t brightnessLevel = 0;
  if (foreground)
    brightnessLevel = BRIGHTNESS_FOREGROUND;
  else
    brightnessLevel = BRIGHTNESS_BACKGROUND;

  int32_t yHueDelta32 = ((int32_t)cos16( ms * (27 / 1) ) * (350 / kMatrixWidth));
  int32_t xHueDelta32 = ((int32_t)cos16( ms * (39 / 1) ) * (310 / kMatrixHeight));
  DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768, brightnessLevel);
  if ( ms < 500 ) {
    FastLED.setBrightness( scale8( 100, (ms * 256) / 5000));
  } else {
    FastLED.setBrightness(100);
  }
}


// Draw one frame of the animation
void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8, int8_t brightness)
{
  byte lineStartHue = startHue8;
  for ( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;
    for ( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue, 255, brightness);
    }
  }
}
