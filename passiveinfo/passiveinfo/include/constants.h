#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define DEBUG

#include <Arduino.h>

const int RED = 0xF800;
const int GREEN = 0x07E0;
const int WHITE = 0xFFFF;

const int HTTP_PORT = 80;
const int HTTPS_PORT = 443;

const float KELVIN_AT_0_C = 273.15;

const String HOST = "blockchainwithscreens.herokuapp.com";

// Weather API
const String WEATHER_ZIP_PATH = "/weather/zip/";

// Stocks API
const String STOCKS_PATH = "/stocks/symbol/";

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
