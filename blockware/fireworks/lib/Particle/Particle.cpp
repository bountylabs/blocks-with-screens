#include <Adafruit_SSD1351.h>
#include <math.h>

#include "Particle.h"
#include <Colors.h>
#include <Random.h>

Particle::Particle(int x, int y, bool isRocket, int color)
{
  _age = 0;
  _pos.set(x, y);
  _isRocket = isRocket;
  _color = color;
  _trail = {Vec2d<int>(_pos.x, _pos.y), Vec2d<int>(_pos.x, _pos.y), Vec2d<int>(_pos.x, _pos.y)};

  if (_isRocket) {
    _vel.set(time_random(-1, 1), time_random(-3, -2));
    _resistance = 1.025f;
  } else {
    float adj = 1000.00;
    float angle = time_random(TAU * adj) / adj;
    float speed = cos(time_random(0, TAU)) * 10;
    _vel.set(cos(angle) * speed, sin(angle) * speed);
    _resistance = 0.75f + time_random(0, 0.15f); // randomizes explosion size. 0.88f is about screen width
  }
}

boolean Particle::explode(std::vector<Particle>& explosions)
{
  //  explode if
  //    is rocketing
  //    AND
  //      at or above max height
  //      OR
  //      above min height
  //      AND
  //        left or right of x bounds
  //        OR
  //        random chance
  //      OR
  //      rocket is going down
  if (
    _isRocket
    && (
      _pos.y <= MAX_HEIGHT
      || (
        _pos.y < MIN_HEIGHT
        && (
          _pos.x <= MIN_X
          || _pos.x >= MAX_X
          || randomf() <= EXPLODE_CHANCE
        )
      )
      || (
        _pos.y > _trail[0].y
      )
    )
  ) {
    // add explosion particles to explosions vector
    for (int i = 0; i < EXPLOSION_PARTICLES; i ++) {
      explosions.push_back(Particle(_pos.x, _pos.y, false, _color));
    }

    return true;
  }

  return false;
}

bool Particle::destroy()
{
  if (_isRocket) {
    return _pos.y > 128;
  } else {
    return _age >= MAX_AGE || _color <= 0 || _pos.y > 128 || (_pos.x < 0 && _vel.x <= 0) || (_pos.x > 128 && _vel.x >= 0);
  }
}

// move with velocity and rotation
void Particle::tick()
{
  _age += 1;

  if (!_isRocket && (MAX_AGE - _age < FADE_AGE || (_age > MIN_FADE_AGE && randomf() < 0.2))) {
    _color = darken(_color, 25);
  }

  // shift every item down by one index
  for (int t = _trail.size() - 1; t >= 1; t--) {
    _trail[t] = _trail[t - 1];
  }
  // put new point at front
  _trail[0] = Vec2d<int>(floor(_pos.x), floor(_pos.y));

  _vel.x *= _resistance;
  _vel.y *= _resistance;
  _vel.y += GRAVITY;

  _pos.x += _vel.x;
  _pos.y += _vel.y;
}

void Particle::draw(GFXcanvas16* canvas)
{
  int pX = (int)floor(_pos.x);
  int pY = (int)floor(_pos.y);

  int trailIndex = 0;
  if (_isRocket) {
    trailIndex = 2;
  }
  // draw trail
  // connect current point with most recent trail at front trail[0]
  canvas->drawLine(pX, pY, _trail[trailIndex].x, _trail[trailIndex].y, _color);

  int pixelColor = _color;
  if (_isRocket || (_age > 10 && _color > 0 && randomf() < 0.2)) pixelColor = 0xFFFF;

  // draw pixel at pos
  canvas->drawPixel(pX, pY, pixelColor);
}
