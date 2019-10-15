#include <ESP8266WiFi.h>
#include <Adafruit_SSD1351.h>

void setupWifi(Adafruit_SSD1351& screen);

bool connectToHost(WiFiClient* client, String host);

bool connectToSecureHost(WiFiClientSecure* client, String host);
