#include "stocks.h"

#include "constants.h"
#include "secrets.h"

String getAlphaVAPIKeyUrl() {
    return String(ALPHA_APIKEY) + String(ALPHA_VANTAGE_API_KEY);
}

String getAlphaVUrlForTimeSeriesDaily(String symbol) {
    return String(ALPHAV_COMMON_URL) + 
    String(ALPHAV_FUNCTION_URL) + String(ALPHAV_STOCK_FUNCTION_TIME_SERIES_DAILY) +
    "&" + String(ALPHAV_STOCK_SYMBOL) + symbol + "&" + getAlphaVAPIKeyUrl();
}