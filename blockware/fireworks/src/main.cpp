// #define SERIALLOG 1
#include <DLog.h>

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <math.h>

#include <Random.h>
#include <TLLogos.h>
#include <Text.h>
#include <Particle.h>
#include <DefaultConfig.h>


const int tlWidth = 40;
const int tlHeight = 33;
const float FIREWORK_CHANCE = 0.05;
const int FIREWORKS_PER_FRAME = 64;
const char *username = "@magusnn";
const char *years = "5 years";


Adafruit_SSD1351 tft =
  Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

std::vector<unsigned char*> Logos;
std::vector<Particle> Particles;

// in-memory 16-bit canvas
GFXcanvas16* canvas;
// screen dimensions
Vec2d<int> screen = Vec2d<int>();
const int FPS = floor(1000 / 30); // every ~16ms (60fps)
uint16_t lastLoop = millis() - FPS + 1;

// logo position x,y
Vec2d<int> position = Vec2d<int>();
// years settings
uint8_t sizeYears = 2;
Vec2d<int> posYears = Vec2d<int>();
// username settings
uint8_t sizeUsername = 2;
Vec2d<int> posUsername = Vec2d<int>();

unsigned char* logo;


// NOTE: `loop`, `setup` and `flush` are sort of common operations
// Modify `initialize` and `tick` to customize this demo

void drawLogo()
{
    canvas->drawRGBBitmap(position.x, position.y, (uint16_t*)logo, tlWidth, tlHeight);
}

void drawText()
{
  canvas->setTextColor(WHITE);

  canvas->setTextSize(sizeYears);
  canvas->setCursor(posYears.x, posYears.y);
  canvas->print(years);

  canvas->setTextSize(sizeUsername);
  canvas->setCursor(posUsername.x, posUsername.y);
  canvas->print(username);

  // // test font sizes
  // for (int i = 0; i < 6; i++) {
  //   canvas->setCursor(0, i * 20);
  //   canvas->setTextColor(RED);
  //   canvas->setTextSize(i);
  //   canvas->println("Hello World!");
  // }
}

void centerText(const char* text, Vec2d<int> &posVec, uint8_t *size, int yTop, int yBottom)
{
  int16_t  x1, y1;
  uint16_t textWidth, textHeight;

  // start at size 2
  canvas->setTextSize(*size);
  canvas->getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);

  // text wider than screen? drop down to size 1
  if (textWidth > 100) {
    *size -= 1;
    canvas->setTextSize(*size);
    canvas->getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);
  }

  // center text
  int16_t textX = max(0, (int) floor(screen.x / 2 - textWidth / 2));
  int16_t textY = yTop + max(0, (int) floor((yBottom - yTop) / 2 - textHeight / 2));
  posVec.set(textX, textY);
}

void randomizeLogo()
{
  // randomize logo
  unsigned char* nextLogo = Logos[floor(Logos.size() * randomf())];
  while (nextLogo == logo) {
    nextLogo = Logos[floor(Logos.size() * randomf())];
  }
  logo = nextLogo;
}

void drawParticles()
{
  // randomly add new particles rocketing
  // if (randomf() < FIREWORK_CHANCE) {

  // add new firework when all particles are done
  if (Particles.size() < 1) {
    Particles.push_back(Particle(randomf() * screen.x, screen.y, true, randomFireworkColor()));
  }

  // explode and destroy old entities
  for (int i = 0, max = (int) Particles.size(); i < max; i++) {
    Particle* p = &Particles[i];
    if (p->destroy() || p->explode(Particles)) {
      Particles.erase(Particles.begin() + i);
    }
  }

  // tick and draw
  for (int i = 0, max = (int) Particles.size(); i < max; i++) {
    Particle* p = &Particles[i];
    p->tick();
    p->draw(canvas);
  }

  // for (int i = 0; i < FIREWORKS_PER_FRAME; i++) {
  //   int color = FIREWORK_COLORS[floor(FIREWORK_COLORS.size() * random())];
  //   int x = floor(screen.x * random());
  //   int y = floor(screen.y * random());
  //   canvas->drawPixel(x, y, color);
  // }
}

void flush()
{
  // write canvas to screen
  tft.drawRGBBitmap(0, 0, (uint16_t*)canvas->getBuffer(), screen.x, screen.y);
  // tft.startWrite();
  // SPI.writeBytes((uint8_t *)canvas->getBuffer(), 128 * 128 * 2);
  // tft.endWrite();
}

void initialize() {
  // download logo binaries
  Logos.push_back(tl_blue);
  Logos.push_back(tl_green);
  Logos.push_back(tl_orange);
  Logos.push_back(tl_purple);
  Logos.push_back(tl_red);
  Logos.push_back(tl_yellow);


  // randomize starting logo
  randomizeLogo();

  // center logo
  position.set(floor(screen.x / 2 - tlWidth / 2), floor(screen.y / 2 - tlHeight / 2));

  // center text
  centerText(username, posUsername, &sizeUsername, 0, position.y);
  centerText(years, posYears, &sizeYears, position.y + tlHeight, 128);

  // base
  drawLogo();
  drawText();
}

void tick()
{
  // clear entire screen
  canvas->fillScreen(BLACK);

  // redraw entire canvas on every tick

  // base
  drawLogo();
  drawText();
  drawParticles();
}

void loop()
{
  uint16_t now = millis();
  uint16_t time = now - lastLoop;

  // Serial.printf("frameMs=%d\n", time);

  if (time > FPS) {
    // track loop millis to keep steady fps
    lastLoop = now;

    // run!
    tick();

    // flush our in-memory canvas to the screen
    flush();
  }
}

void setup(void)
{
  Particles.reserve(23);

  Serial.begin(SERIAL_DATA_RATE);
  tft.begin(SPI_SPEED);
  // initialize screen to black
  tft.fillScreen(BLACK);

  // initialize srand
  randomSeed(ESP.getCycleCount());

  // save dimensions
  screen.x = tft.width();
  screen.y = tft.height();

  // initialize canvas to all black
  canvas = new GFXcanvas16(screen.x, screen.y);
  canvas->fillScreen(BLACK);

  // initialize contains demo specific setup
  initialize();

  flush();
}
