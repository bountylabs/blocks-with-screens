#include <Adafruit_SSD1351.h>
#include <math.h>

#include <Colors.h>
// set SERIALLOG to 1 to enable serial logging
// set TELNETLOG to 1 to enable telnet logging
// see DLog.h
// NOTE: Enabling logging will significantly impact performance of this blockware
#define SERIALLOG 0
#include <DLog.h>
#include <Random.h>

#include "Alphabets.h"
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

void MatrixRain::init(GFXcanvas16* _canvas, int _column, uint8_t _white_darkness, uint8_t _darkness_rate)
{
  active = true;
  canvas = _canvas;
  column = _column;
  column_x = _column * cell.x;
  white_darkness = _white_darkness;
  darkness_rate = _darkness_rate;

  // build vectors of letters and darkness values
  letter_vec = {};
  darkness_vec = {};
  center_vec = {};
  int letterCount = canvas->height() / cell.y;
  for (int i = 0; i < letterCount; i++) {
    // allocate space then immediately replace with random letter
    letter_vec.push_back(UNKNOWN);
    center_vec.push_back(centerCellVec2d(Alphabets::alphabet_textSize));
    this->randomizeLetter(i);
    // ensure letters are full darkness (not visible) to start
    darkness_vec.push_back(NOT_VISIBLE_DARKNESS);
  }
}

MatrixRain::MatrixRain(GFXcanvas16* _canvas, int _column, uint8_t _white_darkness, uint8_t _darkness_rate)
{
  init(_canvas, _column, _white_darkness, _darkness_rate);
}

MatrixRain::MatrixRain(GFXcanvas16* _canvas, int _column)
{
  init(_canvas, _column, rand_white_darkness(), rand_darkness_rate());
}

void MatrixRain::toggleActive()
{
  active = !active;
}

void MatrixRain::activateCell(int cell)
{
  next_cell = cell;
  darkness_vec[next_cell] = VISIBLE_DARKNESS;
  DLOG("\n[MatrixRain#%02d] activateCell [cell=%02d]", column, cell);
}

void MatrixRain::randomizeLetter(int index)
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

void MatrixRain::drawLetter(int index)
{
  const char* letter = letter_vec[index];
  uint8_t darkness = darkness_vec[index];
  Vec2d<uint8_t> center = center_vec[index];

  // if darkness above white_darkness threshold, show as white
  bool isWhite = darkness < white_darkness;
  uint16_t color = isWhite ? WHITE : NEON_GREEN;

  canvas->setTextColor(darken(color, darkness));

  int x = column_x + center.x;
  int y = (cell.y * index) + center.y;
  canvas->setCursor(x, y);
  canvas->printUTF8((char*)letter);
}

void MatrixRain::draw()
{
  for (std::size_t i = 0; i < letter_vec.size(); i++) {
    this->drawLetter(i);
  }
}

void MatrixRain::tick()
{
  // do nothing when not active
  if (!active)
    return;

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
      uint8_t delta = NOT_VISIBLE_DARKNESS - darkness_vec[i];
      if (delta > darkness_rate) {
        darkness_vec[i] += darkness_rate;
      }
      else {
        darkness_vec[i] = NOT_VISIBLE_DARKNESS;
      }

      DLOG("\n[MatrixRain#%02d] [cell=%02d, darkness=%03d]", column, i, darkness_vec[i]);
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
    DLOG("\n[MatrixRain#%02d] !rainVisible", column);

    if (randomf() <= RESTART_CHANCE) {
      white_darkness = rand_white_darkness();
      darkness_rate = rand_darkness_rate();
      this->activateCell(0);
      DLOG(
        "\n[MatrixRain#%02d] RESTART [white_darkness=%03d, darkness_rate=%02d]",
        column, white_darkness, darkness_rate);
    }
  }
}
