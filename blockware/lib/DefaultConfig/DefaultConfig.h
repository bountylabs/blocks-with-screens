// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

// Pin setup
#define SCLK_PIN 7
#define MOSI_PIN 8

// V1 OLED board pinout
// NOTE: since GPIO12 is the hardware SPI MOSI pin, this board requires a hack in SPI.cpp.
// In short, find the PlatformIO SPI library and comment out the lines with `pinMode(MISO`.
// This file is usually somewhere in `~/.platformio/packages`, often here:
// ~/.platformio/packages/framework-arduinoespressif8266/libraries/SPI/SPI.cpp
// #define DC_PIN 12 // D6
// #define CS_PIN 16 // D0
// #define RST_PIN 15 // D8

// V2 OLED board pinout
// #define DC_PIN 5 // D1
// #define CS_PIN 16 // D0
// #define RST_PIN 15 // D8

// V3 OLED board pinout
// #define DC_PIN 2 // D4
// #define CS_PIN 16  // D0
// #define RST_PIN 15 // D8

// Link board pinout
#define DC_PIN 5
#define CS_PIN 6
#define RST_PIN 4

#define SCL_PIN 18
#define SDA_PIN 1

// Serial data rate (115200 bits per second)
// https://www.arduino.cc/reference/en/language/functions/communication/serial/begin/115200
#define SERIAL_DATA_RATE 115200

// 15MHz SPI for communication with the OLED display
// The ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000

// V4 Microcontroller board accelerometer
// #define ACCEL_LIS2DW12
// V2 and V3 Microcontroller board accelerometer
#define ACCEL_MMA8452Q

#define BUTTON_UP 9
#define BUTTON_DOWN 11
#define BUTTON_LEFT 12
#define BUTTON_RIGHT 10

#define BUTTON_A 14
#define BUTTON_B 13

#define BUTTON_START 0
#define BUTTON_SELECT 17
