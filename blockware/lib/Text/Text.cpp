#include <DLog.h>
#include <Adafruit_SSD1351.h>
#include <Colors.h>

extern Adafruit_SSD1351 tft;

void outputf(int color, int size, const char *message, ...) {
    va_list args;
    va_start(args, message);
    tft.printf(message, args);
    va_end(args);
}

void output(const char* message, int color = WHITE, int size = 1)
{
  DLOG(message);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.print(message);
}

void outputln(const char* message, int color = WHITE, int size = 1)
{
  DLOG(message);
  DLOG("\n");
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.println(message);
}

void displayReset() {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
}
