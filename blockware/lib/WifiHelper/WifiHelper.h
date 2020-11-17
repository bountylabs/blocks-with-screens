#include <ESP8266WiFi.h>

void ConnectWifi(const char* ssid, const char* password);

bool ConnectToHost(WiFiClient* client, String host, uint16_t port);

bool ConnectToSecureHost(WiFiClientSecure* client, String host, uint16_t port);
