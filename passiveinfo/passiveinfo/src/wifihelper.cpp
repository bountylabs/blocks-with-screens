#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "constants.h"
#include "secrets.h"

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

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
