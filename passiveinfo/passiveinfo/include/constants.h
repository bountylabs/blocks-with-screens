#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define DEBUG

#include <Arduino.h>

const int HTTP_PORT = 80;
const int HTTPS_PORT = 443;

const float KELVIN_AT_0_C = 273.15;

// Open Weather API
const String OPEN_WEATHER_HOST = "api.openweathermap.org";
const String OPEN_WEATHER_COMMON_URL = "/data/2.5/weather?";
const String OPEN_WEATHER_APPID = "appid=";
const String OPEN_WEATHER_ZIP_URL = "zip=";

// Alpha Vantage API
const String ALPHAV_HOST = "www.alphavantage.co";
const String ALPHAV_COMMON_URL = "/query?";
const String ALPHAV_FUNCTION_URL = "function=";
const String ALPHAV_STOCK_FUNCTION_TIME_SERIES_DAILY = "TIME_SERIES_DAILY";
const String ALPHAV_STOCK_SYMBOL = "symbol=";
const String ALPHA_APIKEY = "apikey=";

#endif
