#include "stocks.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#include "constants.h"
#include "secrets.h"

String getAlphaVAPIKeyUrl() {
    return String(ALPHA_APIKEY) + String(ALPHA_VANTAGE_API_KEY);
}

String getAlphaVUrlForTimeSeriesDaily(String symbol) {
    return String(STOCKS_PATH) + symbol;
}

void drawStock(
  Adafruit_SSD1351 screen,
  char *ticker,
  double amount,
  double change,
  uint8_t textSize,
  uint8_t drawablePadding,
  uint8_t textVerticalPadding) {

  screen.setTextSize(textSize);
  
  int16_t ignoredTickerX;
  int16_t ignoredTickerY;
  uint16_t tickerWidth;
  uint16_t tickerHeight;
  screen.getTextBounds(
    ticker,
    0,
    0,
    &ignoredTickerX,
    &ignoredTickerY,
    &tickerWidth,
    &tickerHeight);

  char delta[128];

  sprintf(delta, "%.2f", amount);
  
  int16_t ignoredDeltaX;
  int16_t ignoredDeltaY;
  uint16_t deltaWidth;
  uint16_t deltaHeight;
  screen.getTextBounds(
    delta,
    0,
    0,
    &ignoredDeltaX,
    &ignoredDeltaY,
    &deltaWidth,
    &deltaHeight);

  uint8_t drawableWidth = (uint8_t) ((tickerHeight / 2) * 1.732);
  uint8_t tickerAndDeltaWidth = max(tickerWidth, deltaWidth);

  uint8_t contentWidth = drawablePadding + drawableWidth + tickerAndDeltaWidth;
  uint8_t contentHeight = 2 * tickerHeight + textVerticalPadding;

  uint8_t tickerY = (screen.height() - contentHeight) / 2;
  uint8_t drawableX = (screen.width() - contentWidth) / 2;
  uint8_t tickerX = drawableX + drawableWidth + drawablePadding;
  uint8_t drawableY = tickerY + tickerHeight + textVerticalPadding;
  uint8_t deltaX = tickerX;
  uint8_t deltaY = drawableY;

  int color = change == 0 ? WHITE : (change > 0 ? GREEN : RED);

  if (change > 0) {
    screen.fillTriangle(
      drawableX + (drawableWidth / 2),
      drawableY,
      drawableX,
      drawableY + tickerHeight,
      drawableX + drawableWidth,
      drawableY + tickerHeight,
      color);
  } else if (change < 0) {
    screen.fillTriangle(
      drawableX,
      drawableY,
      drawableX + drawableWidth,
      drawableY,
      drawableX + (drawableWidth / 2),
      drawableY + tickerHeight,
      color);
  }

  screen.setTextColor(WHITE);
  screen.setCursor(tickerX, tickerY);
  screen.print(ticker);

  screen.setTextColor(color);
  screen.setCursor(deltaX, deltaY);
  screen.print(delta);
}