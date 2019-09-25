#include <Arduino.h>
#include <ArduinoJson.h>

String getWeatherUrlForZipCountry(String zip, String country);

void parseWeatherZipResponse(DynamicJsonDocument* docPtr);