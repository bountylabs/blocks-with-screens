/***************************************************
  Example sketch for the Block with Screen
 ****************************************************/

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <ESP8266HTTPClient.h>
#include <JPEGDecoder.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include "secrets.h"
#include <DefaultConfig.h>
#include <Colors.h>

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

int downloadFile(const char *URL, const char *filepath) {
  HTTPClient http;
  http.begin(URL);
  int httpCode = http.GET();
  int bytesDownloaded = 0;

  Serial.printf("[HTTP] GET %s code: %d\n", URL, httpCode);

  if(httpCode > 0) {
    // HTTP header has been sent and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // File found at server
    if(httpCode == HTTP_CODE_OK) {
      // Open a file for writing
      fs::File f = SPIFFS.open(filepath, "w");
      if (!f) {
          Serial.println("File open failed");
          tft.setTextColor(RED);
          tft.println("File open failed");
      }

      // Get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      uint8_t buff[128] = { 0 };
      WiFiClient *stream = http.getStreamPtr();

      // Read all data from server
      while (http.connected() && (len > 0 || len == -1)) {
        // Get available data size
        size_t size = stream->available();
        if (size) {
          // Read up to 128 byte
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          // Write it to Serial
          Serial.write(buff, c);
          f.write(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          bytesDownloaded += c;
          if (len > 0) {
            len -= c;
          }
        }
        yield();
      }
      f.close();

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

//====================================================================================
//   Decode and paint onto the TFT screen
//====================================================================================
void jpegRender(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;
  tft.setTextColor(WHITE);
  tft.printf("Image is %dx%d\n", max_x, max_y);

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = min(mcu_w, max_x % mcu_w);
  uint32_t min_h = min(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while (JpegDec.read()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right and bottom edges
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;

    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w) <= tft.width() && ( mcu_y + win_h) <= tft.height()) {
      tft.startWrite();

      // Now set a MCU bounding window on the TFT to push pixels into (x, y, x + width - 1, y + height - 1)
      // tft.setAddrWindow(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1);
      tft.setAddrWindow(mcu_x, mcu_y, win_w, win_h);

      // Write all MCU pixels to the TFT window
      // while (mcu_pixels--) tft.pushColor(*pImg++); // Send MCU buffer to TFT 16 bits at a time

      // tft.swapendian((uint8_t *)pImg, mcu_pixels*2);
      // tft.writedata((uint8_t *)pImg, mcu_pixels*2);
      tft.writePixels((uint16_t *)pImg, mcu_pixels);
      tft.endWrite();
    // Stop drawing blocks if the bottom of the screen has been reached,
    // the abort function will close the file
    } else if (( mcu_y + win_h) >= tft.height()) {
      Serial.printf("MCU block does not fit (abort) %d %d %d %d %d %d\n", mcu_x, win_w, tft.width(), mcu_y, win_h, tft.height());
      JpegDec.abort();
    } else {
      Serial.printf("MCU block does not fit %d %d %d %d %d %d\n", mcu_x, win_w, tft.width(), mcu_y, win_h, tft.height());
    }
  }
}

void drawFSJpeg(const char *filename, int xpos, int ypos) {
  Serial.print("Drawing file: "); Serial.println(filename);

  // Open the file (the Jpeg decoder library will close it)
  fs::File jpgFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS

  if (!jpgFile) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  boolean decoded = JpegDec.decodeFsFile(filename);
  if (decoded) {
    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  } else {
    Serial.println("Jpeg file format not supported!");
    tft.setTextColor(RED);
    tft.println("Jpeg file format not supported!");
  }
}

void setupArduinoOTA() {
  ArduinoOTA.setHostname("jpeg-example");
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
    } else {
        f.println("Format Complete");
    }
  } else {
    Serial.println("SPIFFS is formatted. Moving along...");
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
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.print(".");
  }
  tft.println("!");
  tft.setTextColor(GREEN);
  tft.println("Connected!");

  // Configure OTA programming
  setupArduinoOTA();

  tft.setTextColor(WHITE);
  tft.println("Configuring SPIFFS...");
  SPIFFS.begin();
  formatSPIFFSIfNecessary();
  tft.setTextColor(GREEN);
  tft.println("SPIFFS is ready!");

}

void loop() {
  tft.setTextColor(WHITE);
  tft.println("Downloading file");
  uint16_t start = millis();
  int bytesDownloaded = downloadFile("http://www.gravatar.com/avatar/b26354198c6e8a01b014a81810bafe36?s=128", "/out.jpg");
  uint16_t time = millis() - start;
  Serial.printf("Downloaded %d bytes in %dms!\n", bytesDownloaded, time);
  tft.setTextColor(GREEN);
  tft.printf("Downloaded %d bytes in %dms\n", bytesDownloaded, time);
  tft.setTextColor(WHITE);
  tft.println("Rendering file");
  start = millis();
  drawFSJpeg("/out.jpg", 0, 0);
  time = millis() - start;
  Serial.printf("Render done in %dms\n", time);
  // tft.setTextColor(GREEN);
  // tft.printf("Render done in %dms!\n", time);

  while (true) {
    yield();
    ArduinoOTA.handle();
    delay(1000);
  }
}
