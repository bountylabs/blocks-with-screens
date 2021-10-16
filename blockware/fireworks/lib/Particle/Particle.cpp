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
    _vel.set(random(-3, 3), random(-7, -3));
    _resistance = 1.0f;
  } else {
    float angle = random(TAU);
    float speed = cos(random(0, TAU)) * 15;
    _vel.set(cos(angle) * speed, sin(angle) * speed);
    _resistance = 0.92f;
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
  return !_isRocket && _age >= MAX_AGE;
}

// move with velocity and rotation
void Particle::tick()
{
  _age += 1;

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
  // draw pixel at pos
  canvas->drawPixel(pX, pY, _color);

  // draw trail
  // connect current point with most recent trail at front trail[0]
  canvas->drawLine(pX, pY, _trail[0].x, _trail[0].y, _color);
}
