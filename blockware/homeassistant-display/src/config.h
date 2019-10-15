#include "secrets.h"

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// Pin setup
#define SCLK_PIN 14 // D5 -- ESP8266 Hardware SCLK
#define MOSI_PIN 13 // D7 -- ESP8266 Hardware MOSI
// Note this is also the ESP8266 Hardware MISO pin! That seems to be a bad thing that we should fix in V2.
// See the bit about the hack below
#define DC_PIN   12 // D6
#define CS_PIN   16 // D0
#define RST_PIN  15 // D8

// 15MHz SPI
// Note, the ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000

#define HOME_ASSISTANT_IP "10.0.0.2"
#define HOME_ASSISTANT_PORT 8123
