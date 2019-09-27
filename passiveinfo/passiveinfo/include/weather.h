#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1351.h>

float convertKelvinToCelsius(float kelvinTemp);

float convertKelvinToFahrenheit(float kelvinTemp);

String getWeatherUrlForZipCountry(String zip, String country);

void drawWeather(
  Adafruit_SSD1351 screen,
  String cityName,
  String iconUrl,
  float currentTempF
);