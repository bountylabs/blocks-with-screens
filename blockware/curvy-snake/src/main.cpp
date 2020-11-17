#define SERIALLOG 1
#include <DLog.h>

#include <Adafruit_SSD1351.h>
#include <SPI.h>

#include <CurvySnake.h>
#include <Text.h>
#include <DefaultConfig.h>

Adafruit_SSD1351 tft =
  Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

void setup(void)
{
  Serial.begin(SERIAL_DATA_RATE);
  tft.begin(SPI_SPEED);

  // initialize srand
  randomSeed(ESP.getCycleCount());

  displayReset();

  CurvySnake_setup();
}

void loop()
{
  CurvySnake_loop();
}
