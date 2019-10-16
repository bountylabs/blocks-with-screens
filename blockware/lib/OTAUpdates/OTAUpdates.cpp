#include <Adafruit_SSD1351.h>
#include <ArduinoOTA.h>

#include "OTAUpdates.h"
#include <Colors.h>
#include <DLog.h>
#include <WifiHelper.h>
#include <Text.h>

extern Adafruit_SSD1351 tft;

void OTAUpdates_handle()
{
  ArduinoOTA.handle();
}

void OTAUpdates_setup(const char* hostname, const char* password)
{
  DLOG(PSTR("setupOTAUpdates\n"));

  // Configure OTA programming
  ArduinoOTA.setHostname(hostname);
  // No authentication by default
  if (password != NULL) {
    ArduinoOTA.setPassword(password);
  }
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
    outputf(RED, 1, PSTR("Error: %d\n"), error);
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
