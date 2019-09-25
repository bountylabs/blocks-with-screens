#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "passiveinfo.h"
#include "constants.h"
#include "wifihelper.h"
#include "weather.h"
#include "httphelper.h"

void setup() {
  Serial.begin(115200);

  setupWifi();
}

void loop() {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  getCurrentWeather(client);
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
