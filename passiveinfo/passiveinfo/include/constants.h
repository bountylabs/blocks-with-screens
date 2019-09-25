#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define DEBUG

#include <Arduino.h>

const int HTTP_PORT = 80;

const String OPEN_WEATHER_HOST = "api.openweathermap.org";
const String OPEN_WEATHER_COMMON_URL = "/data/2.5/weather?";
const String OPEN_WEATHER_APPID = "appid=";
const String OPEN_WEATHER_ZIP_URL = "zip=";

const float KELVIN_AT_0_C = 273.15;

#endif
