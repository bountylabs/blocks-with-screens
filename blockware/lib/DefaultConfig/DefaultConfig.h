// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

// Pin setup
#define SCLK_PIN 14 // D5 -- ESP8266 Hardware SCLK
#define MOSI_PIN 13 // D7 -- ESP8266 Hardware MOSI

// V3 OLED board pinout
#define DC_PIN 2 // D4
#define CS_PIN 16  // D0
#define RST_PIN 15 // D8

// Serial data rate (115200 bits per second)
// https://www.arduino.cc/reference/en/language/functions/communication/serial/begin/115200
#define SERIAL_DATA_RATE 115200

// 15MHz SPI
// Note, the ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000
