#include "constants.h"

String getWeatherUrlForZipCountry(String zip, String country) {
  return String(OPEN_WEATHER_COMMON_URL) + String(OPEN_WEATHER_ZIP_URL) + zip + "," + country + "&" + getWeatherAppIdUrl();
}

String getWeatherAppIdUrl() {
  return String(OPEN_WEATHER_APPID) + String(OPEN_WEATHER_API_KEY);
}
