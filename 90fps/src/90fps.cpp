#include "90fps.h"
#include <Adafruit_SSD1351.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<time.h>

// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

const int COLORS_LEN = 7;
const int COLORS[COLORS_LEN] = {BLUE,RED,GREEN,CYAN,MAGENTA,YELLOW,WHITE};

extern Adafruit_SSD1351 tft;


float random() {
  // Use current time as seed for random generator
  return (float)rand()/(float)RAND_MAX;
}

void run90FPS() {
  // 1% chance to reset to black
  if (0.01 > random()) {
   tft.fillScreen(BLACK);
  }

  // random x, y and color
  int x = floor(tft.width() * random());
  int y = floor(tft.height() * random());
  int colorIndex = floor(COLORS_LEN * random());
  int color = COLORS[colorIndex];

  // a single random pixel
  // Serial.printf("[%d,%d] %d", x, y, color);
  tft.drawPixel(x, y, color);
}
