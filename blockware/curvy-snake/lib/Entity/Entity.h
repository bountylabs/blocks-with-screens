#include <Adafruit_SSD1351.h>

typedef struct _Vec2 {
  double x;
  double y;
} Vec2;

typedef struct _ColorPoint {
  Vec2 point;
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
  Vec2 _screen;  // screen dimensions
  Vec2 _pos;     // position point
  double _thrust;       // forward thrust
  Vec2 _vel;     // velocity position change rate
  int _rot;             // rotation in degrees
  int _rotD;            // rotation change rate
  int _color;           // rgb565
  int _trailLen;        // trail size
  double _massInv;      // inverse of mass in kg
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
