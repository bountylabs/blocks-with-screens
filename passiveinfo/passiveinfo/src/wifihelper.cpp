#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "constants.h"

const char* ssid     = "";
const char* password = "";

void setupWifi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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
  if (!client->connect(OPEN_WEATHER_HOST, HTTP_PORT)) {
    Serial.println("connection failed");
    return false;
  }
  return true;
}
