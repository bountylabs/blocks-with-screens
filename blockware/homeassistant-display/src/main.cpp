/***************************************************
  Example sketch for the Block with Screen
 ****************************************************/

#define TELNETLOG 1
#include <DLog.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <HARestAPI.h>
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>
#include <OTAUpdates.h>
#include "config.h"
#include <Colors.h>

// Software bit-banged SPI mode works just fine but is slow
// Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

// Hardware SPI mode only works if you make a hack in SPI.c under the hood. In short, find the two
// lines with `pinMode(MISO` in them and comment them out
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

WiFiClient WifiClient;
HARestAPI HAClient(WifiClient);

bool OTARunning;
// In order to avoid blanking the screen on every update, we will just overwrite the text.
// But in some cases we will need to clear the entire screen (e.g. after a failed OTA transfer).
bool NeedsScreenRefresh;

void setup(void) {
  Serial.begin(115200);
  DLOG("setup");

  // 15MHz SPI
  tft.begin(SPI_SPEED);

  // Home assistant connection info
  const char* haIp = HOME_ASSISTANT_IP;
  uint16_t haPort = HOME_ASSISTANT_PORT;
  const char* haPassword = HOME_ASSISTANT_TOKEN;
  HAClient.setHAServer(haIp, haPort);
  HAClient.setHAPassword(haPassword);

  OTAUpdates_setup(PSTR("ha-display"), PSTR(WIFI_SSID), PSTR(WIFI_SSID));

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
    DLOG(PSTR("deserializeJson() failed with code %s"), err.c_str());
    return 0.0;
  }
  return doc["state"];
}

void waitAndHandle(unsigned long ms) {
  for (unsigned long i = 0; i < ms; i += 20) {
    OTAUpdates_handle();
    yield();
    delay(20);
  }
}

void printStatLine(const char *label, const char *format, float value, const char *unit) {
  tft.setTextColor(CYAN, BLACK);
  tft.print(label);
  tft.setTextColor(GREEN, BLACK);
  tft.printf(format, value);
  tft.setTextColor(LIGHT_GRAY, BLACK);
  tft.print(unit);
  tft.println();
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
    tft.fillScreen(BLACK);
    NeedsScreenRefresh = false;
  }
  tft.setCursor(0, 0);

  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  tft.println("Inside:");
  tft.setTextSize(1);
  tft.println("");
  printEnvInfo(insideTemp, insideHumidity, insideAbsHumidity);

  tft.println("");

  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  tft.println("Outside:");
  tft.setTextSize(1);
  tft.println("");
  printEnvInfo(outsideTemp, outsideHumidity, outsideAbsHumidity);

  waitAndHandle(10000);
}
