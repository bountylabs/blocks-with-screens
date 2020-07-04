// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

// Pin setup
#define SCLK_PIN 14 // D5 -- ESP8266 Hardware SCLK
#define MOSI_PIN 13 // D7 -- ESP8266 Hardware MOSI

// V1 OLED board pinout
// NOTE: since GPIO12 is the hardware SPI MOSI pin, this board requires a hack in SPI.c.
// In short, find the PlatformIO SPI library and comment out the lines with `pinMode(MISO`.
// This file is usually somewhere in `~/.platformio/packages`, often here:
// ~/.platformio/packages/framework-arduinoespressif8266/libraries/SPI/SPI.h
// #define DC_PIN 12 // D6
// #define CS_PIN 16 // D0
// #define RST_PIN 15 // D8

// V2 OLED board pinout
// #define DC_PIN 5 // D1
// #define CS_PIN 16 // D0
// #define RST_PIN 15 // D8

// V3 OLED board pinout
#define DC_PIN 2 // D4
#define CS_PIN 16  // D0
#define RST_PIN 15 // D8

// Serial data rate (115200 bits per second)
// https://www.arduino.cc/reference/en/language/functions/communication/serial/begin/115200
#define SERIAL_DATA_RATE 115200

// 15MHz SPI for communication with the OLED display
// The ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000
