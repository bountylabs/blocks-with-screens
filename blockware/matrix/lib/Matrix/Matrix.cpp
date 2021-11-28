// set SERIALLOG to 1 to enable serial logging
// set TELNETLOG to 1 to enable telnet logging
// see DLog.h
#define DEBUG 0
#define SERIALLOG 1
#include <DLog.h>

#include "Matrix.h"

void Matrix::init()
{
  // init text size as small as possible
  Canvas::canvas->setTextSize(1);

  // initialize Touch instance
  touch = new Touch(&Wire);

  // generate enough MatrixRain instances to cover 2 screen widths and push onto vector
  int matrixWidth = 2 * Canvas::canvas->width();
  int centerX = (matrixWidth / 2) - (Canvas::canvas->width() / 2);
  // centerX is the x position that centers the visible canvas (screen) over total animation width (matrixWidth)
  // (left edge)    0 - centerX
  // (right edge)   centerX + canvas->width()
  leftEdgeX = 0 - centerX;
  rightEdgeX = centerX + Canvas::canvas->width();
  // DLOG("[main][initialize] [leftEdge=%04d,rightEdge=%04d]", leftEdge, rightEdge);

  // create enough columns to cover total width of animation
  uint8_t columns = matrixWidth / MatrixRain::cell.x;
  for (uint8_t i = 0; i < columns; i++) {
    // begin at leftEdge (not visible runway)
    rain_vec.push_back(MatrixRain(i, leftEdgeX));
  }
}

Matrix::Matrix()
{
  init();
}

void Matrix::handleTouch()
{
  // tick Touch instance
  touch->tick();

  // // detect tap on y axis
  // if (touch->isTap(Touch::Y_AXIS)) {
  //   DLOG("\n[Touch] tap! (toggling active on the MatrixRain)");
  //   // toggle active on the MatrixRain instances
  //   for (std::size_t i = 0; i < rain_vec.size(); i++) {
  //     MatrixRain* rain = &rain_vec[i];
  //     rain->toggleActive();
  //   }
  // }

  // if (touch->isRest()) {
  //   DLOG("\n[Touch] resting...");
  // }
}

// linear interpolation of value x from (x1, x2) range into (y1, y2) range
float interpolate(float x, float x1, float x2, float y1, float y2)
{
  // keep x within bounds of (x1,x2)
  if (x > x2) {
    x = x2;
  } else if (x < x1) {
    x = x1;
  }

  // DLOG("\n[interpolate] [x=%.2f,x1=%.2f,x2=%.2f,y1=%.2f,y2=%.2f]", x, x1, x2, y1, y2);
  float result = y1 + ((x - x1) / (x2 - x1)) * (y2 - y1);
  // DLOG("\n[interpolate] result = %.2f", result);
  return result;
}

void Matrix::handlePan()
{
  // when at rest, randomly adjust pan
  if (touch->isRest()) {
    // randomly change pan vector in increments of 1
    if (randomf() < 0.0025) {
      int sign = randomf() < 0.5 ? +1 : -1;
      pan.x += sign * PAN_INC;
      // DLOG("\n[Matrix::handlePan] [pan.y += %d] (%d, %d)", pan.x, pan.y);
    }
    else if (randomf() < 0.005) {
      int sign = randomf() < 0.5 ? +1 : -1;
      pan.y += sign * PAN_INC;
      // DLOG("\n[Matrix::handlePan] [pan.x += %d] (%d, %d)", pan.x, pan.y);
    }

    // DLOG("\n[Matrix::handlePan] [bounds=(%d, %d)] (%d, %d)]", PAN_MIN, PAN_MAX, pan.x, pan.y);
    // DLOG("\n[Matrix::handlePan] [pan.x < PAN_MIN=%s]", pan.x < PAN_MIN ? "true" : "false");
    // DLOG("\n[Matrix::handlePan] [pan.x > PAN_MAX=%s]", pan.x > PAN_MAX ? "true" : "false");
    // DLOG("\n[Matrix::handlePan] [pan.y < PAN_MIN=%s]", pan.y < PAN_MIN ? "true" : "false");
    // DLOG("\n[Matrix::handlePan] [pan.y > PAN_MAX=%s]", pan.y > PAN_MAX ? "true" : "false");

    if (pan.x < PAN_MIN || pan.x > PAN_MAX) {
      pan.x = (int)time_random(PAN_MIN, PAN_MAX);
      // DLOG("\n[Matrix::handlePan] [pan.x outside bounds, randomizing] (%d, %d)]", pan.x, pan.y);
    }
    else if (pan.y < PAN_MIN || pan.y > PAN_MAX) {
      pan.y = (int)time_random(PAN_MIN, PAN_MAX);
      // DLOG("\n[Matrix::handlePan] [pan.y outside bounds, randomizing] (%d, %d)]", pan.x, pan.y);
    }
  }
  else {
    // Convert accelerometer data into velocity
    // x (-2000, +2000)
    // z (-3000, +1500)
    float x = touch->X();
    float z = touch->Z();
    pan.x = interpolate(x, -2000.0f, 2000.0f, PAN_MAX, PAN_MIN);
    pan.y = interpolate(z, -3000.0f, 1500.0f, PAN_MAX, PAN_MIN);
    // DLOG("\n[Matrix::handlePan] [vx=%02f][vy=%02f]", pan.x, pan.y);
  }
}

void Matrix::handleEdges()
{
  // detect whether we are at left or right runway bound edges
  // when a column goes off either edge (less than -0.5x screen width or greater than 1.5x screen width) we can recycle
  // a recycle is just creating a new instance of matrixrain at the edge opposite where we hit

  // NOTE: Using a linked list here might be super nice
  // Would require refactoring but essentially we are always moving the head to the tail or the tail to the head
  // This can be implemented as simply moving the head pointer to the correct index
  // e.g. instead of moving 0th element to Nth spot, we can move header pointer to 1st spot
  // which effectively shifts all elements left by one and moves the 0th element to be the last spot
  // i.e. the last thing we will iterate when walking from head for size iterations

  MatrixRain left_edge = rain_vec[0];
  MatrixRain right_edge = rain_vec[rain_vec.size() - 1];
  // hit left edge?
  // shift all left by one and extend Nth (last) element of right runway
  if (left_edge.x < leftEdgeX) {
    // DLOG("\nleft_edge recycle!");
    for (std::size_t i = 0; i < rain_vec.size() - 1; i++) {
      rain_vec[i] = rain_vec[i + 1];
    }
    left_edge.reset(rightEdgeX, 0);
    rain_vec[rain_vec.size() - 1] = left_edge;
  }
  // hit right edge?
  // shift all right by one and add a new 0th extending left runway
  if (right_edge.x > rightEdgeX) {
    // DLOG("\nright_edge recycle!");
    for (std::size_t i = rain_vec.size() - 1; i > 0; i--) {
      rain_vec[i] = rain_vec[i - 1];
    }
    right_edge.reset(leftEdgeX, 0);
    rain_vec[0] = right_edge;
  }
}

void Matrix::draw()
{
  // ==============================
  // === DRAW =====================
  // ==============================
  // redraw entire canvas on every tick

  // clear entire screen
  Canvas::canvas->fillScreen(BLACK);

  // tick and draw each MatrixRain instance
  // for each MatrixRain, call tick and draw
  for (std::size_t i = 0; i < rain_vec.size(); i++) {
    MatrixRain* rain = &rain_vec[i];
    rain->tick(pan);
    rain->draw();
  }
}

void Matrix::tick()
{
  this->handleTouch();
  this->handlePan();
  this->handleEdges();
  this->draw();
}
