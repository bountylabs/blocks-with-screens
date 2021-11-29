#pragma once

#include <Adafruit_SSD1351.h>

class FPS {
private:
  int fps;
  int millisPerFrame;
  uint16_t lastLoop;
protected:
public:
  void init(int fps);
  FPS(int fps);
  FPS();
  bool tick();
};


