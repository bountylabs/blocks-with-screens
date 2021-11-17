#include <Adafruit_SSD1351.h>

#include <Vec2d.h>
#include <Colors.h>

const float TAU = PI * 2;
const float GRAVITY = 0.07;
const int MAX_HEIGHT = floor(0.1 * 128);
const int MIN_X = floor(0.1 * 128);
const int MIN_HEIGHT = floor(0.7 * 128);
const int MAX_X = floor(0.8 * 128);
const float EXPLODE_CHANCE = 0.05;
const int EXPLOSION_PARTICLES = 100;
const int MAX_AGE = 120;
const int FADE_AGE = MAX_AGE / 2;
const int MIN_FADE_AGE = MAX_AGE / 6;

const std::vector<unsigned int> FIREWORK_COLORS{ BLUE,RED,GREEN,CYAN,MAGENTA,YELLOW,DARK_GRAY,GRAY,LIGHT_GRAY };

static inline int randomFireworkColor()
{
  return FIREWORK_COLORS[floor(FIREWORK_COLORS.size() * randomf())];
}



class Particle {
private:
  int _age;         // how old is this particle?
  bool _isRocket;   // is this particle rocketing?
  Vec2d<float> _pos;  // x,y coords
  // history of x,y coords
  std::vector<Vec2d<int>> _trail;
  // x,y velocity
  Vec2d<float> _vel;
  // color of particle
  int _color;
  // change to velocity (1 when rocketing, <1 when firework)
  float _resistance;

protected:
public:
  Particle(int x, int y, bool isRocket, int color);
  boolean isRocket();
  void draw(GFXcanvas16* canvas);
  void tick();
  boolean explode(std::vector<Particle>& explosions);
  bool destroy();
};
