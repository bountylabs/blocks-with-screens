#include <ArduinoJson.h>

#include "constants.h"
#include "secrets.h"

float convertKelvinToCelsius(float kelvinTemp);
float convertKelvinToFahrenheit(float kelvinTemp);

String getWeatherAppIdUrl() {
  return String(OPEN_WEATHER_APPID) + String(OPEN_WEATHER_API_KEY);
}

String getWeatherUrlForZipCountry(String zip, String country) {
  return String(OPEN_WEATHER_COMMON_URL) + String(OPEN_WEATHER_ZIP_URL) + zip + "," + country + "&" + getWeatherAppIdUrl();
}

void parseWeatherZipResponse(DynamicJsonDocument* docPtr) {
  DynamicJsonDocument doc = *docPtr;
  const char* cityName = doc["name"];
  const char* currentWeather = doc["weather"][0]["main"];
  const char* currentWeatherDescription = doc["weather"][0]["description"];
  const char* icon = doc["weather"]["icon"];
  float currentTemp = doc["main"]["temp"];
  float minTemp = doc["main"]["temp_min"];
  float maxTemp = doc["main"]["temp_max"];
  
  Serial.println("City: " + String(cityName));
  Serial.println("Temp: " + String(convertKelvinToFahrenheit(currentTemp)) + " F");
  Serial.println("Min Temp: " + String(convertKelvinToFahrenheit(minTemp)) + " F");
  Serial.println("Max Temp: " + String(convertKelvinToFahrenheit(maxTemp)) + " F");
  Serial.println("Current Weather: " + String(currentWeather));
  Serial.println(String(currentWeatherDescription));
}

float convertKelvinToCelsius(float kelvinTemp) {
  return kelvinTemp - KELVIN_AT_0_C;
}

float convertKelvinToFahrenheit(float kelvinTemp) {
  return ((kelvinTemp - KELVIN_AT_0_C) * 9 / 5) + 32;
}
