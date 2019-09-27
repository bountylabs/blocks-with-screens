#include <ArduinoJson.h>

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

void parseWeatherZipResponse(DynamicJsonDocument* docPtr) {
  DynamicJsonDocument doc = *docPtr;
  const char* cityName = doc["city_name"];
  const char* mainDescription = doc["main_description"];
  const char* secondaryDescription = doc["secondary_description"];
  const char* iconUrl = doc["icon_url"];
  const float currentTemp = doc["current_temp"];
  const float minTemp = doc["temp_min"];
  const float maxTemp = doc["temp_max"];
  
  Serial.println("City: " + String(cityName));
  Serial.println("Temp: " + String(convertKelvinToFahrenheit(currentTemp)) + " F");
  Serial.println("Min Temp: " + String(convertKelvinToFahrenheit(minTemp)) + " F");
  Serial.println("Max Temp: " + String(convertKelvinToFahrenheit(maxTemp)) + " F");
  Serial.println("Current Weather: " + String(mainDescription));
  Serial.println("Secondary Description: " + String(secondaryDescription));
  Serial.println("Icon Url:" + String(iconUrl));
}

float convertKelvinToCelsius(float kelvinTemp) {
  return kelvinTemp - KELVIN_AT_0_C;
}

float convertKelvinToFahrenheit(float kelvinTemp) {
  return ((kelvinTemp - KELVIN_AT_0_C) * 9 / 5) + 32;
}
