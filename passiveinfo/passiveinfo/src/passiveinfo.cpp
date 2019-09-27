#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#include "passiveinfo.h"
#include "constants.h"
#include "wifihelper.h"
#include "weather.h"
#include "httphelper.h"
#include "stocks.h"

Adafruit_SSD1351 display = Adafruit_SSD1351(16, 12, 13, 14, 15);

float closePrice = 0;
float deltaPrice = 0;

void setup() {
  Serial.begin(115200);
  display.begin();
  display.fillScreen(0x000000);
  setupWifi(display);
}

void loop() {
  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  // getCurrentWeather(client);
  getStockPrice(client, "TWTR");

  display.fillScreen(0x000000);
  drawStock(display, "TWTR", closePrice, deltaPrice, 2, 10, 10);

  delay(60000);
}

void getCurrentWeather(WiFiClientSecure client) {
  bool successful = false;
  successful = connectToSecureHost(&client, HOST);
  if (!successful) {
    return;
  }

  String url = getWeatherUrlForZipCountry("95128", "us");
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(getHttpGETRequestWithUrlAndHost(HOST, url));
  delay(300);

  String line;
//  Serial.println("is client available: " + String(client.available()));
  while(client.available()){
    line = client.readStringUntil('\r');
    Serial.println(line);
  }

  DynamicJsonDocument doc(10048);
  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  parseWeatherZipResponse(&doc);
}

void getStockPrice(WiFiClientSecure client, String symbol) {
  bool successful = false;
  successful = connectToSecureHost(&client, HOST);
  if (!successful) {
    return;
  }

  String url = getAlphaVUrlForTimeSeriesDaily("TWTR");
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(getHttpGETRequestWithUrlAndHost(HOST, url));
  int counter = 0;
  while(!client.available()) {
    Serial.println("Response is not available yet: " + String(++counter));
    delay(500);
  }

  String line;
  while(client.available()){
    line = client.readStringUntil('\r');
    Serial.println(line);
  }

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  const char* symbolFromAPI = doc["symbol"];
  const float openPrice = doc["open"];
  closePrice = doc["close"];
  const float lowPrice = doc["low"];
  const float highPrice = doc["high"];
  deltaPrice = closePrice - openPrice;

  Serial.println("Symbol: " + String(symbolFromAPI));
  Serial.println("Open Price: " + String(openPrice));
  Serial.println("Close Price: " + String(closePrice));
  Serial.println("Low Price: " + String(lowPrice));
  Serial.println("High Price: " + String(highPrice));
}
