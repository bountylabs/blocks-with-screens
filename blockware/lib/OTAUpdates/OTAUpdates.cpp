#include <Adafruit_SSD1351.h>
#include <ArduinoOTA.h>

#include "OTAUpdates.h"
#include <Colors.h>
#include <DLog.h>

extern Adafruit_SSD1351 tft;

void output(const char* message, int color = WHITE, int size = 1)
{
  DLOG(message);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.print(message);
}

void outputln(const char* message, int color = WHITE, int size = 1)
{
  DLOG(message);
  DLOG("\n");
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.println(message);
}

void displayReset() {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
}

void OTAUpdates_handle()
{
  ArduinoOTA.handle();
}

void OTAUpdates_setup(const char* hostname, const char* ssid, const char* password)
{
  DLOG(PSTR("setupOTAUpdates\n"));

  displayReset();

  outputln(ssid);
  output(PSTR("Connecting..."));

  // Connect wifi to support OTA programming
  WiFiClient wifiClient;

  WiFi.begin(ssid, password);
  unsigned long time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    output(".");

    // IDEA / TODO: Define multiple ssid/password combos to attempt?
    // Allow block to work in many places

    // There seems to be a bug where sometimes wifi will hang forever trying to connect
    // Try to work around by resetting some things
    // See https://github.com/esp8266/Arduino/issues/5527
    if ((millis() - time) > 60000) {
      output(PSTR("Restarting Wifi"), RED);

      WiFi.disconnect(true);
      WiFi.begin(ssid, password);
      time = millis();
    }
  }
  outputln("!");

  // Configure OTA programming
  ArduinoOTA.setHostname(hostname);
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    displayReset();
    outputln(PSTR("OTA Update"), GREEN);
  });
  ArduinoOTA.onEnd([]() {
    outputln(PSTR("OTA Done!"), GREEN);
    outputln(PSTR("Restarting..."));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DLOG(PSTR("OTA Progress: %u%%\n"), (progress / (total / 100)));
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(0, 10);
    tft.printf(PSTR("%u%%\n"), (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    tft.setTextColor(RED);
    tft.printf(PSTR("Error: %d\n"), error);
    DLOG(PSTR("OTA Error[%u]: "), error);
    if (error == OTA_AUTH_ERROR)
      DLOG(PSTR("OTA Auth Failed\n"));
    else if (error == OTA_BEGIN_ERROR)
      DLOG(PSTR("OTA Begin Failed\n"));
    else if (error == OTA_CONNECT_ERROR)
      DLOG(PSTR("OTA Connect Failed\n"));
    else if (error == OTA_RECEIVE_ERROR)
      DLOG(PSTR("OTA Receive Failed\n"));
    else if (error == OTA_END_ERROR)
      DLOG(PSTR("OTA End Failed\n"));
  });
  ArduinoOTA.begin();
}
