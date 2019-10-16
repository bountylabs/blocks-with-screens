#include <Adafruit_SSD1351.h>
#include <math.h>

#include <Colors.h>
#include <Random.h>
#include "Entity.h"

Entity::Entity(int screenWidth, int screenHeight)
{
  // screen dimensions
  _screen = Vec2d<int>(screenWidth, screenHeight);
  // point position location
  _pos = Vec2d<float>(screenWidth * random(), screenHeight * random());
  // forward thrust
  _thrust = 0.15;
  // velocity in x and y direction
  _vel = Vec2d<float>();
  // rotation in degrees
  _rot = (int)floor(DEGREES_MAX * random());
  _rotD = +6;
  // color of the pixel
  _color = randomColor();
  // number of ColorPoints to store in trail
  _trailLen = 50;
  // inverse of mass in kg
  _massInv = 1 / 10;

  // init trail
  for (int t = 0; t < _trailLen; t++) {
    _trail.push_back({.point = Vec2d<int>(floor(_pos.x), floor(_pos.y)), .color = _color});
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
  _trail[0] = {.point = Vec2d<int>(floor(_pos.x), floor(_pos.y)), .color = _color};
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
  // for a single point collision lets simplify to the
  // top right bottom or left side at a distance of 1
  Vec2d<float> normal = Vec2d<float>();
  if (_pos.x >= _screen.x - 1) {
    normal.set(-1.0, normal.y);
  } else if (_pos.x <= 0) {
    normal.set(+1.0, normal.y);
  }

  if (_pos.y >= _screen.y - 1) {
    normal.set(normal.x, -1.0);
  } else if (_pos.y <= 0) {
    normal.set(normal.x, +1.0);
  }

  // normal.normalize();
  // Serial.printf("\nnormal (%3.1f,%3.1f)", normal.x, normal.y);
  // Serial.printf("\nvel=(%3.1f, %3.1f)", _vel.x, _vel.y);


  if(normal == 0) {
    // Serial.println("no collision detected, skipping collision logic");
    return;
  }

  // Do not resolve if velocity is moving away from normal
  if ((_vel.x * normal.x > 0) || (_vel.y * normal.y > 0)) {
    // Serial.println("vel moving away from normal, skipping collision logic");
    return;
  }


  // colliding with a wall is like an entity with 0 velocity and very high mass
  // relative velocity will be velocity of entity
  // Vec2d<float> rv = _vel;

  // adjust velocity in terms of the normal direction
  if (abs(normal.x)) {
    _vel.y *= -1;
  }

  if (abs(normal.y)) {
    _vel.y *= -1;
  }

  // Serial.printf("\n(n) vel=(%3.1f, %3.1f)", _vel.x, _vel.y);

  // restitution (e) is the some constant multiplied by velocity
  // to get velocity after the collision, i.e. bounciness / elasticity
  //    v' = v * e
  float e = 0.25;
  _vel *= e;
  // Serial.printf("\n(e) vel=(%3.1f, %3.1f)", _vel.x, _vel.y);
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
