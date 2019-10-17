// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// Pin setup
#define SCLK_PIN 14 // D5 -- ESP8266 Hardware SCLK
#define MOSI_PIN 13 // D7 -- ESP8266 Hardware MOSI
// Note this is also the ESP8266 Hardware MISO pin! That seems to be a bad thing
// that we should fix in V2. See the bit about the hack below
#define DC_PIN 12  // D6
#define CS_PIN 16  // D0
#define RST_PIN 15 // D8

// serial data rate (115200 bits per second)
// https://www.arduino.cc/reference/en/language/functions/communication/serial/begin/115200
#define SERIAL_DATA_RATE 115200

// 15MHz SPI
// Note, the ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000

// #define SERIALLOG 1
#include <DLog.h>

#include <Adafruit_SSD1351.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <math.h>

#include "secrets.h"
#include <FS.h>
#include <OTAUpdates.h>
#include <Random.h>
#include <Text.h>
#include <Vec2d.h>
#include <WifiHelper.h>

// Software bit-banged SPI mode works just fine but is slow
// Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

// Hardware SPI mode only works if you make a hack in SPI.c under the hood. In
// short, find the two lines with `pinMode(MISO` in them and comment them out

Adafruit_SSD1351 tft =
  Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

std::vector<char*> Logos;

// in-memory 16-bit canvas
GFXcanvas16* canvas;
// screen dimensions
Vec2d<int> screen = Vec2d<int>();
const int FPS = floor(1000 / 60); // every ~16ms (60fps)
uint16_t lastLoop = millis() - FPS + 1;

int tlWidth = 38;
int tlHeight = 32;
// position x,y
Vec2d<int> position = Vec2d<int>();
// velocity in x and y direction
Vec2d<int> velocity = Vec2d<int>();
char* logo;

void flush()
{
  tft.drawRGBBitmap(0, 0, (uint16_t*)canvas->getBuffer(), screen.x, screen.y);
  // tft.startWrite();
  // SPI.writeBytes((uint8_t *)canvas->getBuffer(), 128 * 128 * 2);
  // tft.endWrite();
}

int downloadFile(const char* URL, const char* filepath, const char* fingerprint = NULL)
{
  HTTPClient http;
  if (fingerprint == NULL) {
    http.begin(URL, fingerprint);
  }
  else {
    http.begin(URL);
  }
  int httpCode = http.GET();

  Serial.printf("[HTTP] GET %s code: %d\n", URL, httpCode);

  if (httpCode != HTTP_CODE_OK) {
    const char* errorString = http.errorToString(httpCode).c_str();
    Serial.printf("[HTTP] GET... failed, error: %s\n", errorString);
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
  }
  else {
    Serial.printf("Wrote %d kb\n", bytesWritten / 1000);
    // tft.setTextColor(WHITE);
    // tft.printf("Wrote %d kb\n", bytesWritten / 1000);
  }

  f.close();
  http.end();

  return bytesWritten;
}

void formatSPIFFSIfNecessary(void)
{
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
    }
    else {
      f.println("Format Complete");
    }
  }
  else {
    Serial.println("SPIFFS is formatted. Moving along...");
    tft.setTextColor(GREEN);
    tft.println("SPIFFS is formatted");
  }
}

void drawFSBmp(const char* filename, int x, int y, int w, int h)
{
  Serial.print("Drawing file: ");
  Serial.println(filename);

  fs::File bmpFile = SPIFFS.open(filename, "r");

  if (!bmpFile) {
    Serial.print("ERROR: File \"");
    Serial.print(filename);
    Serial.println("\" not found!");
    return;
  }

  char buffer[tlWidth * tlHeight * 2];
  bmpFile.readBytes(buffer, sizeof(buffer));

  // for (int i = 0; i < tlWidth * tlHeight; i++) {
  //   int ix = (i % tlWidth) + x;
  //   int iy = floor(i / tlHeight) + y;
  //   uint8_t d1 = buffer[i * 2];
  //   uint8_t d2 = buffer[(i * 2) + 1];
  //   uint16_t rgb565 = (d2 << 8) | d1;
  //   canvas->writePixel(ix, iy, rgb565);
  // }

  canvas->drawRGBBitmap(x, y, (uint16_t*)buffer, tlWidth, tlHeight);

  bmpFile.close();
}

void setup(void)
{
  Serial.begin(SERIAL_DATA_RATE);
  tft.begin(SPI_SPEED);

  // initialize srand
  randomSeed(ESP.getCycleCount());

  displayReset();
  ConnectWifi(WIFI_SSID, WIFI_PASSWORD);
  OTAUpdates_setup("bouncing-logo");

  tft.setTextColor(WHITE);
  tft.println("Configuring SPIFFS...");
  SPIFFS.begin();
  // SPIFFS.format();
  formatSPIFFSIfNecessary();
  FSInfo fsInfo;
  SPIFFS.info(fsInfo);

  tft.setTextColor(GREEN);
  tft.printf("%d / %d kb used\n", fsInfo.usedBytes / 1000, fsInfo.totalBytes / 1000);

  // download logo binaries
  downloadFile("http://192.168.1.20:8080/tl-blue.bin", "/tl-blue.bin", "");
  Logos.push_back("/tl-blue.bin");
  downloadFile("http://192.168.1.20:8080/tl-green.bin", "/tl-green.bin", "");
  Logos.push_back("/tl-green.bin");
  downloadFile("http://192.168.1.20:8080/tl-orange.bin", "/tl-orange.bin", "");
  Logos.push_back("/tl-orange.bin");
  downloadFile("http://192.168.1.20:8080/tl-purple.bin", "/tl-purple.bin", "");
  Logos.push_back("/tl-purple.bin");
  downloadFile("http://192.168.1.20:8080/tl-red.bin", "/tl-red.bin", "");
  Logos.push_back("/tl-red.bin");
  downloadFile("http://192.168.1.20:8080/tl-yellow.bin", "/tl-yellow.bin", "");
  Logos.push_back("/tl-yellow.bin");

  // save dimensions
  screen.x = tft.width();
  screen.y = tft.height();

  // initialize canvas to all black
  canvas = new GFXcanvas16(screen.x, screen.y);
  canvas->fillScreen(BLACK);
  flush();

  // randomize starting logo
  logo = Logos[floor(Logos.size() * random())];

  // randomize starting position
  position.set(floor(random() * screen.x - tlWidth), floor(random() * screen.y - tlHeight));

  // randomize starting velocity (0 or 1)
  while (velocity == 0) {
    velocity.set((random() < 0.5 ? -1 : +1) * 2, (random() < 0.5 ? -1 : +1) * 2);
  }

  Serial.printf("\nFPS=%d", FPS);
}

void tick()
{
  canvas->fillScreen(BLACK);

  // calculate next position
  position += velocity;

  // detect boundary hit
  Vec2d<int> normal = Vec2d<int>();
  if (position.x >= screen.x - 1 - tlWidth) {
    normal.set(-1.0, normal.y);
  }
  else if (position.x <= 0) {
    normal.set(+1.0, normal.y);
  }

  if (position.y >= screen.y - 1 - tlHeight) {
    normal.set(normal.x, -1.0);
  }
  else if (position.y <= 0) {
    normal.set(normal.x, +1.0);
  }

  // no collision detected
  if (normal == 0) {
    // no-op
  }
  else if ((velocity.x * normal.x > 0) || (velocity.y * normal.y > 0)) {
    // Do not resolve if velocity is moving away from normal
  }
  else {
    // collision!
    // randomize logo
    logo = Logos[floor(Logos.size() * random())];

    // adjust velocity in terms of the normal direction
    if (abs(normal.x)) {
      velocity.x *= -1;
    }

    if (abs(normal.y)) {
      velocity.y *= -1;
    }
  }

  drawFSBmp(logo, position.x, position.y, tlWidth, tlHeight);
}

void loop()
{
  OTAUpdates_handle();

  uint16_t now = millis();
  uint16_t time = now - lastLoop;

  Serial.printf("frameMs=%d", time);

  if (time > FPS) {
    // track loop millis to keep steady fps
    lastLoop = now;

    // run!
    tick();

    // flush our in-memory canvas to the screen
    flush();
  }
}
