#ifndef COLORS_H
#define COLORS_H

#include <algorithm>
#include <stdint.h>
#include <Random.h>
#include <math.h>

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

// logo colors
#define LOGO_BLUE 0x1e1d
#define LOGO_GREEN 0x1dec
#define LOGO_ORANGE 0xf2e4
#define LOGO_PURPLE 0x787a
#define LOGO_RED 0xe10c
#define LOGO_YELLOW 0xfd64

#define RGB565(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

static inline int randomColor()
{
  // random color
  const int COLORS[] = {BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
  int colorIndex = floor((sizeof(COLORS)/sizeof(int)) * randomf());
  return COLORS[colorIndex];
}

static inline int randomLogoColor()
{
  // random logo color
  const int COLORS[] = {LOGO_BLUE, LOGO_GREEN, LOGO_ORANGE, LOGO_PURPLE, LOGO_RED, LOGO_YELLOW};
  int colorIndex = floor((sizeof(COLORS)/sizeof(int)) * randomf());
  return COLORS[colorIndex];
}

// Takes a 5-6-5 RGB and returns the red component
static uint8_t getRedRaw(uint16_t color)
{
  return (color & 0xF800) >> 11;
}

// Takes a 5-6-5 RGB and returns the green component as a value from 0-255
static uint8_t getRed(uint16_t color)
{
  return getRedRaw(color) * 256 / 32;
}

// Takes a 5-6-5 RGB and returns the green component
static uint8_t getGreenRaw(uint16_t color)
{
  return (color & 0x07E0) >> 5;
}

// Takes a 5-6-5 RGB and returns the green component as a value from 0-255
static uint8_t getGreen(uint16_t color)
{
  return getGreenRaw(color) * 256 / 64;
}

// Takes a 5-6-5 RGB and returns the blue component
static uint8_t getBlueRaw(uint16_t color)
{
  return (color & 0x001F);
}

// Takes a 5-6-5 RGB and returns the green component as a value from 0-255
static uint8_t getBlue(uint16_t color)
{
  return getBlueRaw(color) * 256 / 32;
}

// Makes a 5-6-5 RGB value from its RGB components
static uint16_t mkColor(uint16_t red, uint16_t green, uint16_t blue)
{
  uint16_t r = (red << 11) & 0xF800;
  uint16_t g = (green << 5) & 0x07E0;
  uint16_t b = (blue) & 0x001F;
  return r | g | b;
}

// Get the luminence of a color as an 8 bit value
static uint8_t luminence(uint16_t color)
{
  // Calculate luminence according to the W3C spec
  uint8_t r = getRed(color) * 3 / 10; // 0.3
  uint8_t g = getGreen(color) * 59 / 100; // 0.59
  uint8_t b = getBlue(color) * 11 / 100; // 0.11

  // Sum and clip to max value of 255
  return std::min(0x00FF, (r + g + b));
}

// Applies the luminence value to the given color if it's less than the color's luminence
static uint16_t lumDarken(uint16_t color, uint8_t lum)
{
  int diff = lum - luminence(color);
  if (diff >= 0) return color; // clip to color lum

  uint8_t rt = std::max(0, getRed(color) + diff);
  uint8_t gt = std::max(0, getGreen(color) + diff);
  uint8_t bt = std::max(0, getBlue(color) + diff);
  return RGB565(rt, gt, bt);
}

// Picks the darker luminence of bg and src, and applies it to src.
static uint16_t colorize(uint16_t bg, uint16_t src)
{
  uint8_t lum = luminence(bg);
  return lumDarken(src, lum);
}

// Darkens the color by the given factor out of 255. Higher value is darker.
static uint16_t darken(uint16_t color, uint8_t factor)
{
  uint8_t nomin = 0xFF - factor;
  uint16_t rt = getRedRaw(color) * nomin / 0xFF;
  uint16_t gt = getGreenRaw(color) * nomin / 0xFF;
  uint16_t bt = getBlueRaw(color) * nomin / 0xFF;
  return mkColor(rt, gt, bt);
}

// Mixes two colors according to a step ratio from fromColor to toColor.
static uint16_t getColorMix(uint16_t fromColor, uint16_t toColor, int totalSteps, int currStep)
{
  int fromRed = getRedRaw(fromColor);
  int fromGreen = getGreenRaw(fromColor);
  int fromBlue = getBlueRaw(fromColor);

  int redDiff = ((int)getRedRaw(toColor) - fromRed) * currStep / totalSteps;
  int greenDiff = ((int)getGreenRaw(toColor) - fromGreen) * currStep / totalSteps;
  int blueDiff = ((int)getBlueRaw(toColor) - fromBlue) * currStep / totalSteps;

  uint16_t nextRed = fromRed + redDiff;
  uint16_t nextGreen = fromGreen + greenDiff;
  uint16_t nextBlue = fromBlue + blueDiff;

  return mkColor(nextRed, nextGreen, nextBlue);
}

#endif
