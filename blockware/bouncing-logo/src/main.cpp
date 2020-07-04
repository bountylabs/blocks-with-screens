// #define SERIALLOG 1
#include <DLog.h>

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <math.h>

#include "secrets.h"
#include <OTAUpdates.h>
#include <Random.h>
#include <TLLogos.h>
#include <Text.h>
#include <Vec2d.h>
#include <DefaultConfig.h>


Adafruit_SSD1351 tft =
  Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

std::vector<unsigned char*> Logos;

// in-memory 16-bit canvas
GFXcanvas16* canvas;
// screen dimensions
Vec2d<int> screen = Vec2d<int>();
const int FPS = floor(1000 / 60); // every ~16ms (60fps)
uint16_t lastLoop = millis() - FPS + 1;

int tlWidth = 40;
int tlHeight = 33;
// position x,y
Vec2d<int> position = Vec2d<int>();
// velocity in x and y direction
Vec2d<int> velocity = Vec2d<int>();
unsigned char* logo;

void flush()
{
  tft.drawRGBBitmap(0, 0, (uint16_t*)canvas->getBuffer(), screen.x, screen.y);
  // tft.startWrite();
  // SPI.writeBytes((uint8_t *)canvas->getBuffer(), 128 * 128 * 2);
  // tft.endWrite();
}

void drawLogo(const unsigned char* logo, int x, int y, int w, int h)
{
  canvas->drawRGBBitmap(x, y, (uint16_t*)logo, tlWidth, tlHeight);
}

void setup(void)
{
  Serial.begin(SERIAL_DATA_RATE);
  tft.begin(SPI_SPEED);

  tft.fillScreen(BLACK);

  // initialize srand
  randomSeed(ESP.getCycleCount());

  // ConnectWifi(WIFI_SSID, WIFI_PASSWORD);
  OTAUpdates_setup("bouncing-logo");

  // download logo binaries
  Logos.push_back(tl_blue);
  Logos.push_back(tl_green);
  Logos.push_back(tl_orange);
  Logos.push_back(tl_purple);
  Logos.push_back(tl_red);
  Logos.push_back(tl_yellow);

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
    unsigned char* nextLogo = Logos[floor(Logos.size() * random())];
    while (nextLogo == logo) {
      nextLogo = Logos[floor(Logos.size() * random())];
    }

    logo = nextLogo;

    // adjust velocity in terms of the normal direction
    if (abs(normal.x)) {
      velocity.x *= -1;
    }

    if (abs(normal.y)) {
      velocity.y *= -1;
    }
  }

  drawLogo(logo, position.x, position.y, tlWidth, tlHeight);
}

void loop()
{
  // OTAUpdates_handle();

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
