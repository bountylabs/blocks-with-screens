#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "passiveinfo.h"
#include "constants.h"
#include "wifihelper.h"
#include "weather.h"
#include "httphelper.h"
#include "stocks.h"

void setup() {
  Serial.begin(115200);

  setupWifi();
}

void loop() {
  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  // getCurrentWeather(client);

  getStockPrice(client, "TWTR");
}

void getCurrentWeather(WiFiClient client) {
  bool successful = false;
  successful = connectToHost(&client, OPEN_WEATHER_HOST);
  if (!successful) {
    return;
  }

  String url = getWeatherUrlForZipCountry("95128", "us");
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(getHttpGETRequestWithUrlAndHost(OPEN_WEATHER_HOST, url));
  delay(300);

  String line;
//  Serial.println("is client available: " + String(client.available()));
  while(client.available()){
    line = client.readStringUntil('\r');
    Serial.println(line);
    Serial.println();
  }

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    delay(600000);
    return;
  }

  int cod = doc["cod"];
  if (cod == 200) {
    parseWeatherZipResponse(&doc);
  } else {
    Serial.println("Request was not successful");
  }

  delay(600000);
}

void getStockPrice(WiFiClientSecure client, String symbol) {
  bool successful = false;
  successful = connectToSecureHost(&client, ALPHAV_HOST);
  if (!successful) {
    return;
  }

  String url = getAlphaVUrlForTimeSeriesDaily("TWTR");
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(getHttpGETRequestWithUrlAndHost(ALPHAV_HOST, url));
  int counter = 0;
  while(!client.available()) {
    Serial.println("Response is not available yet: " + String(++counter));
    delay(500);
  }

  String line;
  while(client.available()){
    // Serial.println("Num of Bytes: " + String(client.available()));
    // line = client.readStringUntil('\r');
    // Serial.println(line);
    Serial.print(char(client.read()));
    // Serial.println();
  }

  while(1);

  // DynamicJsonDocument doc;
  // JsonObject& obj = doc.parseObject(client.getStream());
  // DeserializationError error = deserializeJson(doc, line);
  // if (error) {
  //   Serial.print(F("deserializeJson() failed: "));
  //   Serial.println(error.c_str());
  //   delay(600000);
  //   return;
  // }

  // const char* symbolFromAPI = doc["Meta Data"]["2. Symbol"];
  // const char* lastRefreshedDate = doc["Meta Data"]["3. Last Refreshed"];
  // const char* openPrice = doc["Times Series (Daily)"][lastRefreshedDate]["open"];
  // const char* closePrice = doc["Times Series (Daily)"][lastRefreshedDate]["close"];
  // Serial.println("Symbol: " + String(symbolFromAPI));
  // Serial.println("Open Price: " + String(openPrice));
  // Serial.println("Close Price: " + String(closePrice));
}
