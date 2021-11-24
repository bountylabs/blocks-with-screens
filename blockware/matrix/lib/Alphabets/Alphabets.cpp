#include "Alphabets.h"

const char* Alphabets::alphabet[] PROGMEM = {
  // Symbols
  "!", "?", "@", "#", "$", "%", "&", "*", "+", "-", ":", "<", ">", ";",
  // Numbers
  "0",  "1",  "2",  "3",  "4",  "5", "6",  "7",  "8",  "9",
  // Latin
  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
  "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
  // Halfwidth Katakana
  "ｦ", "ｧ", "ｨ", "ｩ", "ｪ", "ｫ", "ｬ", "ｭ", "ｮ", "ｯ",
  "ｰ", "ｱ", "ｲ", "ｳ", "ｴ", "ｵ", "ｶ", "ｷ", "ｸ", "ｹ", "ｺ", "ｻ", "ｼ", "ｽ", "ｾ", "ｿ",
  "ﾀ", "ﾁ", "ﾂ", "ﾃ", "ﾄ", "ﾅ", "ﾆ", "ﾇ", "ﾈ", "ﾉ", "ﾊ", "ﾋ", "ﾌ", "ﾍ", "ﾎ", "ﾏ",
  "ﾐ", "ﾑ", "ﾒ", "ﾓ", "ﾔ", "ﾕ", "ﾖ", "ﾗ", "ﾘ", "ﾙ", "ﾚ", "ﾛ", "ﾜ", "ﾝ",
};

const char* Alphabets::hiragana[] PROGMEM = {
  // Hiragana
  // https://www.unicode.org/charts/PDF/U3040.pdf
  "ぁ", "あ", "ぃ", "い", "ぅ", "う", "ぇ", "え", "ぉ", "お", "か", "が", "き",
  "ぎ", "く", "ぐ", "け", "げ", "こ", "ご", "さ", "ざ", "し", "じ", "す", "ず",
  "せ", "ぜ", "そ", "ぞ", "た", "だ", "ち", "ぢ", "っ", "つ", "づ", "て", "で",
  "と", "ど", "な", "に", "ぬ", "ね", "の", "は", "ば", "ぱ", "ひ", "び", "ぴ",
  "ふ", "ぶ", "ぷ", "へ", "べ", "ぺ", "ほ", "ぼ", "ぽ", "ま", "み", "む", "め",
  "も", "ゃ", "や", "ゅ", "ゆ", "ょ", "よ", "ら", "り", "る", "れ", "ろ", "ゎ",
  "わ", "ゐ", "ゑ", "を", "ん", "ゔ", "ゕ", "ゖ", "ゝ", "ゞ", "ゟ",
};


// find length of array using pointer arithmetic
// since we have a pointer at the start of the array, the length of the​ array can be calculated
// let A = arr        the address of the start of the array
// let B = &arr + 1   points to the memory address right after the end of the array
// let C = *B         casts the above address to an int *
// subtracting A from C gives the length of the array
// int length = *(&arr + 1) - arr;
const int Alphabets::alphabet_length = *(&alphabet + 1) - alphabet;
const int Alphabets::hiragana_length = *(&hiragana + 1) - hiragana;

const Vec2d<uint8_t> Alphabets::alphabet_textSize = Vec2d<uint8_t>(8, 12);
const Vec2d<uint8_t> Alphabets::hiragana_textSize = Vec2d<uint8_t>(12, 14);
