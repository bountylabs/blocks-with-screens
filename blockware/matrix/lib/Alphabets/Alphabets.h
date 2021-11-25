#pragma once

#include <Adafruit_SSD1351.h>

#ifndef PROGMEM
#define PROGMEM
#endif

#include <Vec2d.h>

class Alphabets {
private:
protected:
public:
  static const char* alphabet[] PROGMEM;
  static const char* hiragana[] PROGMEM;
  static const int alphabet_length;
  static const int hiragana_length;
  static const Vec2d<uint8_t> alphabet_textSize;
  static const Vec2d<uint8_t> hiragana_textSize;
};


