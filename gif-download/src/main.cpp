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
  int bytesDownloaded = 0;

  Serial.printf("[HTTP] GET %s code: %d\n", URL, httpCode);

  if(httpCode > 0) {
    // HTTP header has been sent and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // File found at server
    if(httpCode == HTTP_CODE_OK) {

      if (SPIFFS.exists(filepath)) {
        tft.setTextColor(WHITE);
        tft.printf("Deleting %s\n", filepath);
        SPIFFS.remove(filepath);
      }
      // Open a file for writing
      fs::File f = SPIFFS.open(filepath, "w");
      if (!f) {
          Serial.println("File open failed");
          tft.setTextColor(RED);
          tft.print("File open failed");
          return 0;
      }

      // Get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      const int total = len;
      uint8_t buff[128] = { 0 };
      WiFiClient *stream = http.getStreamPtr();

      tft.setTextColor(WHITE);
      tft.print("Downloading\n");
      int cursorX = tft.getCursorX();
      int cursorY = tft.getCursorY();
      int kbDownloaded = 0;

      // Read all data from server
      while (http.connected() && (len > 0 || len == -1)) {
        // Get available data size
        size_t size = stream->available();
        if (size) {
          // Read up to buffer size
          int bytesToWrite = ((size > sizeof(buff)) ? sizeof(buff) : size);
          int c = stream->readBytes(buff, bytesToWrite);
          int sizeWritten = f.write(buff, bytesToWrite);
          if (f.getWriteError()) {
            tft.println();
            tft.setTextColor(RED);
            tft.printf("File write error %d\n", f.getWriteError());
            Serial.printf("File write error %d\n", f.getWriteError());
            f.close();
            return bytesDownloaded;
          } else if (sizeWritten != bytesToWrite) {
            tft.println();
            tft.setTextColor(RED);
            tft.printf("Write size mismatch %d %d\n", sizeWritten, bytesToWrite);
            Serial.printf("Write size mismatch %d %d\n", sizeWritten, bytesToWrite);
            f.close();
            return bytesDownloaded;
          }
          bytesDownloaded += c;
          if (len > 0) {
            len -= c;
          }
          if (bytesDownloaded / 1000 != kbDownloaded) {
            kbDownloaded = bytesDownloaded / 1000;
            tft.setTextColor(WHITE, BLACK);
            tft.setCursor(cursorX, cursorY);
            tft.printf("%d kb", kbDownloaded);
          }
        }
        yield();
        ArduinoOTA.handle();
      }
      f.flush();
      f.close();

      tft.println("");
      Serial.println();
      Serial.print("[HTTP] connection closed or file end.\n");
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      tft.setTextColor(RED);
      tft.printf("HTTP Status: %d\n", httpCode);
    }
  }
  http.end();
  return bytesDownloaded;
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
  tft.println("Connecting to WiFi...");

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
    if ((millis() - time) > 5000) {
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

void loop() {
  uint16_t start;
  uint16_t time;

  tft.setTextColor(WHITE);
  tft.println("Downloading file");
  start = millis();
  // int bytesDownloaded = downloadFile("http://10.0.0.20:3000/api/gif?block=johnsgifs", "/out2.raw", "");
  // int bytesDownloaded = downloadFile("https://blockswithscreens.now.sh/api/gif?block=johnsgifs", "/out2.raw", "50 C4 95 BF 61 81 79 51 F3 DE 80 AB B6 DF 6D 31 54 57 AA C4");
  int bytesDownloaded = downloadFile("http://blockswithscreens.appspot.com/api/gif?block=johnsgifs", "/out2.raw", "");
  time = millis() - start;
  Serial.printf("Downloaded %d bytes in %dms!\n", bytesDownloaded, time);
  tft.setTextColor(GREEN);
  tft.printf("Downloaded %d bytes in %dms\n", bytesDownloaded, time);
  tft.setTextColor(WHITE);

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
