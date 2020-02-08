#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 256
#define DATA_PIN 5

// Define the array of leds
CRGB leds[NUM_LEDS];

int numIndex0[] = {26, 25, 24, 23, 22, 21, 37, 38, 39, 40, 41, 42, 60, 59, 52, 51, 67, 68, 75, 76, 92, 91, 84, 83, 99, 100, 107, 108, 124, 123, 116, 115, 131, 132, 139, 140, 156, 155, 148, 147, 163, 164, 171, 172, 188, 187, 180, 179, 195, 196, 203, 204, 218, 217, 216, 215, 214, 213, 229, 230, 231, 232, 233, 234};
int numIndex1[] = {23, 24, 40, 39, 51, 52, 53, 54, 55, 56, 76, 75, 74, 73, 72, 71, 87, 88, 104, 103, 119, 120, 136, 135, 151, 152, 168, 167, 183, 184, 200, 199, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 236, 235, 234, 233, 232, 231, 230, 29, 228, 227};



void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); // WS2812B

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }
  delay(750);


  for (int i = 0; i < sizeof(numIndex1); i++) {
    leds[numIndex1[i]] = CRGB::Blue;
    FastLED.show();
  }
  delay(1000);


  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    delay(1);
    FastLED.show();
  }
  for (int i = 0; i < sizeof(numIndex0); i++) {
    leds[numIndex0[i]] = CRGB::Yellow;
    FastLED.show();
    delay(1);
  }
  delay(2000);
}

void loop() {
  // Turn the LED on, then pause
  for (int i = 0; i < 256; i++) {
    leds[i] = CRGB::Yellow;
    FastLED.show();
    delay(10);
    // Now turn the LED off, then pause
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(10);
  }
}
