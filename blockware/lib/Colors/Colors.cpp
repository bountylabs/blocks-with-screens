#include <math.h>

#include <Colors.h>

int randomColor()
{
  // random color
  int colorIndex = floor(COLORS_LEN * random());
  return COLORS[colorIndex];
}
