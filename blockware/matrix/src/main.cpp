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

Vec2d<int8_t> pan = Vec2d<int8_t>(2, 0);;

void tick()
{
  // // tick Touch instance
  // touch->tick();
  // // detect tap on y axis
  // if (touch->isTap(Touch::Y_AXIS)) {
  //   DLOG("\n[Touch] tap! (toggling active on the MatrixRain)");
  //   // toggle active on the MatrixRain instances
  //   for (std::size_t i = 0; i < rain_vec.size(); i++) {
  //     MatrixRain* rain = &rain_vec[i];
  //     rain->toggleActive();
  //   }
  // }

  // ==============================
  // === DRAW =====================
  // ==============================

  // redraw entire canvas on every tick

  // clear entire screen
  Canvas::canvas->fillScreen(BLACK);

  // randomly change pan vector
  // if (randomf() < 0.05) {
  //   pan = Vec2d<int8_t>(pan.x * -1, 0);
  // }

  // detect whether we are at left or right runway bound edges
  // when a column goes off either edge (less than -0.5x screen width or greater than 1.5x screen width) we can recycle
  // a recycle is just creating a new instance of matrixrain at the edge opposite where we hit
  MatrixRain left_edge = rain_vec[0];
  MatrixRain right_edge = rain_vec[rain_vec.size() - 1];
  // hit left edge?
  // shift all left by one and extend Nth (last) element of right runway
  if (left_edge.recycle() == 'l') {
    DLOG("\nleft_edge recycle!");
    for (std::size_t i = 0; i < rain_vec.size() - 1; i++) {
      rain_vec[i] = rain_vec[i + 1];
    }
  }
  // hit right edge?
  // shift all right by one and add a new 0th extending left runway
  if (right_edge.recycle() == 'r') {
    DLOG("\nright_edge recycle!");
    for (std::size_t i = rain_vec.size() - 1; i > 0; i--) {
      rain_vec[i] = rain_vec[i - 1];
    }
    right_edge.reset(-0.5 * Canvas::canvas->width(), 0);
    rain_vec[0] = right_edge;

    // for (std::size_t i = 0; i < rain_vec.size(); i++) {
    //   MatrixRain rain = rain_vec[i];
    //   DLOG("\n[MatrixRain#%02d] [(%d, %d)]", i, rain.x, rain.y);
    // }
  }

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

  // generate enough MatrixRain instances to cover 2 screen widths and push onto vector
  uint8_t columns = (2 * Canvas::canvas->width()) / MatrixRain::cell.x;
  // begin at negative 0.5x screen width (not visible runway)
  int start_x = -0.5 * Canvas::canvas->width();
  for (uint8_t i = 0; i < columns; i++) {
    rain_vec.push_back(MatrixRain(Canvas::canvas, i, start_x));
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
