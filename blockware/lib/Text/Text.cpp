#include "Text.h"
#include <DLog.h>
#include <Colors.h>
#include <Adafruit_SSD1351.h>

void output(const char* message, int color, int size)
{
  DLOG(message);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.print(message);
}

void outputln(const char* message, int color, int size)
{
  DLOG(message);
  DLOG("\n");
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.println((char *)message);
}

void displayReset() {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
}
