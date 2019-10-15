// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// Pin setup
#define SCLK_PIN 14 // D5 -- ESP8266 Hardware SCLK
#define MOSI_PIN 13 // D7 -- ESP8266 Hardware MOSI
// Note this is also the ESP8266 Hardware MISO pin! That seems to be a bad thing
// that we should fix in V2. See the bit about the hack below
#define DC_PIN 12  // D6
#define CS_PIN 16  // D0
#define RST_PIN 15 // D8

// serial data rate (115200 bits per second)
// https://www.arduino.cc/reference/en/language/functions/communication/serial/begin/115200
#define SERIAL_DATA_RATE 115200

// 15MHz SPI
// Note, the ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000

#define SERIALLOG 1
#include <DLog.h>

#include <Adafruit_SSD1351.h>
#include <SPI.h>

#include "secrets.h"
#include <CurvySnake.h>
#include <OTAUpdates.h>
#include <Text.h>
#include <WiFi.h>

// Software bit-banged SPI mode works just fine but is slow
// Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

// Hardware SPI mode only works if you make a hack in SPI.c under the hood. In short, find the two
// lines with `pinMode(MISO` in them and comment them out
// e.g. /Users/jboiles/.platformio/packages/framework-arduinoespressif8266/libraries/SPI/SPI.h
Adafruit_SSD1351 tft =
  Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

void setup(void)
{
  Serial.begin(SERIAL_DATA_RATE);
  tft.begin(SPI_SPEED);

  // initialize srand
  randomSeed(ESP.getCycleCount());

  displayReset();
  ConnectWifi(WIFI_SSID, WIFI_PASSWORD);
  OTAUpdates_setup("curvy-snake");
  CurvySnake_setup();
}

void loop()
{
  OTAUpdates_handle();
  CurvySnake_loop();
}
