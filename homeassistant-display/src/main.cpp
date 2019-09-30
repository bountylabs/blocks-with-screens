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
// Adafruit_SSD1351 TFT = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

// Hardware SPI mode only works if you make a hack in SPI.c under the hood. In short, find the two
// lines with `pinMode(MISO` in them and comment them out
Adafruit_SSD1351 TFT = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

WiFiClient WifiClient;
HARestAPI HAClient(WifiClient);

bool OTARunning;
// In order to avoid blanking the screen on every update, we will just overwrite the text.
// But in some cases we will need to clear the entire screen (e.g. after a failed OTA transfer).
bool NeedsScreenRefresh;

void setup(void) {
  Serial.begin(115200);
  Serial.print("setup");

  // 15MHz SPI
  TFT.begin(SPI_SPEED);

  TFT.fillScreen(BLACK);
  TFT.setCursor(0, 0);
  TFT.setTextWrap(true);
  TFT.setTextSize(1);

  TFT.setTextColor(WHITE);
  TFT.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    TFT.print(".");

    // There seems to be a bug where sometimes wifi will hang forever trying to connect
    // Try to work around by resetting some things
    // See https://github.com/esp8266/Arduino/issues/5527
    if ((millis() - time) > 10000) {
      Serial.print("Restarting Wifi");
      TFT.setTextColor(RED);
      TFT.println("\nRestarting Wifi");
      TFT.setTextColor(WHITE);
      WiFi.disconnect(true);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      time = millis();
    }
  }
  TFT.println();
  TFT.setTextColor(GREEN);
  TFT.println("Connected!");

  const char* haIp = HOME_ASSISTANT_IP;
  uint16_t haPort = HOME_ASSISTANT_PORT;
  const char* haPassword = HOME_ASSISTANT_TOKEN;
  HAClient.setHAServer(haIp, haPort);
  HAClient.setHAPassword(haPassword);

  // Configure OTA programming
  ArduinoOTA.setHostname("ha-display");
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
    TFT.fillScreen(BLACK);
    TFT.setCursor(0, 0);
    TFT.setTextWrap(true);
    TFT.setTextSize(1);
    TFT.setTextColor(GREEN);
    TFT.println("OTA Programming");
    OTARunning = true;
    NeedsScreenRefresh = true;
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA End");
    TFT.setTextColor(GREEN);
    TFT.println("OTA Done!");
    TFT.setTextColor(WHITE);
    TFT.println("Restarting");
    OTARunning = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r\n", (progress / (total / 100)));
    TFT.setTextColor(WHITE, BLACK);
    TFT.setCursor(0, 10);
    TFT.printf("%u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    TFT.setTextColor(RED);
    TFT.printf("Error: %d\n", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
    OTARunning = false;
  });
  ArduinoOTA.begin();

  NeedsScreenRefresh = true;
}

DeserializationError getHAEntity(const char *entityId, JsonDocument &doc) {
  String resp = HAClient.sendGetHA(String("/api/states/") + String(entityId), "");
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
  TFT.setTextColor(CYAN, BLACK);
  TFT.print(label);
  TFT.setTextColor(GREEN, BLACK);
  TFT.printf(format, value);
  TFT.setTextColor(LIGHT_GRAY, BLACK);
  TFT.print(unit);
  TFT.println();
}

void printEnvInfo(float temp, float humidity, float absHumidity) {
  char tempUnit;
  sprintf(&tempUnit, "%cF", (char)247);
  printStatLine("Temp:      ", "%.1f", temp, &tempUnit);
  printStatLine("Humidity:  ", "%.0f", humidity, "%");
  printStatLine("Abs. Hum.: ", "%.2f", absHumidity, "g/m^3");
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

  if (NeedsScreenRefresh) {
    TFT.fillScreen(BLACK);
    NeedsScreenRefresh = false;
  }
  TFT.setCursor(0, 0);

  TFT.setTextColor(WHITE, BLACK);
  TFT.setTextSize(2);
  TFT.println("Inside:");
  TFT.setTextSize(1);
  TFT.println("");
  printEnvInfo(insideTemp, insideHumidity, insideAbsHumidity);

  TFT.println("");

  TFT.setTextColor(WHITE, BLACK);
  TFT.setTextSize(2);
  TFT.println("Outside:");
  TFT.setTextSize(1);
  TFT.println("");
  printEnvInfo(outsideTemp, outsideHumidity, outsideAbsHumidity);

  waitAndHandle(10000);
}
