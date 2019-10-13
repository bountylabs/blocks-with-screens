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

const int X = 0;
const int Y = 1;


// Already defined in /Users/noah/.platformio/packages/framework-arduinoespressif8266/cores/esp8266/Arduino.h:66:20
// const float PI = 3.14159265;
// const float DEG_TO_RAD = PI/180;
const int DEGREES_MAX = 360;
