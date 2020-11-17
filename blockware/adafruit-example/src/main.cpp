#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include "adafruit_benchmark.h"
#include <DefaultConfig.h>

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

void setup(void) {
  Serial.begin(SERIAL_DATA_RATE);
  Serial.printf("Starting adafruit-example with SPI speed %d", SPI_SPEED);
  tft.begin(SPI_SPEED);
}

void loop() {
  uint16_t start = millis();

  runAdafruitBenchmark();
  
  uint16_t time = millis() - start;
  Serial.printf("done in %dms\n", time);
  delay(1000);
}
