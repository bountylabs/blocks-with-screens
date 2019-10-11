#include "90fps.h"
#include <Adafruit_SSD1351.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <algorithm>

// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

const int COLORS_LEN = 7;
const int COLORS[COLORS_LEN] = {BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
const int DIRECTIONS_LEN = 3;
const int DIRECTIONS[DIRECTIONS_LEN] = {-1, 0, 1};
const int X = 0;
const int Y = 1;
// Already defined in /Users/noah/.platformio/packages/framework-arduinoespressif8266/cores/esp8266/Arduino.h:66:20
// const float PI = 3.14159265;
// const float DEG_TO_RAD = PI/180;
const int DEGREES_MAX = 360;

double random()
{
  // Use current time as seed for random generator
  return (double)rand() / (float)RAND_MAX;
}

typedef struct _Point
{
  double x;
  double y;
} Point;

typedef struct _Vector
{
  double x;
  double y;
} Vector;

typedef struct _ColorPoint
{
  Point point;
  int color;
} ColorPoint;

// screen dimensions
int height = -1;
int width = -1;

// state
// point position location
Point pos = {.x = 0, .y = 0};
// velocity in x and y direction
Vector vel = {.x = 0, .y = 0};
// x and y can be [-1,+1], representing the change in x,y over tick
// e.g. x=1,y=-1 is moving in the downward right diagonal direction
// covers all 8 directional moves in a 2d cartesian space
Point dir = {.x = +0, .y = +1};
// color of the pixel
int color = WHITE;
// rotation in degrees
int rotation = 0;
int rotationDirection = +6;

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

void randomColor()
{
  // random color
  int colorIndex = floor(COLORS_LEN * random());
  color = COLORS[colorIndex];
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
    pos.x = floor(width * random());
    pos.y = floor(height * random());

    // init trail
    for (int t = 0; t < TRAIL_LEN; t++)
    {
      trail[t] = {.point = {.x = pos.x, .y = pos.y}, .color = WHITE};
    }

    rotation = (int)floor(DEGREES_MAX * random());
  }

  // // 1% chance to reset
  // if (0.0001 > random())
  // {
  //   canvas->fillScreen(BLACK);
  //   pos.x = floor(width * random());
  //   pos.y = floor(height * random());
  // }

  // keep a trail of our pos
  pushFront(trail, {.point = {.x = floor(pos.x), .y = floor(pos.y)}, .color = color});

  // 1% chance to change rotation and color
  if (random() < 0.01)
  {
    rotationDirection = rotationDirection * -1;
    randomColor();
  }

  // clear previous pixels
  canvas->fillScreen(BLACK);
  // canvas->drawPixel(pos.x, pos.y, BLACK);

  // move with velocity and rotation
  rotation += rotationDirection;
  vel.x += cos(rotation * DEG_TO_RAD) * 0.15;
  vel.y += sin(rotation * DEG_TO_RAD) * 0.15;
  pos.x = pos.x + vel.x;
  pos.y = pos.y + vel.y;

  Serial.printf("(%f, %f) + (%f, %f) @ %ddeg\n", pos.x, pos.y, vel.x, vel.y, rotation);

  // cool idea: we could have it bounce when it hits an edge?
  // ensure we stay within bounds
  if (pos.x >= width - 1)
  {
    // reduce and flip velocities; flip rotation
    vel.x = vel.x * -1 / 2;
    vel.y = vel.y * -1 / 2;
    rotation = DEGREES_MAX - (rotation % DEGREES_MAX);

    // bump backward based on velocity
    pos.x = (vel.x * 10) + (width - 1);
    pos.y = (vel.y * 10) + pos.y;
  }
  else if (pos.x <= 0)
  {
    vel.x = vel.x * -1 / 2;
    vel.y = vel.y * -1 / 2;
    rotation = DEGREES_MAX - (rotation % DEGREES_MAX);

    // bump backward based on velocity
    pos.x = (vel.x * 10);
    pos.y = (vel.y * 10) + pos.y;
  }

  if (pos.y >= height - 1)
  {
    vel.x = vel.x * -1 / 2;
    vel.y = vel.y * -1 / 2;
    rotation = DEGREES_MAX - (rotation % DEGREES_MAX);

    pos.x = (vel.x * 10) + pos.x;
    pos.y = (vel.y * 10) + (height - 1);
  }
  else if (pos.y <= 0)
  {
    vel.x = vel.x * -1 / 2;
    vel.y = vel.y * -1 / 2;
    rotation = DEGREES_MAX - (rotation % DEGREES_MAX);

    pos.x = (vel.x * 10) + pos.x;
    pos.y = (vel.y * 10);
  }

  // draw pixel at new pos
  // Serial.printf("[%d,%d] %d", pos.x, pos.y, color);
  canvas->drawPixel(floor(pos.x), floor(pos.y), color);

  // draw trail
  // connect current point with most recent trail at front trail[0]
  canvas->drawLine(floor(pos.x), floor(pos.y), trail[0].point.x, trail[0].point.y, trail[0].color);
  // then connect subseqent trail ColorPoints until we hit the end
  for (int t = 1; t < TRAIL_LEN; t++)
  {
    //Serial.printf("[%d,%d]", trail[t].point.x, trail[t].point.y);
    canvas->drawLine(trail[t - 1].point.x, trail[t - 1].point.y, trail[t].point.x, trail[t].point.y, trail[t].color);
  }
  //Serial.printf("\n");

  // delay(500);
}
