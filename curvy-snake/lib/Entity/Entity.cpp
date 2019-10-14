#include <Adafruit_SSD1351.h>
#include <Colors.h>
#include <Random.h>

#include "Entity.h"

Entity::Entity(int screenWidth, int screenHeight)
{
  // screen dimensions
  _screen = {.x = (double)screenWidth, .y = (double)screenHeight};
  // point position location
  _pos = {.x = (screenWidth * random()), .y = floor(screenWidth * random())};
  // forward thrust
  _thrust = 0.15;
  // velocity in x and y direction
  _vel = {.x = 0, .y = 0};
  // rotation in degrees
  _rot = (int)floor(DEGREES_MAX * random());
  _rotD = +6;
  // color of the pixel
  _color = WHITE;
  // number of ColorPoints to store in trail
  _trailLen = 50;

  // init trail
  for (int t = 0; t < _trailLen; t++) {
    _trail.push_back({.point = {.x = floor(_pos.x), .y = floor(_pos.y)}, .color = _color});
  }
}

void Entity::debug()
{
  Serial.printf("(%3.1f+%3.1f, %3.1f+%3.1f) @ %d+%ddeg [color=%d] \n", _pos.x,
                _vel.x, _pos.y, _vel.y, _rot, _rotD, _color);
}

void Entity::recordTrailPoint()
{
  // shift every item down by one index
  for (int t = _trailLen - 1; t >= 1; t--) {
    _trail[t] = _trail[t - 1];
  }

  // put new point at front
  _trail[0] = {.point = {.x = floor(_pos.x), .y = floor(_pos.y)}, .color = _color};
}

void Entity::tick()
{
  recordTrailPoint();

  // 1% chance to reverse rotD and color
  if (random() < 0.01) {
    _rotD = _rotD * -1;
    _color = randomColor();
  }

  // move with velocity and rotation
  move();

  // handle collisions with screen edges
  handleScreenCollision();
}

void Entity::move()
{
  // move with velocity and rotation
  _rot += _rotD;
  _vel.x += cos(_rot * DEG_TO_RAD) * _thrust;
  _vel.y += sin(_rot * DEG_TO_RAD) * _thrust;
  _pos.x = _pos.x + _vel.x;
  _pos.y = _pos.y + _vel.y;
}

void Entity::handleScreenCollision()
{
  // cool idea: we could have it bounce when it hits an edge?
  // ensure we stay within bounds
  if (_pos.x >= _screen.x - 1) {
    // reduce and flip velocities; flip rotation
    _vel.x = _vel.x * -1 / 2;
    _vel.y = _vel.y * -1 / 2;
    _rot = DEGREES_MAX - (_rot % DEGREES_MAX);

    // bump backward based on velocity
    _pos.x = (_vel.x * 10) + (_screen.x - 1);
    _pos.y = (_vel.y * 10) + _pos.y;
  }
  else if (_pos.x <= 0) {
    _vel.x = _vel.x * -1 / 2;
    _vel.y = _vel.y * -1 / 2;
    _rot = DEGREES_MAX - (_rot % DEGREES_MAX);

    // bump backward based on velocity
    _pos.x = (_vel.x * 10);
    _pos.y = (_vel.y * 10) + _pos.y;
  }

  if (_pos.y >= _screen.y - 1) {
    _vel.x = _vel.x * -1 / 2;
    _vel.y = _vel.y * -1 / 2;
    _rot = DEGREES_MAX - (_rot % DEGREES_MAX);

    _pos.x = (_vel.x * 10) + _pos.x;
    _pos.y = (_vel.y * 10) + (_screen.y - 1);
  }
  else if (_pos.y <= 0) {
    _vel.x = _vel.x * -1 / 2;
    _vel.y = _vel.y * -1 / 2;
    _rot = DEGREES_MAX - (_rot % DEGREES_MAX);

    _pos.x = (_vel.x * 10) + _pos.x;
    _pos.y = (_vel.y * 10);
  }
}

void Entity::draw(GFXcanvas16* canvas)
{
  int posX = (int)floor(_pos.x);
  int posY = (int)floor(_pos.y);
  // draw pixel at pos
  canvas->drawPixel(posX, posY, _color);

  // draw trail
  // connect current point with most recent trail at front trail[0]
  // Serial.printf("canvas->drawLine(%d, %d, %3.0f, %3.0f, %d)", posX, posY, _trail[0].point.x, _trail[0].point.y, _trail[0].color);
  canvas->drawLine(posX, posY, _trail[0].point.x, _trail[0].point.y, _trail[0].color);
  // then connect subseqent trail ColorPoints until we hit the end
  for (int t = 1; t < _trailLen; t++) {
    // Serial.printf("canvas->drawLine(%3.0f, %3.0f, %3.0f, %3.0f, %d)", _trail[t - 1].point.x, _trail[t - 1].point.y, _trail[t].point.x, _trail[t].point.y, _trail[t].color);
    canvas->drawLine(_trail[t - 1].point.x, _trail[t - 1].point.y,
                     _trail[t].point.x, _trail[t].point.y, _trail[t].color);
  }
  // Serial.printf("\n");
}
