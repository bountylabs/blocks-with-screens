// set SERIALLOG to 1 to enable serial logging
// set TELNETLOG to 1 to enable telnet logging
// see DLog.h
// NOTE: Enabling logging will significantly impact performance of this blockware
#define SERIALLOG 1
#include <DLog.h>

#include <Adafruit_SSD1351.h>
#include <math.h>

#include <Colors.h>
#include <Random.h>

#include "Alphabets.h"
#include "Canvas.h"
#include "MatrixRain.h"

const char* UNKNOWN = "?";
const float RESTART_CHANCE = 0.75;
const float WHITE_RANDOM_CHANGE = 0.20;
const float GREEN_RANDOM_CHANGE = 0.01;
const uint8_t VISIBLE_DARKNESS = 0;
const uint8_t NOT_VISIBLE_DARKNESS = 255;
const uint8_t MIN_WHITE_THRESHOLD = NOT_VISIBLE_DARKNESS / 32;
const uint8_t MAX_WHITE_THRESHOLD = NOT_VISIBLE_DARKNESS / 8;
const uint8_t MIN_DARKNESS_RATE = 4;
const uint8_t MAX_DARKNESS_RATE = 8;

Vec2d<uint8_t> MatrixRain::cell = Vec2d<uint8_t>(
  std::max(Alphabets::hiragana_textSize.x, Alphabets::alphabet_textSize.x),
  std::max(Alphabets::hiragana_textSize.y, Alphabets::alphabet_textSize.y));

Vec2d<uint8_t> centerCellVec2d(const Vec2d<uint8_t>& relativeVector)
{
  return Vec2d<uint8_t>((MatrixRain::cell.x / 2) - (relativeVector.x / 2),
                        (MatrixRain::cell.y / 2) - (relativeVector.y / 2));
}

uint8_t rand_white_darkness()
{
  return (uint8_t)time_random(MIN_WHITE_THRESHOLD, MAX_WHITE_THRESHOLD);
}

uint8_t rand_darkness_rate()
{
  return (uint8_t)time_random(MIN_DARKNESS_RATE, MAX_DARKNESS_RATE);
}

// can be used too ensure we do not overflow integer values
// stops at specified max, otherwise increments
template <typename T>
void safe_bounded_inc(T& value, int increment, int max)
{
  int delta = max - value;
  if (delta > increment) {
    value += increment;
    return;
  }

  value = max;
}

void MatrixRain::init(uint8_t _column, int _start_x, uint8_t _white_darkness, uint8_t _darkness_rate)
{
  active = true;
  column = _column;

  x = _start_x + (_column * cell.x);
  y = 0;

  white_darkness = _white_darkness;
  darkness_rate = _darkness_rate;

  // build vectors of letters and darkness values
  letter_vec = {};
  darkness_vec = {};
  center_vec = {};
  // build enough to the screen height, plus 2 extra for runway outside visible window
  int letterCount = (Canvas::canvas->height() / cell.y) + 2;
  for (int i = 0; i < letterCount; i++) {
    // allocate space then immediately replace with random letter
    letter_vec.push_back(UNKNOWN);
    center_vec.push_back(centerCellVec2d(Alphabets::alphabet_textSize));
    this->randomizeLetter(i);
    // ensure letters are full darkness (not visible) to start
    darkness_vec.push_back(NOT_VISIBLE_DARKNESS);
  }
}

MatrixRain::MatrixRain(uint8_t _column, int _start_x)
{
  init(_column, _start_x, rand_white_darkness(), rand_darkness_rate());
}

void MatrixRain::toggleActive()
{
  active = !active;
}

void MatrixRain::activateCell(uint8_t cell)
{
  next_cell = cell;
  darkness_vec[next_cell] = VISIBLE_DARKNESS;
  // DLOG("\n[MatrixRain#%02d] activateCell [cell=%02d]", column, cell);
}

void MatrixRain::randomizeLetter(uint8_t index)
{
  // weight all letters equally for now by selecting a random index between the two lengths
  int letterIndex = time_random(0, Alphabets::alphabet_length + Alphabets::hiragana_length);

  if (letterIndex < Alphabets::alphabet_length) {
    letter_vec[index] = Alphabets::alphabet[letterIndex];
    center_vec[index] = centerCellVec2d(Alphabets::alphabet_textSize);
    return;
  }

  letterIndex = letterIndex - Alphabets::alphabet_length;
  if (letterIndex < Alphabets::hiragana_length) {
    letter_vec[index] = Alphabets::hiragana[letterIndex];
    center_vec[index] = centerCellVec2d(Alphabets::hiragana_textSize);
    return;
  }

  DLOG("ERROR [MatrixRain] Invalid random letterIndex [%d] ", letterIndex);
  letter_vec[index] = UNKNOWN;
  center_vec[index] = centerCellVec2d(Alphabets::alphabet_textSize);
}

void MatrixRain::drawLetter(uint8_t index)
{
  const char* letter = letter_vec[index];
  uint8_t darkness = darkness_vec[index];
  Vec2d<uint8_t> center = center_vec[index];

  // if darkness above white_darkness threshold, show as white
  bool isWhite = darkness < white_darkness;
  uint16_t color = isWhite ? WHITE : NEON_GREEN;

  Canvas::canvas->setTextColor(darken(color, darkness));

  int letter_x = x + center.x;
  int letter_y = y + (cell.y * index) + center.y;
  Canvas::canvas->setCursor(letter_x, letter_y);
  Canvas::canvas->printUTF8((char*)letter);
}

void MatrixRain::reset(int _x, int _y)
{
  x = _x;
  y = _y;
  for (std::size_t i = 0; i < darkness_vec.size(); i++) {
    darkness_vec[i] = NOT_VISIBLE_DARKNESS;
  }
}

void MatrixRain::draw()
{
  // skip columns outside visible canvas bounds
  if (x <= -1 * cell.x || (x + (0.75 * cell.x)) >= Canvas::canvas->width()) {
    // DLOG("\n[MatrixRain#%02d] [(%d, %d)] SKIP", column, x, y);
    return;
  }

  // DLOG("\n[MatrixRain#%02d] [(%d, %d)] DRAW", column, x, y);

  for (std::size_t i = 0; i < letter_vec.size(); i++) {
    this->drawLetter(i);
  }
}

void MatrixRain::debug(const char* message)
{
  DLOG(
    "\n\n[MatrixRain#%02d] [%s] [white_darkness=%03d, darkness_rate=%02d, "
    "next_cell=%02d]",
    column, message, white_darkness, darkness_rate, next_cell);
  for (std::size_t i = 0; i < darkness_vec.size(); i++) {
    DLOG("\n  [cell=%02d, darkness=%03d]", i, darkness_vec[i]);
  }
}

void MatrixRain::pan(Vec2d<int8_t>& pan)
{
  x += pan.x;
  y += pan.y;

  // if y is outside bounds, shift and interpolate darkness on new shifted letter slot
  // e.g.
  // shifting up, insert at bottom with darkness = last darkness + increment
  // shifting down, insert at top with darkness = first darkness - increment
  if (y + cell.y < 0) {
    // this->debug("ShiftUp::BEFORE");

    for (std::size_t i = 0; i < letter_vec.size() - 1; i++) {
      letter_vec[i] = letter_vec[i + 1];
      darkness_vec[i] = darkness_vec[i + 1];
      center_vec[i] = center_vec[i + 1];
    }
    // darkness_vec[darkness_vec.size() - 1] = darkness_vec[darkness_vec.size() - 2] - darkness_rate;
    darkness_vec[darkness_vec.size() - 1] = darkness_vec[darkness_vec.size() - 1];
    if (darkness_vec[darkness_vec.size() - 1] != NOT_VISIBLE_DARKNESS) {
      safe_bounded_inc(darkness_vec[darkness_vec.size() - 1], -1 * darkness_rate, 0);
      if (darkness_vec[darkness_vec.size() - 1] <= white_darkness) {
        darkness_vec[darkness_vec.size() - 1] = NOT_VISIBLE_DARKNESS;
      }
    }

    if (next_cell > 0 && next_cell < darkness_vec.size() - 1) {
      next_cell -= 1;
    }
    this->randomizeLetter(darkness_vec.size() - 1);
    y = 0;

    // this->debug("ShiftUp::AFTER");
  }
  else if (y > cell.y) {
    // this->debug("ShiftDown::BEFORE");

    for (std::size_t i = letter_vec.size() - 1; i > 0; i--) {
      letter_vec[i] = letter_vec[i - 1];
      darkness_vec[i] = darkness_vec[i - 1];
      center_vec[i] = center_vec[i - 1];
    }
    // darkness_vec[0] = darkness_vec[1] + darkness_rate;
    darkness_vec[0] = darkness_vec[1];
    if (darkness_vec[0] <= white_darkness) {
      darkness_vec[0] = white_darkness;
    }
    safe_bounded_inc(darkness_vec[0], darkness_rate, NOT_VISIBLE_DARKNESS);
    this->randomizeLetter(0);
    next_cell += 1;
    y = 0;

    // this->debug("ShiftDown::AFTER");
  }
}

void MatrixRain::light()
{
  bool rainVisible = false;

  // for each letter
  for (int i = 0; i < (int)letter_vec.size(); i++) {
    // increment darkness for all letters, but do not overflow
    // effect is visible letters slowly fade out
    if (darkness_vec[i] < NOT_VISIBLE_DARKNESS) {
      rainVisible = true;

      // increment darkness (fade out)
      // ensure we do not overflow darkness values
      // overflow will skip stop condition (darkness == 255)
      // resulting in infinite loop of same speed since the stop condition (!rainVisible) will never become true
      safe_bounded_inc(darkness_vec[i], darkness_rate, NOT_VISIBLE_DARKNESS);
      // DLOG("\n[MatrixRain#%02d] [cell=%02d, darkness=%03d]", column, i, darkness_vec[i]);
    }

    // update rain by advancing white letters (rain droplet)
    // if this cell is at white_darkness threshold (dark enough)
    // then light up the next cell
    bool currentCellDarkEnough = darkness_vec[i] >= white_darkness;
    bool nextCellNotVisible = darkness_vec[i + 1] == NOT_VISIBLE_DARKNESS;
    if (next_cell == i && currentCellDarkEnough && nextCellNotVisible) {
      this->activateCell(i + 1);
    }

    bool isWhite = darkness_vec[i] <= white_darkness;
    float randomLetterChance = isWhite ? WHITE_RANDOM_CHANGE : GREEN_RANDOM_CHANGE;
    if (randomf() < randomLetterChance) {
      this->randomizeLetter(i);
    }
  }

  // if all letters are not visible (darkness 255)
  // then random chance to set first letter to 0 darkness (visible and white)
  // this starts the cycle over again
  if (!rainVisible) {
    // DLOG("\n[MatrixRain#%02d] !rainVisible", column);

    if (randomf() <= RESTART_CHANCE) {
      white_darkness = rand_white_darkness();
      darkness_rate = rand_darkness_rate();
      this->activateCell(0);
      // DLOG(
      //   "\n[MatrixRain#%02d] RESTART [white_darkness=%03d,
      //   darkness_rate=%02d]", column, white_darkness, darkness_rate);
    }
  }
}

void MatrixRain::tick(Vec2d<int8_t>& pan)
{
  // do nothing when not active
  if (!active) {
    return;
  }

  // handle the pan vector by translating entire column
  this->pan(pan);
  // handle lighting and darkening cells in column
  this->light();
}
