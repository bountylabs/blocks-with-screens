#include <Adafruit_SSD1351.h>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "CurvySnake.h"
#include <Colors.h>
#include <Entity.h>
#include <Random.h>

extern Adafruit_SSD1351 tft;

// in-memory 16-bit canvas
GFXcanvas16* canvas;
int screenWidth = -1;
int screenHeight = -1;

// used by CurvySnake_loop for tick frequency
const int FPS = floor(1000 / 60); // every ~16ms (60fps)
// track last run time in ms for CurvySnake_loop
uint16_t lastLoop = millis() - FPS + 1;

// state
std::vector<Entity> entities;

void flush()
{
  tft.drawRGBBitmap(0, 0, (uint16_t*)canvas->getBuffer(), screenWidth, screenHeight);
  // tft.startWrite();
  // SPI.writeBytes((uint8_t *)canvas->getBuffer(), 128 * 128 * 2);
  // tft.endWrite();
}

void tick()
{
  // reset screen to black
  canvas->fillScreen(BLACK);
  // canvas->drawPixel(pos.x, pos.y, BLACK);

  // for all entities
  for (std::size_t i = 0, max = entities.size(); i != max; i++) {
    Entity* e = &entities[i];
    e->tick();
    // e->debug();
    e->draw(canvas);
  }
}

void CurvySnake_setup()
{
  // save dimensions
  screenWidth = tft.width();
  screenHeight = tft.height();

  // initialize canvas to all black
  canvas = new GFXcanvas16(screenWidth, screenHeight);
  canvas->fillScreen(BLACK);
  flush();

  // create a single entity
  entities.push_back(Entity(screenWidth, screenHeight));

  Serial.printf("\nFPS=%d", FPS);
  Serial.printf("\nEntities[%d]", entities.size());
  Serial.printf("\n");

  // Serial.printf("\n%d -- %d\n", RGB565(255, 0, 0), RED);
  // Serial.printf("\n%d -- %d\n", RGB565(0, 255, 0), GREEN);
  // Serial.printf("\n%d -- %d\n", RGB565(0, 0, 255), BLUE);
}

void CurvySnake_loop()
{
  uint16_t now = millis();
  uint16_t time = now - lastLoop;

  if (time > FPS) {
    // track loop millis to keep steady fps
    lastLoop = now;

    // run!
    tick();

    // flush our in-memory canvas to the screen
    flush();
  }
}
