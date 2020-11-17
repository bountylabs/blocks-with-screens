#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <FS.h>
#include <JPEGDecoder.h>
#include <ESP8266HTTPClient.h>

#include "constants.h"
#include "weather.h"
#include "httphelper.h"
#include "stocks.h"
#include <DefaultConfig.h>
#include <WifiHelper.h>
#include <OTAUpdates.h>
#include "secrets.h"
#include "constants.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

void getCurrentWeather(WiFiClientSecure client);
void getStockPrice(WiFiClientSecure client, String symbol);

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

float closePrice = 0;
float deltaPrice = 0;

String weatherCityName;
String weatherIconUrl;
float currentTempF;

void parseWeatherZipResponse(DynamicJsonDocument& docPtr);

void printAllCharacters() {
  tft.setTextColor(WHITE);
    for (int i = 0; i < 256; i++) {
      tft.setCursor(0,0);
      tft.print(i);
      tft.print(" ");
      tft.write(i);
      delay(500);
      tft.fillScreen(0x000000);
    }
  delay(60000);
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

int downloadFile(const char *URL, const char *filepath) {
  HTTPClient http;
  http.begin(URL, "08:3B:71:72:02:43:6E:CA:ED:42:86:93:BA:7E:DF:81:C4:BC:62:30");
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
        OTAUpdates_handle();
      }
      f.close();

      Serial.println();
      Serial.print("[HTTP] connection closed or file end.\n");
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
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

void setup() {
  Serial.begin(SERIAL_DATA_RATE);
  tft.begin();
  tft.fillScreen(0x000000);
  ConnectWifi(WIFI_SSID, WIFI_PASS);
  OTAUpdates_setup("passive-info");

  SPIFFS.begin();
  formatSPIFFSIfNecessary();
}

void loop() {
  WiFiClientSecure client;
  // *************** Stock **********************
  getStockPrice(client, "TWTR");
  tft.fillScreen(BLACK);
  drawStock(tft, "TWTR", closePrice, deltaPrice, 2, 10, 10);
  for (int i = 0; i < 30; i++) {
    OTAUpdates_handle();
    delay(1000);
  }
  // ***********************************************

  // TODO: At one time these could both run together. But it seems that we are
  // running out of memory now. Either the responses got bigger, or we have
  // a memory bug or both.

  // *************** Weather **********************
  // getCurrentWeather(client);
  // tft.fillScreen(WHITE);
  // int bytesDownloaded = downloadFile(weatherIconUrl.c_str(), "/out.jpg");
  // drawFSJpeg("/out.jpg", (tft.width() - 50) / 2, (tft.height() - 50) / 2);
  // drawWeather(tft, weatherCityName, weatherIconUrl, currentTempF);
  // for (int i = 0; i < 30; i++) {
  //   OTAUpdates_handle();
  //   delay(1000);
  // }
  // ***********************************************
}

void getCurrentWeather(WiFiClientSecure client) {
  bool successful = false;
  successful = ConnectToSecureHost(&client, HOST, HTTPS_PORT);
  if (!successful) {
    return;
  }

  String url = getWeatherUrlForZipCountry("94103", "us");
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(getHttpGETRequestWithUrlAndHost(HOST, url));
  delay(300);

  String line;
//  Serial.println("is client available: " + String(client.available()));
  while(client.available()){
    line = client.readStringUntil('\r');
    Serial.println(line);
  }

  DynamicJsonDocument doc(10048);
  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  parseWeatherZipResponse(doc);
}

void getStockPrice(WiFiClientSecure client, String symbol) {
  bool successful = false;
  successful = ConnectToSecureHost(&client, HOST, HTTPS_PORT);
  if (!successful) {
    return;
  }

  String url = getAlphaVUrlForTimeSeriesDaily("TWTR");
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(getHttpGETRequestWithUrlAndHost(HOST, url));
  int counter = 0;
  while(!client.available()) {
    Serial.println("Response is not available yet: " + String(++counter));
    delay(500);
  }

  String line;
  while(client.available()){
    line = client.readStringUntil('\r');
    Serial.println(line);
  }

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  const char* symbolFromAPI = doc["symbol"];
  const float openPrice = doc["open"];
  closePrice = doc["close"];
  const float lowPrice = doc["low"];
  const float highPrice = doc["high"];
  deltaPrice = closePrice - openPrice;

  Serial.println("Symbol: " + String(symbolFromAPI));
  Serial.println("Open Price: " + String(openPrice));
  Serial.println("Close Price: " + String(closePrice));
  Serial.println("Low Price: " + String(lowPrice));
  Serial.println("High Price: " + String(highPrice));
}

void parseWeatherZipResponse(DynamicJsonDocument& doc) {
  const char* cityName = doc["city_name"];
  const char* mainDescription = doc["main_description"];
  const char* secondaryDescription = doc["secondary_description"];
  const char* iconUrl = doc["icon_url"];
  const float tempF = convertKelvinToFahrenheit(doc["current_temp"]);
  const float minTemp = doc["temp_min"];
  const float maxTemp = doc["temp_max"];

  weatherCityName = String(cityName);
  weatherIconUrl = String(iconUrl);
  currentTempF = tempF;
  
  Serial.println("City: " + String(weatherCityName));
  Serial.println("Temp: " + String(currentTempF) + " F");
  Serial.println("Min Temp: " + String(convertKelvinToFahrenheit(minTemp)) + " F");
  Serial.println("Max Temp: " + String(convertKelvinToFahrenheit(maxTemp)) + " F");
  Serial.println("Current Weather: " + String(mainDescription));
  Serial.println("Secondary Description: " + String(secondaryDescription));
  Serial.println("Icon Url:" + String(weatherIconUrl));
}
