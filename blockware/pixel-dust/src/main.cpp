#include <DefaultConfig.h>
// #define SERIALLOG 1
#include <DLog.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <Text.h>
#include <Adafruit_PixelDust.h>
#include <Colors.h>

// In 80MHz mode, we can do 100 grains at about 120FPS
// In 160MHz mode, we can do 100 grains at about 185FPS which feels nice

#define N_GRAINS    100 // Number of grains of sand
#define MAX_FPS     185 // Maximum redraw rate, frames/second
#define ELASTICITY  128 // Bounciness (0-255)

// Sand object, last 2 args are accelerometer scaling and grain elasticity
Adafruit_PixelDust sand(SCREEN_WIDTH, SCREEN_HEIGHT, N_GRAINS, 1, ELASTICITY);

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// Colors for each one of the grains
uint16_t colors[N_GRAINS];

typedef struct {
  dimension_t x;
  dimension_t y;
} point;
point grains[N_GRAINS];

// Twitter logo in blue
int larryWidth = 40;
int larryHeight = 33;
unsigned char larry[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0xd4, 0x43, 0xfd, 0x1c, 0xfe, 0x1c,
  0x1d, 0x25, 0xdd, 0x1c, 0x1e, 0x1d, 0xfe, 0x1c, 0xfd, 0x1c, 0xfe, 0x1c, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x4b,
  0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0x4c, 0x86, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0x14, 0xfd, 0x24, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0x56, 0x4c, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0x1c, 0xfc, 0x24,
  0x76, 0x4c, 0x86, 0x00, 0x86, 0x00, 0x76, 0x4c, 0x1d, 0x1d, 0xfe, 0x1c, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x00, 0x97, 0x4c, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0xfe, 0x1c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x55, 0x4c, 0x67, 0x00, 0x67, 0x00, 0x76, 0x54, 0xfe, 0x1c, 0x1d,
  0x1d, 0x97, 0x54, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbd, 0x1c, 0x1d, 0x25, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0xfd, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x87, 0x00, 0x77, 0x4c, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1d, 0x1d, 0xfd, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 0x1c, 0xfe, 0x1c,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0xdc, 0x24, 0x7b, 0x14, 0xdc, 0x1c, 0xbc, 0x14, 0x67, 0x00, 0x76,
  0x54, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0x76, 0x4c, 0x86, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x00, 0x76, 0x54, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x25, 0xfe, 0x1c, 0x1d, 0x1d, 0xfd, 0x24, 0x9b, 0x1c, 0x86,
  0x00, 0x77, 0x4c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0xfe, 0x1c, 0x97, 0x54, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x00, 0x77, 0x4c,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x97, 0x54, 0x87,
  0x00, 0x67, 0x00, 0x76, 0x54, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0xdd, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x00,
  0x76, 0x54, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x25, 0x00,
  0x00, 0x00, 0x00, 0x87, 0x00, 0x77, 0x4c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0xfe,
  0x1c, 0x56, 0x4c, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x86, 0x00, 0x56, 0x4c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0xf3, 0x43, 0x66,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0xf3, 0x4b, 0xfe, 0x1c, 0x1d, 0x25,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xdd, 0x1c, 0xdc, 0x1c, 0x35, 0x4c,
  0x66, 0x00, 0x67, 0x00, 0x97, 0x54, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 0x1c,
  0xfe, 0x1c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0xfe, 0x1c, 0xfd, 0x1c, 0xfe, 0x1c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x91, 0x43,
  0x67, 0x00, 0x97, 0x54, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x00,
  0x77, 0x4c, 0xfd, 0x1c, 0xdd, 0x1c, 0xfd, 0x1c, 0xfe, 0x1c, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x67, 0x00, 0x76, 0x54, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x86, 0x00, 0x77, 0x4c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0xfe, 0x1c, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x67, 0x00, 0x96, 0x54, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0xdc, 0x1c, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 0x1c, 0xfe,
  0x1c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0xfc, 0x24, 0x5b, 0x14,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67,
  0x00, 0x97, 0x54, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0x77, 0x4c,
  0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x87, 0x00, 0xb8, 0x4c, 0xfd, 0x1c, 0xfe, 0x1c, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x76, 0x4c, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x35, 0x4c, 0xfe, 0x1c,
  0xfd, 0x24, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0xfd, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1d, 0x25, 0xdd, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdd, 0x1c,
  0x1d, 0x25, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0x97, 0x4c, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x87, 0x00, 0x77, 0x4c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x35, 0x4c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x66, 0x00, 0x14, 0x4c, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d,
  0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0xfd, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x1c, 0xfe, 0x1c, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x96, 0x54, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xbc, 0x1c, 0xfd, 0x24, 0xfe, 0x1c, 0xfd, 0x24, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x25, 0x56, 0x4c, 0x66, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x1c, 0xfe, 0x1c, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x5b, 0x14, 0xdc, 0x24, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfe, 0x1c, 0xfd, 0x1c, 0x97, 0x54, 0x86, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87,
  0x00, 0x77, 0x4c, 0xdc, 0x1c, 0xfd, 0x1c, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0xfc, 0x2c, 0xf8, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x1c, 0xfd, 0x24, 0xfe, 0x1c, 0xfd,
  0x1c, 0xfe, 0x1c, 0x1d, 0x25, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d,
  0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe,
  0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c, 0x1d, 0x1d, 0xfe, 0x1c,
  0x1d, 0x1d, 0xfd, 0x1c, 0xdc, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9b, 0x14, 0xdd, 0x1c, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e,
  0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d,
  0x1d, 0x1d, 0xdd, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1e, 0x1d, 0xfd, 0x1c, 0xfe, 0x14, 0x1e, 0x1d, 0x3e, 0x1d, 0x1e, 0x1d,
  0x3e, 0x1d, 0x1e, 0x1d, 0x3e, 0x1d, 0x1e, 0x1d, 0x3e, 0x1d, 0x1e, 0x1d, 0x3e,
  0x1d, 0x1e, 0x1d, 0x3e, 0x1d, 0x1e, 0x1d, 0x1e, 0x1d, 0xfe, 0x1c, 0x1e, 0x1d,
  0xdd, 0x1c, 0x97, 0x4c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x35, 0x44, 0xdd, 0x1c,
  0x1e, 0x1d, 0x1d, 0x25, 0x1e, 0x1d, 0x1e, 0x25, 0x3e, 0x1d, 0x1e, 0x25, 0x3e,
  0x1d, 0x1e, 0x25, 0x1e, 0x1d, 0xfd, 0x24, 0xfd, 0x1c, 0xdb, 0x24, 0x5a, 0x14,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00};

// Accelerometer
#if defined(ACCEL_LIS2DW12)
#include <LIS2DW12Sensor.h>
#define ACCEL_ADDR 0x31U
LIS2DW12Sensor* accel;
#elif defined(ACCEL_MMA8452Q)
#include <SparkFun_MMA8452Q.h>
#define ACCEL_ADDR 0x1C
MMA8452Q accel(ACCEL_ADDR);
#endif

// Used for frames-per-second throttle
uint32_t prevTime = 0;

uint32_t frameCounter = 0;
uint32_t lastFrameMessage = 0;

int begin_retval;

void err(int x) {
  uint8_t i;
  pinMode(LED_BUILTIN, OUTPUT);       // Using onboard LED
  for(i=1;;i++) {                     // Loop forever...
    digitalWrite(LED_BUILTIN, i & 1); // LED on/off blink to alert user
    delay(x);
  }
}

void setup(void) {
  Wire.begin();

  if(!sand.begin()) err(1000); // Slow blink = malloc error

#if defined(ACCEL_LIS2DW12)
  accel = new LIS2DW12Sensor(&Wire, ACCEL_ADDR);
  if (!accel->begin()) err(250);
  accel->Enable_X();
#elif defined(ACCEL_MMA8452Q)
  if (!accel.begin(Wire, ACCEL_ADDR)) err(250);
#endif

  Serial.begin(SERIAL_DATA_RATE);
  tft.begin(SPI_SPEED);

  // Max is 700000 in 160MHz mode and 400000 in 80MHz mode
  Wire.setClock(700000); // Run I2C at 700 KHz for faster screen updates

  // Clear the display
  displayReset();

  // Draw an obstacle for sand to move around
  dimension_t larryStartY = (SCREEN_HEIGHT - larryHeight) / 2;
  dimension_t larryStartX = (SCREEN_WIDTH - larryWidth) / 2;
  for (uint8_t y = 0; y < larryHeight; y++) {
    for (uint8_t x = 0; x < larryWidth; x++) {
      uint16_t color = ((uint16_t *)larry)[y * larryWidth + x];
      if (color > 0) {
        sand.setPixel(larryStartX + x, larryStartY + y);             // Set pixel in the simulation
      }
      tft.drawPixel(larryStartX + x, larryStartY + y, color);
    }
  }

  // Create some colors
  for (int i = 0; i < N_GRAINS; i ++) {
    colors[i] = randomColor();
  }

  sand.randomize(); // Initialize random sand positions

  lastFrameMessage = micros();
}

void loop() {
  // Limit the animation frame rate to MAX_FPS. Because the subsequent sand
  // calculations are non-deterministic (don't always take the same amount
  // of time, depending on their current states), this helps ensure that
  // things like gravity appear constant in the simulation.
  uint32_t t;
  while(((t = micros()) - prevTime) < (1000000L / MAX_FPS));
  prevTime = t;

  uint8_t i;
  dimension_t x, y;

  // Display frame calculated on the prior pass.  It's done immediately after the
  // FPS sync (rather than after calculating) for consistent animation timing.
  tft.startWrite();
  for(i = 0; i < N_GRAINS; i++) {
    // Clear the old position of the grain
    tft.setAddrWindow(grains[i].x, grains[i].y, 1, 1);
    tft.SPI_WRITE16(BLACK);

    sand.getPosition(i, &x, &y);

    // Write the new position of the pixel
    tft.setAddrWindow(x, y, 1, 1);
    tft.SPI_WRITE16(colors[i]);

    // Save the grain position so we can erase it after the next iteration
    grains[i] = {x, y};
  }
  tft.endWrite();

  // Read accelerometer and run the physics

#if defined(ACCEL_LIS2DW12)
  int16_t axes[3];
  accel->Get_X_AxesRaw(axes);
  sand.iterate((int)-axes[0], (int)-axes[1], (int)-axes[2]);
#elif defined(ACCEL_MMA8452Q)
  // Wait for the accelerometer to become ready
  while (!accel.available()) {}
  // Read accelerometer and run the physics
  accel.read();
  sand.iterate((int)(-accel.x), (int)(-accel.y), (int)(accel.z));
#endif

  // Log the FPS for debugging once per second
  frameCounter++;
  if ((t - lastFrameMessage) > 1000000L) {
    DLOG("FPS: %f\n", (float)frameCounter/((t - lastFrameMessage)/1000000.0));
    frameCounter = 0;
    lastFrameMessage = t;
  }
}
