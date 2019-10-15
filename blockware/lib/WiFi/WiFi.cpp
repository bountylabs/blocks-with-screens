#include <ESP8266WiFi.h>
#include <Colors.h>
#include <DLog.h>
#include <Text.h>

void ConnectWifi(const char* ssid, const char* password)
{
  DLOG(PSTR("ConnectWifi\n"));
  displayReset();

  outputln(ssid);
  output(PSTR("Connecting to wifi..."));

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
  outputln("Connected!", GREEN);
}
