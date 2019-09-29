/***************************************************
  Example sketch for the Block with Screen
 ****************************************************/

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <HARestAPI.h>
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include "config.h"
#include "colors.h"

// Software bit-banged SPI mode works just fine but is slow
// Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

// Hardware SPI mode only works if you make a hack in SPI.c under the hood. In short, find the two
// lines with `pinMode(MISO` in them and comment them out
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

WiFiClient wifiClient;
HARestAPI haClient(wifiClient);

bool OTARunning;

void setup(void) {
  Serial.begin(115200);
  Serial.print("setup");

  // 15MHz SPI
  tft.begin(SPI_SPEED);

  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextWrap(true);
  tft.setTextSize(1);

  tft.setTextColor(WHITE);
  tft.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.print(".");

    // There seems to be a bug where sometimes wifi will hang forever trying to connect
    // Try to work around by resetting some things
    // See https://github.com/esp8266/Arduino/issues/5527
    if ((millis() - time) > 10000) {
      Serial.print("Restarting Wifi");
      tft.setTextColor(RED);
      tft.println("\nRestarting Wifi");
      tft.setTextColor(WHITE);
      WiFi.disconnect(true);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      time = millis();
    }
  }
  tft.println();
  tft.setTextColor(GREEN);
  tft.println("Connected!");

  const char* haIp = HOME_ASSISTANT_IP;
  uint16_t haPort = HOME_ASSISTANT_PORT;
  const char* haPassword = HOME_ASSISTANT_TOKEN;
  haClient.setHAServer(haIp, haPort);
  haClient.setHAPassword(haPassword);

  // Configure OTA programming
  ArduinoOTA.setHostname("ha-display");
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.setTextWrap(true);
    tft.setTextSize(1);
    tft.setTextColor(GREEN);
    tft.println("OTA Programming");
    OTARunning = true;
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA End");
    tft.setTextColor(GREEN);
    tft.println("OTA Done!");
    tft.setTextColor(WHITE);
    tft.println("Restarting");
    OTARunning = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r\n", (progress / (total / 100)));
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(0, 10);
    tft.printf("%u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    tft.setTextColor(RED);
    tft.printf("Error: %d\n", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
    OTARunning = false;
  });
  ArduinoOTA.begin();
}

DeserializationError getHAEntity(const char *entityId, JsonDocument &doc) {
  String resp = haClient.sendGetHA(String("/api/states/") + String(entityId), "");
  DeserializationError err = deserializeJson(doc, resp);
  return err;
}

StaticJsonDocument<1024> doc;

float getHAEntityStateFloat(const char *entityId) {
  DeserializationError err = getHAEntity(entityId, doc);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    return 0.0;
  }
  return doc["state"];
}

void waitAndHandle(unsigned long ms) {
  for (int i = 0; i < ms; i += 20) {
    ArduinoOTA.handle();
    yield();
    delay(20);
  }
}

void printStatLine(const char *label, const char *format, float value, const char *unit) {
  tft.setTextColor(CYAN);
  tft.print(label);
  tft.setTextColor(GREEN);
  tft.printf(format, value);
  tft.setTextColor(LIGHT_GRAY);
  tft.print(unit);
  tft.println();
}

void printEnvInfo(float temp, float humidity, float absHumidity) {
  char tempUnit;
  sprintf(&tempUnit, "%cF", (char)247);
  printStatLine("Temp:       ", "%.1f", temp, &tempUnit);
  printStatLine("Humidity:   ", "%.0f", humidity, "%");
  printStatLine("Abs. Hum.:  ", "%.2f", absHumidity, "g/m^3");
}

void loop() {
  while (OTARunning) {
    waitAndHandle(1000);
  }
  float insideTemp = getHAEntityStateFloat("sensor.thermostat_temperature");
  float insideHumidity = getHAEntityStateFloat("sensor.thermostat_humidity");
  float insideAbsHumidity = getHAEntityStateFloat("sensor.inside_absolutehumidity");
  float outsideTemp = getHAEntityStateFloat("sensor.outside_temperature");
  float outsideHumidity = getHAEntityStateFloat("sensor.outside_humidity");
  float outsideAbsHumidity = getHAEntityStateFloat("sensor.outside_absolutehumidity");

  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Inside:");
  tft.setTextSize(1);
  tft.println("");
  printEnvInfo(insideTemp, insideHumidity, insideAbsHumidity);

  tft.println("");

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Outside:");
  tft.setTextSize(1);
  tft.println("");
  printEnvInfo(outsideTemp, outsideHumidity, outsideAbsHumidity);

  waitAndHandle(10000);
}
