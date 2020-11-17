/***************************************************
  Example sketch for the Block with Screen
 ****************************************************/

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <stdlib.h>
#include "90fps.h"
#include <DefaultConfig.h>
#include <Colors.h>

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// in-memory 16-bit canvas
GFXcanvas16 *canvas;

uint16_t lastLoop;

void flush()
{
  tft.drawRGBBitmap(0, 0, (uint16_t *)canvas->getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  //tft.startWrite();
  // SPI.writeBytes((uint8_t *)canvas->getBuffer(), 128 * 128 * 2);
  //tft.endWrite();
}

void setup(void)
{
  Serial.begin(115200);
  Serial.print("setup");
  // 24MHz SPI
  // For a 128x128 pixel screen with 16-bit color depth this gives us
  // 24000000 /(128*128*16) = 91.552734375 Hz (or 91.55 FPS max)
  tft.begin(24000000);

  // initialize canvas
  canvas = new GFXcanvas16(SCREEN_WIDTH, SCREEN_HEIGHT);

  // reset canvas to all black
  canvas->fillScreen(BLACK);
  flush();

  lastLoop = millis();
}

void loop()
{

  uint16_t now = millis();
  uint16_t time = now - lastLoop;

  // every ~16ms (60fps)
  if (time > 16)
  {
    // track loop millis to keep steady fps
    lastLoop = now;

    // run!
    run90FPS(canvas);

    // flush our in-memory canvas to the screen
    flush();
  }
  // Serial.printf("frame %dms\n", time);

  // // run!
  // run90FPS(canvas);

  // // flush our in-memory canvas to the screen
  // flush();
}
