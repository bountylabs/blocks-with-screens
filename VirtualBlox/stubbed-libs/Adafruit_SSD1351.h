#ifndef _ADAFRUIT_SSD1351_H_
#define _ADAFRUIT_SSD1351_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "Adafruit_GFX.h"

#define DEG_TO_RAD 0.01745329252
extern void* SPI;

class FakeSerial {
public:
    void begin(int rate);
    int printf(const char *format, ...);

    int pp2(const char* format, ...);
    int print(const char* str);
    int println();
    int println(const char* str);
    int write(uint8_t* data, int bytes);
};

extern FakeSerial Serial;

class FakeESP {
public:
    uint32_t getCycleCount()
    {
        return 0;
    }
};

extern FakeESP ESP;

void millis_start();
uint16_t millis();
void randomSeed(uint32_t seed);
void displayReset();
int open_udp();

// Adafruit_SSD1351 for VirtualBlox simulator
class Adafruit_SSD1351 {
public:
    Adafruit_SSD1351(uint16_t width, uint16_t height,
      void *spi, int8_t cs_pin, int8_t dc_pin, int8_t rst_pin = -1);

    void begin(uint32_t freq = 0);
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap,
      int16_t w, int16_t h);
    void setTextColor(int16_t color);
    void println(char* str);
    void print(const char* str);
    int printf(const char *format, ...);
    int width();
    int height();
    void startWrite();
    void endWrite();
    void setAddrWindow(int x, int y, int w, int h);
    void writePixels(uint16_t* pixels, int num_pixels);
    void fillScreen(uint16_t color);
    void setCursor(int x, int y);
    void setTextWrap(bool wrap);
    void setTextSize(int size);
private:
    int _width;
    int _height;
};


#endif