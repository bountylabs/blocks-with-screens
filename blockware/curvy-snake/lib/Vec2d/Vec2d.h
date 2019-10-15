#include <Adafruit_SSD1351.h>

// generic T
// e.g. Vec2d<double>
// curated from https://gist.github.com/raizam/29fa7bcf127e6b36fc076c14c77d3d40

template <typename T>
class Vec2d {
  public:
    T x, y;

    // constructors
    // default constructor
    Vec2d() :x(0), y(0) {}
    // setup with x y
    Vec2d(T x, T y) : x(x), y(y) {}
    // copy constructor
    Vec2d(const Vec2d& v) : x(v.x), y(v.y) {}

    // =
    // set this vector ref to another vector ref values
    Vec2d& operator=(const Vec2d& v) {
      x = v.x;
      y = v.y;
      return *this;
    }

    // +
    // return a new vector of this + constant
    Vec2d operator+(const T& s) const {
      return Vec2d(x + s, y + s);
    }
    // +
    // return a new vector of this + vector
    Vec2d operator+(const Vec2d& v) const {
      return Vec2d(x + v.x, y + v.y);
    }
    // +=
    // update this ref by + from another vector ref
    Vec2d& operator+=(const Vec2d& v) {
      x += v.x;
      y += v.y;
      return *this;
    }


    // -
    // return a new vector of this - constant
    Vec2d operator-(const T& s) const {
      return Vec2d(x - s, y - s);
    }
    // -
    // return a new vector of this - vector
    Vec2d operator-(const Vec2d& v) const {
      return Vec2d(x - v.x, y - v.y);
    }
    // -=
    // update this ref by - from another vector ref
    Vec2d& operator-=(const Vec2d& v) {
      x -= v.x;
      y -= v.y;
      return *this;
    }


    // *
    // return a new vector of this * constant
    Vec2d operator*(const T& s) const {
      return Vec2d(x * s, y * s);
    }
    // *
    // return a new vector of this * vector
    Vec2d operator*(const Vec2d& v) const {
      return Vec2d(x * v.x, y * v.y);
    }
    // *=
    // update this ref by * from another vector ref
    Vec2d& operator*=(const float v) {
      x = x * v,
      y = y * v;
      return *this;
    }


    // /
    // return a new vector of this / vector
    Vec2d operator/(const Vec2d& v) const {
      return Vec2d(x / v.x, y / v.y);
    }
    // /
    // return a new vector of this / constant
    Vec2d operator/(const T& s)  const {
      return Vec2d(x / s, y / s);
    }
    // /=
    // update this ref by / from another vector ref
    Vec2d& operator/=(const float v) {
      x = x / v,
      y = y / v;
      return *this;
    }





    // set arbitary values
    void set(T x, T y) {
      this->x = x;
      this->y = y;
    }

    // distance formula for two cartesian points
    // difference between two points form two sides of triangle
    // hypotenuse will represent the straight line between points
    // then use pythagorean theorem (length) to calculate hypotenuse
    float dist(Vec2d v) const {
      Vec2d d(v.x - x, v.y - y);
      return d.length();
    }

    // pythagorean theorem, hypotenuse length
    float length() const {
      return std::sqrt(x * x + y * y);
    }

    // orthogonal, i.e. *this->dot(*this->ortho()) == Vec2d() // zero vector
    Vec2d ortho() const {
      return Vec2d(y, -x);
    }

    // calculate the dot product of two vectors
    static float dot(Vec2d v1, Vec2d v2) {
      return v1.x * v2.x + v1.y * v2.y;
    }

    // calculate the cross product of two vectors
    static float cross(Vec2d v1, Vec2d v2) {
      return (v1.x * v2.y) - (v1.y * v2.x);
    }

    // calculate cross product of this vector
    const float cross(Vec2d v2) const
    {
      return cross(*this, v2);
    }

    // calculate dot product of this vector
    const float dot(Vec2d v2) const
    {
      return dot(*this, v2);
    }

    // generate a unit vector by dividing to create length == 1
    Vec2d& normalize() {
      if (length() == 0) return *this;
      *this *= (1.0 / length());
      return *this;
    }

    // rotate by deg
    void rotate(double deg) {
      double theta = deg * DEG_TO_RAD;
      double c = cos(theta);
      double s = sin(theta);
      double tx = x * c - y * s;
      double ty = x * s + y * c;
      x = tx;
      y = ty;
    }
};
