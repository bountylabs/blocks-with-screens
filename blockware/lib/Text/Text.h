#include <Colors.h>
#include <Adafruit_SSD1351.h>

extern Adafruit_SSD1351 tft;

#define outputf(color, size, msg, ...) do {tft.setTextSize(size); tft.setTextColor(color); tft.printf(msg, ##__VA_ARGS__);} while (0)

void output(const char* message, int color = WHITE, int size = 1);
void outputln(const char* message, int color = WHITE, int size = 1);
void displayReset();
