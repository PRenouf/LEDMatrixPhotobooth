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
  if (command == 0) {
    //Toggle the state of the LED
    if (currentMillis > previousMillis + 500) {
      digitalWrite(IndicatorLED, !digitalRead(IndicatorLED));
      previousMillis = currentMillis;
    }
  }
  else if (command == 1) {
    takePhoto();
  }

  server.send(200, "text/html", "Data received");
}

void takePhoto() {
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
  //WiFi.softAP(ssid, password);
 // IPAddress myIP = WiFi.softAPIP();

  pinMode(IndicatorLED, OUTPUT);
  pinMode(TriggerLED, OUTPUT);
  pinMode(shutter, OUTPUT);
  pinMode(focus, OUTPUT);
  pinMode(13, INPUT_PULLUP);

  digitalWrite(shutter, LOW);
  digitalWrite(focus, LOW);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  //FastLED.setBrightness( BRIGHTNESS );

  uint32_t ST = millis();
  uint32_t ms = 0;
  while (ms < ST + 5000) {
    ms = millis();
    createFrame(ms, true);
    drawSmallZero(5);
    drawSmallZero(6);
    FastLED.show();
    delay(10);
  }

  //server.on("/data/", HTTP_GET, handleSentVar); // when the server receives a request with /data/ in the string then run the handleSentVar function
  //server.begin();

  Serial.println("Setup Complete.");
}

void loop() {
  //server.handleClient();
  
  currentLEDUpdate = millis();
  if(currentLEDUpdate > prevLEDUpdate + 500){

    uint32_t ms = millis();
    createFrame(ms, true);

    FastLED.show();
    delay(10);
  }
  
  takePhoto();
  if (digitalRead(13) == LOW) {
    takePhoto();
  }
}


///

#define FONT_NUMER_STYLE_STND_

unsigned long startTime = 0;
unsigned long currentTime = 0;


void flash() {
  for (int x = 1; x < 9; x++) {
    for (int y = 1; y < 9; y++) {
      drawSquare(x, y);
    }
  }
  FastLED.show();
  delay(10);
}
  
void takePhoto(){
  startTime = millis();

  while(currentTime < startTime + 1000){
    createFrame(ms, false);
    drawBigThree();
        FastLED.show();
    delay(10);
    currentTime = millis();
  }

  while(currentTime < startTime + 2000){
    createFrame(ms, false);
    drawBigTwo();
        FastLED.show();
    delay(10);
    currentTime = millis();
  }

  while(currentTime < startTime + 3000){
    createFrame(ms, false);
    drawBigOne();
        FastLED.show();
    delay(10);
    currentTime = millis();
  }

  flash();
}



#ifdef FONT_NUMER_STYLE_STND
// Function to convert letter coordinates into 4x4 squares on matrix
void drawSquare(uint8_t x, uint8_t y) {
  leds[ XY( (2 * x) - 1, (2 * y) - 1)] = CHSV( 0, 0, 255);
  leds[ XY( (2 * x) - 1, (2 * y))]   = CHSV( 0, 0, 255);
  leds[ XY( (2 * x)   , (2 * y) - 1)] = CHSV( 0, 0, 255);
  leds[ XY( (2 * x)   , (2 * y))] = CHSV( 0, 0, 255);
}

void drawBigZero() {
  Serial.println("Showing 0...");
  drawSquare(3, 1);
  drawSquare(4, 1);
  drawSquare(5, 1);
  drawSquare(2, 2);
  drawSquare(6, 2);
  drawSquare(2, 3);
  drawSquare(6, 3);
  drawSquare(2, 4);
  drawSquare(6, 4);
  drawSquare(2, 5);
  drawSquare(6, 5);
  drawSquare(2, 6);
  drawSquare(6, 6);
  drawSquare(3, 7);
  drawSquare(4, 7);
  drawSquare(5, 7);
}

void drawBigOne() {
  Serial.println("Showing 1...");
  drawSquare(4, 1);
  drawSquare(6, 2);
  drawSquare(5, 2);
  drawSquare(4, 2);
  drawSquare(4, 3);
  drawSquare(4, 4);
  drawSquare(4, 5);
  drawSquare(4, 6);
  drawSquare(2, 7);
  drawSquare(3, 7);
  drawSquare(4, 7);
  drawSquare(5, 7);
  drawSquare(6, 7);
}

void drawBigTwo() {
  Serial.println("Showing 2...");
  drawSquare(5, 1);
  drawSquare(4, 1);
  drawSquare(3, 1);
  drawSquare(6, 2);
  drawSquare(2, 2);
  drawSquare(3, 3);
  drawSquare(4, 4);
  drawSquare(5, 5);
  drawSquare(6, 6);
  drawSquare(6, 7);
  drawSquare(5, 7);
  drawSquare(4, 7);
  drawSquare(3, 7);
  drawSquare(2, 7);
}

void drawBigThree() {
  Serial.println("Showing 3...");
  drawSquare(5, 1);
  drawSquare(4, 1);
  drawSquare(3, 1);
  drawSquare(6, 2);
  drawSquare(2, 2);
  drawSquare(2, 3);
  drawSquare(4, 4);
  drawSquare(3, 4);
  drawSquare(2, 5);
  drawSquare(2, 6);
  drawSquare(6, 6);
  drawSquare(5, 7);
  drawSquare(4, 7);
  drawSquare(3, 7);
}

///////////////////////////////////////////////////////////////////////////////////////
#else
// Function to convert letter coordinates into 4x4 squares on matrix
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

#endif

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