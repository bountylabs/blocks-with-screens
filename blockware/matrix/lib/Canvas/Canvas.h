#pragma once

#include <Adafruit_SSD1351.h>

class Canvas {
private:
protected:
public:
  // reference to the in-memory 16-bit drawing canvas, used by draw functions
  static GFXcanvas16* canvas;
  static Adafruit_SSD1351 tft;
  static void setup();
  static void flush();
};


