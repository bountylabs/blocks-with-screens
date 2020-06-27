#include <Adafruit_NeoPixel.h>

// Pin setup
#define PIN 4 // D2

Adafruit_NeoPixel pixels(1, PIN, NEO_GRB);

void setup(void) {
  pixels.begin();
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'
  // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
  // Here we're using a moderately bright green color:
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(500);
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(500);
  pixels.setPixelColor(0, pixels.Color(0, 0, 255));
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(500);
  pixels.clear(); // Set all pixel colors to 'off'
  delay(500);
}
