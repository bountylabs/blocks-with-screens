#include <Adafruit_SSD1351.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <algorithm>

#include <Colors.h>
#include <Random.h>
#include "CurvySnake.h"
#include <GameStructs.h>


extern Adafruit_SSD1351 tft;

// in-memory 16-bit canvas
GFXcanvas16 *canvas;
int screenWidth = -1;
int screenHeight = -1;

// used by CurvySnake_loop for tick frequency
const int FPS = floor(1000 / 60);// every ~16ms (60fps)
// track last run time in ms for CurvySnake_loop
uint16_t lastLoop = millis() - FPS + 1;



// state
// point position location
Point pos = {.x = 0, .y = 0};
// velocity in x and y direction
Vector vel = {.x = 0, .y = 0};
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


void flush()
{
  tft.drawRGBBitmap(0, 0, (uint16_t *)canvas->getBuffer(), screenWidth, screenHeight);
  //tft.startWrite();
  // SPI.writeBytes((uint8_t *)canvas->getBuffer(), 128 * 128 * 2);
  //tft.endWrite();
}

void tick()
{
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
    color = randomColor();
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

  // Serial.printf("(%f, %f) + (%f, %f) @ %ddeg\n", pos.x, pos.y, vel.x, vel.y, rotation);

  // cool idea: we could have it bounce when it hits an edge?
  // ensure we stay within bounds
  if (pos.x >= screenWidth - 1)
  {
    // reduce and flip velocities; flip rotation
    vel.x = vel.x * -1 / 2;
    vel.y = vel.y * -1 / 2;
    rotation = DEGREES_MAX - (rotation % DEGREES_MAX);

    // bump backward based on velocity
    pos.x = (vel.x * 10) + (screenWidth - 1);
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

  if (pos.y >= screenHeight - 1)
  {
    vel.x = vel.x * -1 / 2;
    vel.y = vel.y * -1 / 2;
    rotation = DEGREES_MAX - (rotation % DEGREES_MAX);

    pos.x = (vel.x * 10) + pos.x;
    pos.y = (vel.y * 10) + (screenHeight - 1);
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

void CurvySnake_setup() {
  // save dimensions
  screenWidth = tft.width();
  screenHeight = tft.height();
  // initialize canvas to all black
  canvas = new GFXcanvas16(screenWidth, screenHeight);
  canvas->fillScreen(BLACK);
  flush();

  // initialize srand
  randomSeed(ESP.getCycleCount());

  // start at random x, y
  pos.x = floor(screenWidth * random());
  pos.y = floor(screenHeight * random());

  // init trail
  for (int t = 0; t < TRAIL_LEN; t++)
  {
    trail[t] = {.point = {.x = pos.x, .y = pos.y}, .color = WHITE};
  }

  rotation = (int)floor(DEGREES_MAX * random());

  Serial.printf("\nFPS=%d\n", FPS);
  Serial.println("State");
  Serial.printf("(%f, %f) + (%f, %f) @ %ddeg\n", pos.x, pos.y, vel.x, vel.y, rotation);

  Serial.printf("\n%d -- %d\n", RGB565(255, 0, 0), RED);
  Serial.printf("\n%d -- %d\n", RGB565(0, 255, 0), GREEN);
  Serial.printf("\n%d -- %d\n", RGB565(0, 0, 255), BLUE);
}

void CurvySnake_loop() {
  uint16_t now = millis();
  uint16_t time = now - lastLoop;

  if (time > FPS)
  {
    // track loop millis to keep steady fps
    lastLoop = now;

    // run!
    tick();

    // flush our in-memory canvas to the screen
    flush();
  }
}
