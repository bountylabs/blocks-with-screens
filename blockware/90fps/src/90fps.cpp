#include "90fps.h"
#include <Adafruit_SSD1351.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <list>
#include <Colors.h>
#include <Random.h>

const int COLORS_LEN = 7;
const int COLORS[COLORS_LEN] = {BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
const int DIRECTIONS_LEN = 3;
const int DIRECTIONS[DIRECTIONS_LEN] = {-1, 0, 1};

typedef struct _Point
{
  int x;
  int y;
} Point;

typedef struct _ColorPoint
{
  Point point;
  int color;
} ColorPoint;

const int X = 0;
const int Y = 1;

// screen dimensions
int height = -1;
int width = -1;

// state
// point position location
Point pos = {.x = 0, .y = 0};
// x and y can be [-1,+1], representing the change in x,y over tick
// e.g. x=1,y=-1 is moving in the downward right diagonal direction
// covers all 8 directional moves in a 2d cartesian space
Point dir = {.x = +0, .y = +1};
// color of the pixel
int color = WHITE;

// store the last 10 points along with their color
const int TRAIL_LEN = 50;
ColorPoint trail[TRAIL_LEN];

void pushFront(ColorPoint *trail, ColorPoint newFront)
{
  // shift every item down by one index
  for (int t = TRAIL_LEN - 1; t >= 1; t--)
  {
    trail[t] = trail[t - 1];
  }

  // put new point at front
  trail[0] = newFront;
}

void run90FPS(GFXcanvas16 *canvas)
{

  // init
  if (height == -1)
  {
    // initialize srand
    randomSeed(ESP.getCycleCount());

    height = canvas->height();
    width = canvas->width();
    // start at random x, y
    pos.x = floor(width * randomf());
    pos.y = floor(height * randomf());

    // init trail
    for (int t = 0; t < TRAIL_LEN; t++)
    {
      trail[t] = {.point = {.x = pos.x, .y = pos.y}, .color = WHITE};
    }
  }

  // // 1% chance to reset
  // if (0.0001 > randomf())
  // {
  //   canvas->fillScreen(BLACK);
  //   pos.x = floor(width * randomf());
  //   pos.y = floor(height * randomf());
  // }

  // keep a trail of our pos
  pushFront(trail, {.point = {.x = pos.x, .y = pos.y}, .color = color});

  // 10% chance to change direction and color
  // also force change if we accidentally stop
  if (randomf() < 0.01 || dir.x == 0 && dir.y == 0)
  {
    dir.x = DIRECTIONS[(int)floor(DIRECTIONS_LEN * randomf())];
    dir.y = DIRECTIONS[(int)floor(DIRECTIONS_LEN * randomf())];

    randomColor();
  }

  // clear previous pixels
  canvas->fillScreen(BLACK);
  // canvas->drawPixel(pos.x, pos.y, BLACK);

  // move in direction
  pos.x = pos.x + dir.x;
  pos.y = pos.y + dir.y;

  // cool idea: we could have it bounce when it hits an edge?
  // ensure we stay within bounds
  if (pos.x >= width - 1)
  {
    // flip direction
    dir.x = dir.x * -1;
    pos.x = width - 1;
    // // random chance to flip y when hitting x bound
    // if (randomf() < 0.1)
    // {
    //   dir.y = DIRECTIONS[(int)floor(DIRECTIONS_LEN * randomf())];
    //   randomColor();
    // }
  }
  else if (pos.x <= 0)
  {
    // flip direction
    dir.x = dir.x * -1;
    pos.x = 0;
  }

  if (pos.y >= height - 1)
  {
    dir.y = dir.y * -1;
    pos.y = height - 1;
  }
  else if (pos.y <= 0)
  {
    dir.y = dir.y * -1;
    pos.y = 0;
  }

  // draw pixel at new pos
  // Serial.printf("[%d,%d] %d", pos.x, pos.y, color);
  canvas->drawPixel(pos.x, pos.y, color);

  // draw trail
  // connect current point with most recent trail at front trail[0]
  canvas->drawLine(pos.x, pos.y, trail[0].point.x, trail[0].point.y, trail[0].color);
  // then connect subseqent trail ColorPoints until we hit the end
  for (int t = 1; t < TRAIL_LEN; t++)
  {
    //Serial.printf("[%d,%d]", trail[t].point.x, trail[t].point.y);
    canvas->drawLine(trail[t - 1].point.x, trail[t - 1].point.y, trail[t].point.x, trail[t].point.y, trail[t].color);
  }
  //Serial.printf("\n");

  //delay(1000);
}
