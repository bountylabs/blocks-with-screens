#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#include "constants.h"
#include "secrets.h"

float convertKelvinToCelsius(float kelvinTemp);
float convertKelvinToFahrenheit(float kelvinTemp);

String getWeatherAppIdUrl() {
  return String(OPEN_WEATHER_APPID) + String(OPEN_WEATHER_API_KEY);
}

String getWeatherUrlForZipCountry(String zip, String country) {
  return String(WEATHER_ZIP_PATH) + zip;
}

float convertKelvinToCelsius(float kelvinTemp) {
  return kelvinTemp - KELVIN_AT_0_C;
}

float convertKelvinToFahrenheit(float kelvinTemp) {
  return ((kelvinTemp - KELVIN_AT_0_C) * 9 / 5) + 32;
}

void drawWeather(
  Adafruit_SSD1351 screen,
  String cityName,
  String iconUrl,
  float currentTempF
) {
  char charBuffer[128];
  sprintf(charBuffer, "%s", cityName.c_str());

  int16_t ignoredX;
  int16_t ignoredY;
  uint16_t cityNameWidth;
  uint16_t cityNameHeight;
  screen.getTextBounds(
    charBuffer,
    0,
    0,
    &ignoredX,
    &ignoredY,
    &cityNameWidth,
    &cityNameHeight
  );

  uint8_t cityNameX = (screen.width() - cityNameWidth) / 2;
  screen.setTextColor(BLACK);
  screen.setCursor(cityNameX, 10);
  screen.setTextSize(1.5);
  screen.print(cityName);

  sprintf(charBuffer, "%.1f%cF", currentTempF, (char)247);

  uint16_t currentTempWidth;
  uint16_t currentTempHeight;
  screen.getTextBounds(
    charBuffer,
    0,
    0,
    &ignoredX,
    &ignoredY,
    &currentTempWidth,
    &currentTempHeight
  );

  uint8_t currentTempX = (screen.width() - currentTempWidth) / 2;
  uint8_t currentTempY = screen.height() - currentTempHeight - 10;
  screen.setCursor(currentTempX, currentTempY);
  screen.setTextSize(2);
  screen.print(charBuffer);
}
