// set SERIALLOG to 1 to enable serial logging
// set TELNETLOG to 1 to enable telnet logging
// see DLog.h
#define SERIALLOG 1
#include <DLog.h>
#include <Colors.h>
#include <Random.h>

#include "Alphabets.h"
#include "DebugAlphabet.h"

void DebugAlphabet::debug_alphabet(GFXcanvas16* canvas)
{
  int charsPerLine = canvas->width() / Alphabets::alphabet_textSize.x;

  // 0 thru 3 corresponding to 4 cardinal rotations
  // canvas->setRotation(3);
  canvas->setTextSize(1);
  canvas->setTextColor(NEON_GREEN);

  DLOG("\n\n====================================\n\n");

  for (uint8_t i = 0; i < Alphabets::alphabet_length; i++) {
    const char* text = Alphabets::alphabet[i];

    int mx = i % charsPerLine;
    int ln = i / charsPerLine;
    int x = 0 + (mx * Alphabets::alphabet_textSize.x);
    int y = 0 + (ln * Alphabets::alphabet_textSize.y);

    // Example
    // [38] mx=8,ln=3 -> (96, 42)
    // [39] mx=9,ln=3 -> (108, 42)
    // [40] mx=0,ln=4 -> (0, 56)
    // [41] mx=1,ln=4 -> (12, 56)
    // [42] mx=2,ln=4 -> (24, 56)
    // // print just debug information (no text measurement)
    // DLOG("\n[%s] i=%d,mx=%d,ln=%d -> (%d, %d)", text, i, mx, ln, x,
    // y); optionally print text measurements
    int16_t text_x, text_y;
    uint16_t text_width, text_height;
    canvas->getTextBounds(text, 0, 0, &text_x, &text_y, &text_width, &text_height);
    DLOG("\n[%s] i=%d,mx=%d,ln=%d -> %dx%d (%d, %d)", text, i, mx, ln,
                  text_width, text_height, x, y);

    canvas->setCursor(x, y);
    canvas->printUTF8((char*)text);
  }
}

void DebugAlphabet::debug_hiragana(GFXcanvas16* canvas)
{
  int charsPerLine = canvas->width() / Alphabets::hiragana_textSize.x;

  // 0 thru 3 corresponding to 4 cardinal rotations
  // canvas->setRotation(3);
  canvas->setTextSize(1);
  canvas->setTextColor(NEON_GREEN);

  DLOG("\n\n====================================\n\n");

  for (uint8_t i = 0; i < Alphabets::hiragana_length; i++) {
    const char* text = Alphabets::hiragana[i];

    int mx = i % charsPerLine;
    int ln = i / charsPerLine;
    int x = 0 + (mx * Alphabets::hiragana_textSize.x);
    int y = 0 + (ln * Alphabets::hiragana_textSize.y);

    // Example
    // [38] mx=8,ln=3 -> (96, 42)
    // [39] mx=9,ln=3 -> (108, 42)
    // [40] mx=0,ln=4 -> (0, 56)
    // [41] mx=1,ln=4 -> (12, 56)
    // [42] mx=2,ln=4 -> (24, 56)
    // // print just debug information (no text measurement)
    // DLOG("\n[%s] i=%d,mx=%d,ln=%d -> (%d, %d)", text, i, mx, ln, x, y);

    // Example
    // [Z] i=25,mx=5,ln=2 -> 8x16 (60, 28)
    // [ア] i=26,mx=6,ln=2 -> 24x16 (72, 28)
    // optionally print text measurements
    int16_t text_x, text_y;
    uint16_t text_width, text_height;
    canvas->getTextBounds(text, 0, 0, &text_x, &text_y, &text_width, &text_height);
    DLOG("\n[%s] i=%d,mx=%d,ln=%d -> %dx%d (%d, %d)", text, i, mx, ln,
                  text_width, text_height, x, y);

    canvas->setCursor(x, y);
    canvas->printUTF8((char*)text);
  }
}
