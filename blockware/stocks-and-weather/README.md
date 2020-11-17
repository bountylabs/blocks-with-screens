# Stocks And Weather

An example program that can show a stock price and optionally the weather.

Note: While at one time this program would cycle between showing the stock price and the weather, now we get a memory error in the serial terminal when trying to run both. Either the sizes of the responses from the APIs we're using increased, or we inadvertently introduced a memory bug, or both. By default only stocks are enabled. See the `loop` function in `main.cpp:238` to re-enable weather.

## Configuring

Create a `secrets.h` file based on `secrets.h.example` that contains your WiFi credentials, Open Weather API key, and Alpha Vantage API key.
