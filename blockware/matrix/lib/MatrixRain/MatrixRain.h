#pragma once

#include <Adafruit_SSD1351.h>
#include <vector>

#include <Vec2d.h>

class MatrixRain {
private:
  // whether this instance is currently active
  bool active;
  // column acts as an identifier for this column, e.g. 4
  uint8_t column;
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
  void debug(const char* message);
  void pan(Vec2d<int8_t>& pan);
  void light();

protected:
public:
  static Vec2d<uint8_t> cell;

  // cartesian location of this column
  // shifts via pan vector passed in tick
  int x, y;

  void init(uint8_t column, int start_x, uint8_t white_darkness, uint8_t darkness_rate);
  MatrixRain(uint8_t column, int start_x);
  void toggleActive(void);
  void draw();
  void tick(Vec2d<int8_t>& pan);
  void reset(int x, int y);
};
