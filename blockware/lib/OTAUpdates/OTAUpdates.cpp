#include <Adafruit_SSD1351.h>
#include <ArduinoOTA.h>

#include "OTAUpdates.h"
#include <Colors.h>

extern Adafruit_SSD1351 tft;

void output(const char* message, int color = WHITE, int size = 1)
{
  Serial.print(message);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.print(message);
}

void outputln(const char* message, int color = WHITE, int size = 1)
{
  Serial.println(message);
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
  Serial.printf("setupOTAUpdates\n");

  displayReset();

  outputln(ssid);
  output("Connecting...");

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
      output("Restarting Wifi", RED);

      WiFi.disconnect(true);
      WiFi.begin(ssid, password);
      time = millis();
    }
  }
  outputln("!");

  outputln("Connected!", GREEN);
  outputln(WiFi.localIP().toString().c_str());
  outputln("\nProceeding in 5 seconds...");
  delay(5000);

  // Configure OTA programming
  ArduinoOTA.setHostname(hostname);
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    displayReset();
    outputln("OTA Update", GREEN);
  });
  ArduinoOTA.onEnd([]() {
    outputln("OTA Done!", GREEN);
    outputln("Restarting...");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r\n", (progress / (total / 100)));
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(0, 10);
    tft.printf("%u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    tft.setTextColor(RED);
    tft.printf("Error: %d\n", error);
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("OTA End Failed");
  });
  ArduinoOTA.begin();
}
