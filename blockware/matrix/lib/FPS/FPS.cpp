// set SERIALLOG to 1 to enable serial logging
// set TELNETLOG to 1 to enable telnet logging
// see DLog.h
#define DEBUG 0
#define SERIALLOG 1
#include <DLog.h>
#include <math.h>

#include "FPS.h"

void FPS::init(int _fps)
{
  fps = _fps;
  millisPerFrame = floor(1000 / fps);
  lastLoop = millis();
}

FPS::FPS(int _fps)
{
  init(_fps);
}

FPS::FPS()
{
  init(30);
}


bool FPS::tick() {
  uint16_t now = millis();
  uint16_t time = now - lastLoop;

#if DEBUG
  DLOG("[FPS][Target=%d,Actual=%d]\n", fps, floor(1000 / time));
#endif

  if (time < fps) {
#if DEBUG
    DLOG("[WARNING][FPS] throttling FPS to [%d]; skipping tick\n", fps);
#endif
    return false;
  }

  // if we reach this point we have reached a frame (greater or equal to FPS threshold)

  // track loop millis to keep steady fps
  lastLoop = now;

  return true;
}
