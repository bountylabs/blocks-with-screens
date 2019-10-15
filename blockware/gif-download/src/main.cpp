/***************************************************
  Example sketch for the Block with Screen
 ****************************************************/

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// Pin setup
#define SCLK_PIN 14 // D5 -- ESP8266 Hardware SCLK
#define MOSI_PIN 13 // D7 -- ESP8266 Hardware MOSI
// Note this is also the ESP8266 Hardware MISO pin! That seems to be a bad thing that we should fix in V2.
// See the bit about the hack below
#define DC_PIN   12 // D6
#define CS_PIN   16 // D0
#define RST_PIN  15 // D8

// 15MHz SPI
// Note, the ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000

#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <qrcode.h>
#include "secrets.h"

// Software bit-banged SPI mode works just fine but is slow
// Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

// Hardware SPI mode only works if you make a hack in SPI.c under the hood. In short, find the two
// lines with `pinMode(MISO` in them and comment them out
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

int downloadFile(const char *URL, const char *filepath, const char *fingerprint = NULL) {
  HTTPClient http;
  if (fingerprint == NULL) {
    http.begin(URL, fingerprint);
  } else {
    http.begin(URL);
  }
  int httpCode = http.GET();

  Serial.printf("[HTTP] GET %s code: %d\n", URL, httpCode);

  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    // tft.setTextColor(RED);
    // tft.printf("HTTP Status: %d\n", httpCode);
    http.end();
    return 0;
  }

  // Delete file if exists
  if (SPIFFS.exists(filepath)) {
    // tft.setTextColor(WHITE);
    // tft.printf("Deleting %s\n", filepath);
    SPIFFS.remove(filepath);
  }

  // Open a file for writing
  fs::File f = SPIFFS.open(filepath, "w");
  if (!f) {
      Serial.println("File open failed");
      tft.setTextColor(RED);
      tft.println("File open failed");
      return 0;
  }

  // tft.setTextColor(WHITE);
  // tft.println("Downloading...");

  // Write response to file
  // TODO: Would be nice to modify writeToStream to support progress
  int bytesWritten = http.writeToStream(&f);
  if (bytesWritten < 0) {
    Serial.printf("writeToStream error %d\n", bytesWritten);
    tft.setTextColor(RED);
    tft.printf("writeToStream error %d\n", bytesWritten);
  } else {
    Serial.printf("Wrote %d kb\n", bytesWritten / 1000);
    // tft.setTextColor(WHITE);
    // tft.printf("Wrote %d kb\n", bytesWritten / 1000);
  }

  f.close();
  http.end();

  return bytesWritten;
}

void drawFSBmp(const char *filename, int x, int y, int w, int h) {
  Serial.print("Drawing file: "); Serial.println(filename);

  fs::File bmpFile = SPIFFS.open( filename, "r");

  if (!bmpFile) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  int fileSize = bmpFile.size();
  Serial.printf("File size is %d\n", fileSize);

  int numBytes = 0;

  tft.startWrite();
  tft.setAddrWindow(x, y, w, h);

  while (bmpFile.available()) {
    yield();
    ArduinoOTA.handle();

    char buffer[256];
    size_t size = bmpFile.readBytes(buffer, sizeof(buffer));
    numBytes += size;
    tft.writePixels((uint16_t *)buffer, size / 2);
  }
  tft.endWrite();
  bmpFile.close();
  Serial.printf("Read %d bytes", numBytes);
}

void setupArduinoOTA() {
  ArduinoOTA.setHostname("gif-example");
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
  });
  ArduinoOTA.begin();
}

void formatSPIFFSIfNecessary(void) {
  if (!SPIFFS.exists("/formatComplete.txt")) {
    tft.setTextColor(WHITE);
    tft.println("SPIFFS Formatting...");

    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
    SPIFFS.format();
    Serial.println("Spiffs formatted");

    File f = SPIFFS.open("/formatComplete.txt", "w");
    if (!f) {
        Serial.println("file open failed");
        tft.setTextColor(RED);
        tft.println("File open failed");
    } else {
        f.println("Format Complete");
    }
  } else {
    Serial.println("SPIFFS is formatted. Moving along...");
    tft.setTextColor(GREEN);
    tft.println("SPIFFS is formatted");
  }
}

void setup(void) {
  Serial.begin(115200);
  Serial.print("setup");
  // 15MHz SPI
  tft.begin(15000000);

  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextWrap(true);
  tft.setTextSize(1);

  tft.setTextColor(WHITE);
  tft.printf("Connecting to %s...", WIFI_SSID);

  // Connect wifi to support OTA programming
  WiFiClient wifiClient;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.print(".");

    // There seems to be a bug where sometimes wifi will hang forever trying to connect
    // Try to work around by resetting some things
    // See https://github.com/esp8266/Arduino/issues/5527
    if ((millis() - time) > 60000) {
      Serial.print("Restarting Wifi");
      tft.setTextColor(RED);
      tft.println("Restarting Wifi");
      tft.setTextColor(WHITE);
      WiFi.disconnect(true);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      time = millis();
    }
  }
  tft.println("!");
  tft.setTextColor(GREEN);
  tft.println("Connected!");

  // Configure OTA programming
  setupArduinoOTA();

  tft.setTextColor(WHITE);
  tft.println("Configuring SPIFFS...");
  SPIFFS.begin();
  // SPIFFS.format();
  formatSPIFFSIfNecessary();
  FSInfo fsInfo;
  SPIFFS.info(fsInfo);

  tft.setTextColor(GREEN);
  tft.printf("%d / %d kb used\n", fsInfo.usedBytes / 1000, fsInfo.totalBytes / 1000);
}

int getRandomNumber(int startNum, int endNum) {
  randomSeed(ESP.getCycleCount());
  return random(startNum, endNum);
}

void loop() {
  uint16_t start;
  uint16_t time;

  int randomId = getRandomNumber(0, INT_MAX);
  char url[64];
  sprintf(url, "http://blockswithscreens.appspot.com?block=%d", randomId);

  // The structure to manage the QR code
  QRCode qrcode;
  // Allocate a chunk of memory to store the QR code
  uint8_t qrcodeBytes[qrcode_getBufferSize(4)];
  qrcode_initText(&qrcode, qrcodeBytes, 4, ECC_LOW, url);
  tft.fillScreen(BLACK);

  int blockSize = tft.width() / qrcode.size;
  int blockOffset = (tft.width() - qrcode.size * blockSize) / 2;
  for (uint8 y = 0; y < qrcode.size; y++) {
    for (uint8 x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) {
          // tft.drawPixel(x, y, WHITE);
          tft.fillRect(x*blockSize + blockOffset, y*blockSize + blockOffset, blockSize, blockSize, WHITE);
        }
    }
  }
  char downloadUrl[64];
  sprintf(downloadUrl, "http://blockswithscreens.appspot.com/api/gif?block=%d", randomId);

  while (true) {
    delay(5000);
    yield();
    ArduinoOTA.handle();
    start = millis();
    int bytesDownloaded = downloadFile(downloadUrl, "/out2.raw", "");
    if (bytesDownloaded > 100) {
      time = millis() - start;
      Serial.printf("Downloaded %d bytes in %dms!\n", bytesDownloaded, time);
      break;
    }
  }

  tft.println("Rendering file");
  start = millis();
  drawFSBmp("/out2.raw", 0, 0, 128, 128);
  time = millis() - start;
  Serial.printf("Render done in %dms\n", time);

  while (true) {
    yield();
    ArduinoOTA.handle();
    drawFSBmp("/out2.raw", 0, 0, 128, 128);
  }
}
