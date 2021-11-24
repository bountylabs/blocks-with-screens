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
  // horizontal location of this column, based on int column provided to constructor
  uint8_t column;
  uint8_t column_x;
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
  int next_cell;
  // offset to ensure this letter is centered
  std::vector<Vec2d<uint8_t>> center_vec;

  void activateCell(int cell);
  void randomizeLetter(int index);
  void drawLetter(int letter);

protected:
public:
  static Vec2d<uint8_t> cell;

  void init(GFXcanvas16* canvas, int column, uint8_t white_darkness, uint8_t darkness_rate);
  MatrixRain(GFXcanvas16* canvas, int column, uint8_t white_darkness, uint8_t darkness_rate);
  MatrixRain(GFXcanvas16* canvas, int column);
  void toggleActive(void);
  void draw();
  void tick();
};
