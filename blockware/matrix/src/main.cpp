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

#include "Canvas.h"
#include "FPS.h"
#include "MatrixRain.h"
#include "Touch.h"
#include "DebugAlphabet.h"


// handle FPS throttle via tick
FPS* fps;

// Touch instance
// Can detect taps on all 3 cartesian axes
Touch* touch;

// MatrixRain vector for columns
std::vector<MatrixRain> rain_vec;

Vec2d<uint8_t> pan = Vec2d<uint8_t>(2, 0);

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
  Canvas::canvas->fillScreen(BLACK);

  // draw everything
  // for each MatrixRain, call tick and draw
  for (std::size_t i = 0; i < rain_vec.size(); i++) {
    MatrixRain* rain = &rain_vec[i];
    rain->tick(pan);
    rain->draw();
  }

  // ==============================
  // ===  DEBUG  ==================
  // ==============================
  // Canvas::canvas->fillScreen(BLACK);
  // DebugAlphabet::debug_alphabet(Canvas::canvas);
  // DebugAlphabet::debug_hiragana(Canvas::canvas);
}

void initialize()
{
  // setup variables, draw something which is not moving, etc.
  Canvas::setup();
  // init text size as small as possible
  Canvas::canvas->setTextSize(1);

  // inotialize FPS instance
  fps = new FPS(30);

  // initialize Touch instance
  touch = new Touch(&Wire);

  // generate all MatrixRain instances and push onto vector
  uint8_t columns = Canvas::canvas->width() / MatrixRain::cell.x;
  for (uint8_t i = 0; i < columns; i++) {
    rain_vec.push_back(MatrixRain(Canvas::canvas, i));
  }
}

// NOTE: `loop`, `setup` and `flush` are sort of common operations
// Modify `initialize` and `tick` to customize this demo

void loop()
{
  if (!fps->tick()) {
    return;
  }

  // run!
  tick();

  // flush our in-memory canvas to the screen
  Canvas::flush();
}

void setup(void)
{
  Wire.begin();
  // Max is 700000 in 160MHz mode and 400000 in 80MHz mode
  // Run I2C at 700 KHz for faster screen updates
  Wire.setClock(700000);
  Serial.begin(SERIAL_DATA_RATE);

  // initialize srand
  randomSeed(ESP.getCycleCount());

  initialize();
}
