#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "constants.h"

void setup() {
  Serial.begin(115200);
  delay(100);

  setupWifi();
}

void loop() {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(OPEN_WEATHER_HOST, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = getWeatherUrlForZipCountry("95128", "us");
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(getHttpGETWithUrlAndHost(OPEN_WEATHER_HOST, url));

  delay(500);

  String line;
  while(client.available()){
    line = client.readStringUntil('\r');
//    Serial.println(line);
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
    const char* cityName = doc["name"];
    const char* currentWeather = doc["weather"][0]["main"];
    const char* currentWeatherDescription = doc["weather"][0]["description"];
    const char* icon = doc["weather"]["icon"];
    float currentTemp = doc["main"]["temp"];
    float minTemp = doc["main"]["temp_min"];
    float maxTemp = doc["main"]["temp_max"];
    
    Serial.println("City: " + String(cityName));
    Serial.println("Temp: " + String(currentTemp));
    Serial.println("Min Temp: " + String(minTemp));
    Serial.println("Max Temp: " + String(maxTemp));
    Serial.println("Current Weather: " + String(currentWeather));
    Serial.println(String(currentWeatherDescription));
  } else {
    Serial.println("Request was not successful");
  }

  delay(600000);
}
