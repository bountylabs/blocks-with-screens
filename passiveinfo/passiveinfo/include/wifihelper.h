#include <ESP8266WiFi.h>

void setupWifi();

bool connectToHost(WiFiClient* client, String host);

bool connectToSecureHost(WiFiClientSecure* client, String host);
