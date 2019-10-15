#include <Adafruit_SSD1351.h>

#include <Vec2d.h>

typedef struct _ColorPoint {
  Vec2d<int> point;
  int color;
} ColorPoint;

const int X = 0;
const int Y = 1;

// Already defined in /Users/noah/.platformio/packages/framework-arduinoespressif8266/cores/esp8266/Arduino.h:66:20
// const float PI = 3.14159265;
// const float DEG_TO_RAD = PI/180;
const int DEGREES_MAX = 360;

// Maximum trail length
const int MAX_TRAIL_LEN = 50;

class Entity {
private:
protected:
  Vec2d<int> _screen; // screen dimensions
  Vec2d<float> _pos;  // position point
  float _thrust;      // forward thrust
  Vec2d<float> _vel;  // velocity position change rate
  int _rot;           // rotation in degrees
  int _rotD;          // rotation change rate
  int _color;         // rgb565
  int _trailLen;      // trail size
  float _massInv;     // inverse of mass in kg
  // store the last _trailLen points along with their color
  std::vector<ColorPoint> _trail;

  void recordTrailPoint();
  void move();
  void handleScreenCollision();

public:
  Entity(int screenWidth, int screenHeight);
  void debug();
  void draw(GFXcanvas16* canvas);
  void tick();
};
