#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>

#include "wifihelper.h"
#include "constants.h"
#include "secrets.h"

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;
String connectingStr = "";
char buffer[128];

void setupWifi(Adafruit_SSD1351& screen) {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  sprintf(buffer, "Connecting to %s", ssid);
  screen.setCursor(0, 0);
  screen.setTextColor(WHITE);
  screen.print(buffer);

  WiFi.begin(ssid, password);
  String tempConnectingStr = String(buffer);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    screen.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}

bool connectToHost(WiFiClient* client, String host) {
  if (!client->connect(host, HTTP_PORT)) {
    Serial.println("connection failed");
    return false;
  }
  return true;
}

bool connectToSecureHost(WiFiClientSecure* client, String host) {
  client->setInsecure();
  if (!client->connect(host, HTTPS_PORT)) {
    Serial.println("connection failed");
    return false;
  }
  return true;
}
