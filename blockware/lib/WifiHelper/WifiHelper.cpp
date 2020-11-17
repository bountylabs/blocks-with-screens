#include <ESP8266WiFi.h>
#include <Colors.h>
#include <DLog.h>
#include <Text.h>

void ConnectWifi(const char* ssid, const char* password)
{
  DLOG(PSTR("ConnectWifi(%s, ****)\n"), ssid);

  outputf(WHITE, 1, PSTR("Connecting to %s..."), ssid);

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
      outputln(PSTR("Restarting Wifi"), RED);

      WiFi.disconnect(true);
      WiFi.begin(ssid, password);
      time = millis();
    }
  }
  outputln("Connected!", GREEN);
  DLOG(PSTR("WiFi connected\n")); 
  DLOG(PSTR("IP address: %s\n"), WiFi.localIP().toString().c_str());
  DLOG(PSTR("Netmask: %s\n"), WiFi.subnetMask().toString().c_str());
  DLOG(PSTR("Gateway: %s\n"), WiFi.gatewayIP().toString().c_str());
}

bool ConnectToHost(WiFiClient* client, String host, uint16_t port) {
  if (!client->connect(host, port)) {
    Serial.println("connection failed");
    return false;
  }
  return true;
}

bool ConnectToSecureHost(WiFiClientSecure* client, String host, uint16_t port) {
  client->setInsecure();
  if (!client->connect(host, port)) {
    Serial.println("connection failed");
    return false;
  }
  return true;
}
