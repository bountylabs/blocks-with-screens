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

const int COLORS_LEN = 7;
const int COLORS[COLORS_LEN] = {BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};

#define RGB565(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

int randomColor();
