#pragma once

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
#include "MatrixRain.h"
#include "Touch.h"

class Matrix {
private:
  static const int8_t PAN_MIN = -3;
  static const int8_t PAN_MAX = +3;
  static const int8_t PAN_INC = +1;

  // Touch instance
  // Can detect taps on all 3 cartesian axes
  Touch* touch;

  // MatrixRain vector for columns
  std::vector<MatrixRain> rain_vec;

  Vec2d<int8_t> pan = Vec2d<int8_t>(0, -1);

  int leftEdgeX;
  int rightEdgeX;

  void handleTouch();
  void randomPan();
  void handleEdges();
  void draw();

protected:
public:
  void init();
  Matrix();

  void tick();
};
