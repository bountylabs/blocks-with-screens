/***************************************************
  Example sketch for the Block with Screen
 ****************************************************/

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// Pin setup
#define SCLK_PIN 14 // D5 -- ESP8266 Hardware SCLK
#define MOSI_PIN 13 // D7 -- ESP8266 Hardware MOSI
// Note this is also the ESP8266 Hardware MISO pin! That seems to be a bad thing that we should fix in V2.
// See the bit about the hack below
#define DC_PIN 12  // D6
#define CS_PIN 16  // D0
#define RST_PIN 15 // D8

// 15MHz SPI
// Note, the ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <stdlib.h>
#include "90fps.h"
#include <OTAUpdates.h>
#include "secrets.h"

// Software bit-banged SPI mode works just fine but is slow
// Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

// Hardware SPI mode only works if you make a hack in SPI.c under the hood. In short, find the two
// lines with `pinMode(MISO` in them and comment them out
// e.g. /Users/jboiles/.platformio/packages/framework-arduinoespressif8266/libraries/SPI/SPI.h
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// in-memory 16-bit canvas
GFXcanvas16 *canvas;

uint16_t lastLoop;

void flush()
{
  tft.drawRGBBitmap(0, 0, (uint16_t *)canvas->getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
  //tft.startWrite();
  // SPI.writeBytes((uint8_t *)canvas->getBuffer(), 128 * 128 * 2);
  //tft.endWrite();
}

void setup(void)
{
  Serial.begin(115200);
  Serial.print("setup");
  // 15MHz SPI
  tft.begin(15000000);

  OTAUpdates_setup("curvy-snake", WIFI_SSID, WIFI_PASSWORD);

  // initialize canvas
  canvas = new GFXcanvas16(SCREEN_WIDTH, SCREEN_HEIGHT);

  // reset canvas to all black
  canvas->fillScreen(0x0000);
  flush();

  lastLoop = millis();
}

void loop()
{
  OTAUpdates_handle();

  uint16_t now = millis();
  uint16_t time = now - lastLoop;

  // every ~16ms (60fps)
  if (time > 16)
  {
    // track loop millis to keep steady fps
    lastLoop = now;

    // run!
    run90FPS(canvas);

    // flush our in-memory canvas to the screen
    flush();
  }
  // Serial.printf("frame %dms\n", time);

  // // run!
  // run90FPS(canvas);

  // // flush our in-memory canvas to the screen
  // flush();
}
