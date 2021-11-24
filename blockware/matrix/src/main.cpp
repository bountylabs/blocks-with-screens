// set SERIALLOG to 1 to enable serial logging
// set TELNETLOG to 1 to enable telnet logging
// see DLog.h
#define DEBUG 0
#define SERIALLOG 1
#include <DLog.h>

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <math.h>

#include <Colors.h>
#include <DefaultConfig.h>
#include <Random.h>
#include <Vec2d.h>

#include "DebugAlphabet.h"
#include "Touch.h"
#include "MatrixRain.h"

Adafruit_SSD1351 tft =
  Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// in-memory 16-bit canvas
GFXcanvas16* canvas;

const int FPS = floor(1000.0f / 30.0f); // (30fps)
uint16_t lastLoop = millis() - FPS + 1;

// screen dimensions
Vec2d<int> screen = Vec2d<int>();

// Touch instance
// Can detect taps on all 3 cartesian axes
Touch* touch;

// MatrixRain vector for columns
std::vector<MatrixRain> rain_vec;

void tick()
{
  // tick Touch instance
  touch->tick();
  // detect tap on y axis
  if (touch->isTap(Touch::Y_AXIS)) {
    DLOG("\n[Touch] tap! (toggling active on the MatrixRain)");
    // toggle active on the MatrixRain instances
    for (std::size_t i = 0; i < rain_vec.size(); i++) {
      MatrixRain* rain = &rain_vec[i];
      rain->toggleActive();
    }
  }

  // ==============================
  // === DRAW =====================
  // ==============================

  // redraw entire canvas on every tick

  // clear entire screen
  canvas->fillScreen(BLACK);

  // draw everything
  // for each MatrixRain, call tick and draw
  for (std::size_t i = 0; i < rain_vec.size(); i++) {
    MatrixRain* rain = &rain_vec[i];
    rain->tick();
    rain->draw();
  }

  // ==============================
  // ===  DEBUG  ==================
  // ==============================
  // canvas->fillScreen(BLACK);
  // DebugAlphabet::debug_alphabet(canvas);
  // DebugAlphabet::debug_hiragana(canvas);
}

void initialize()
{
  // setup variables
  // draw something which is not moving
  // etc.

  // init text size as small as possible
  canvas->setTextSize(1);

  // initialize Touch instance
  touch = new Touch(&Wire);

  // generate all MatrixRain instances and push onto vector
  uint8_t columns = screen.x / MatrixRain::cell.x;
  for (uint8_t i = 0; i < columns; i++) {
    rain_vec.push_back(MatrixRain(canvas, i));
  }
}

// NOTE: `loop`, `setup` and `flush` are sort of common operations
// Modify `initialize` and `tick` to customize this demo

void flush()
{
  // write canvas to screen
  tft.drawRGBBitmap(0, 0, (uint16_t*)canvas->getBuffer(), canvas->width(), canvas->height());
}

void loop()
{
  uint16_t now = millis();
  uint16_t time = now - lastLoop;

#if DEBUG
  DLOG("frameMs=%d\n", time);
  DLOG("FPS=%g\n", floor(1000.0f / (float)time));
#endif

  if (time < FPS) {
    DLOG("[WARNING] throttling FPS to [%d]; skipping draw\n", FPS);
    return;
  }

  // if we reach this point we have reached a frame (greater or equal to FPS threshold)

  // track loop millis to keep steady fps
  lastLoop = now;

  // run!
  tick();

  // flush our in-memory canvas to the screen
  flush();
}

void setup(void)
{
  Wire.begin();
  // Max is 700000 in 160MHz mode and 400000 in 80MHz mode
  // Run I2C at 700 KHz for faster screen updates
  Wire.setClock(700000);

  Serial.begin(SERIAL_DATA_RATE);
  tft.begin(SPI_SPEED);
  // initialize screen to black
  tft.fillScreen(BLACK);

  // save dimensions
  screen.x = tft.width();
  screen.y = tft.height();

  // initialize canvas to all black
  canvas = new GFXcanvas16(screen.x, screen.y);
  canvas->fillScreen(BLACK);

  // initialize srand
  randomSeed(ESP.getCycleCount());

  initialize();
  flush();
}
