#include <Colors.h>
#include <DefaultConfig.h>

#include "Canvas.h"

Adafruit_SSD1351 Canvas::tft =
  Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

GFXcanvas16* Canvas::canvas = new GFXcanvas16(Canvas::tft.width(), Canvas::tft.height());

void Canvas::setup()
{
  tft.begin(SPI_SPEED);

  // initialize screen to black
  tft.fillScreen(BLACK);

  // initialize canvas to all black
  canvas->fillScreen(BLACK);
}

void Canvas::flush()
{
  // write canvas to screen
  tft.drawRGBBitmap(0, 0, (uint16_t*)canvas->getBuffer(), canvas->width(), canvas->height());
}
