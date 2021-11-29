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
#include "DebugAlphabet.h"
#include "FPS.h"
#include "Matrix.h"
#include "MatrixRain.h"
#include "Touch.h"

FPS* fps;
Matrix* matrix;

void tick()
{
  matrix->tick();

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

  // inotialize FPS instance
  fps = new FPS(30);

  // setup Matrix animation
  matrix = new Matrix();
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
