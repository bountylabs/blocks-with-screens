#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

String getAlphaVAPIKeyUrl();

String getAlphaVUrlForTimeSeriesDaily(String symbol);

void drawStock(
  Adafruit_SSD1351 screen,
  char *ticker,
  double amount,
  double change,
  uint8_t textSize,
  uint8_t drawablePadding,
  uint8_t textVerticalPadding);
