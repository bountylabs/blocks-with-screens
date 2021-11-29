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
  static constexpr float PAN_MIN = -3.0f;
  static constexpr float PAN_MAX = +3.0f;
  static constexpr float PAN_INC = +1.0f;

  // Touch instance
  // Can detect taps on all 3 cartesian axes
  Touch* touch;

  // MatrixRain vector for columns
  std::vector<MatrixRain> rain_vec;

  Vec2d<float> pan = Vec2d<float>(0.0f, -1.0f);

  int leftEdgeX;
  int rightEdgeX;

  void handleTouch();
  void handlePan();
  void handleEdges();
  void draw();

protected:
public:
  void init();
  Matrix();

  void tick();
};
