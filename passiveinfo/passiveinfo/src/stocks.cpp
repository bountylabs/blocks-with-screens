#include "stocks.h"

#include "constants.h"
#include "secrets.h"

String getAlphaVAPIKeyUrl() {
    return String(ALPHA_APIKEY) + String(ALPHA_VANTAGE_API_KEY);
}

String getAlphaVUrlForTimeSeriesDaily(String symbol) {
    return String(STOCKS_PATH) + symbol;
}