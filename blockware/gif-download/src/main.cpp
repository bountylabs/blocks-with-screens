/***************************************************
  Example sketch for the Block with Screen
 ****************************************************/
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <qrcode.h>
#include "secrets.h"
#include <DefaultConfig.h>
#include <Colors.h>
#include <OTAUpdates.h>
#include <WifiHelper.h>

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
    OTAUpdates_handle();

    char buffer[256];
    size_t size = bmpFile.readBytes(buffer, sizeof(buffer));
    numBytes += size;
    tft.writePixels((uint16_t *)buffer, size / 2);
  }
  tft.endWrite();
  bmpFile.close();
  Serial.printf("Read %d bytes", numBytes);
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

  // Connect wifi to support OTA programming
  ConnectWifi(WIFI_SSID, WIFI_PASSWORD);

  // Configure OTA programming
  OTAUpdates_setup("gif-download");

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
  return time_random(startNum, endNum);
}

void loop() {
  uint16_t start;
  uint16_t time;

  int randomId = getRandomNumber(0, INT_MAX);
  char url[64];
  char *serverURL = "http://blockswithscreens.appspot.com";
  // char *serverURL = "http://10.0.0.130:3000";
  sprintf(url, "%s?block=%d", serverURL, randomId);

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
  sprintf(downloadUrl, "%s/api/gif?block=%d", serverURL, randomId);

  while (true) {
    delay(5000);
    yield();
    OTAUpdates_handle();
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
    OTAUpdates_handle();
    drawFSBmp("/out2.raw", 0, 0, 128, 128);
  }
}
