#ifndef COLORS_H
#define COLORS_H

#include <Random.h>

// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define DARK_GRAY 0x4108
#define GRAY 0x618C
#define LIGHT_GRAY 0xA294

#define RGB565(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

static inline int randomColor()
{
  // random color
  const int COLORS[] = {BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
  int colorIndex = floor((sizeof(COLORS)/sizeof(int)) * random());
  return COLORS[colorIndex];
}

#endif
