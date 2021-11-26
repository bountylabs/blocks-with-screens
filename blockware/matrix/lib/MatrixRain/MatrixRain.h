#pragma once

#include <Adafruit_SSD1351.h>
#include <vector>

#include <Vec2d.h>

class MatrixRain {
private:
  // reference to the drawing canvas, used by draw functions
  GFXcanvas16* canvas;
  // whether this instance is currently active
  bool active;
  // column acts as an identifier for this column, e.g. 4
  uint8_t column;
  // cartesian location of this column
  // shifts via pan vector passed in tick
  uint8_t x, y;
  // darkness level at which letter changes from white to green
  // also controls the rate at which new letters form (only tip is white)
  uint8_t white_darkness;
  // darkness increase rate, controls how quickly letters fade to black
  uint8_t darkness_rate;
  // the vector of all letters in this column
  std::vector<const char*> letter_vec;
  // the vector of all darkness values for this column
  std::vector<uint8_t> darkness_vec;
  // store whether this cell is fading out
  uint8_t next_cell;
  // offset to ensure this letter is centered
  std::vector<Vec2d<uint8_t>> center_vec;

  void activateCell(uint8_t cell);
  void randomizeLetter(uint8_t index);
  void drawLetter(uint8_t letter);

protected:
public:
  static Vec2d<uint8_t> cell;

  void init(GFXcanvas16* canvas, uint8_t column, uint8_t white_darkness, uint8_t darkness_rate);
  MatrixRain(GFXcanvas16* canvas, uint8_t column, uint8_t white_darkness, uint8_t darkness_rate);
  MatrixRain(GFXcanvas16* canvas, uint8_t column);
  void toggleActive(void);
  void draw();
  void tick(Vec2d<uint8_t> &pan);
};
