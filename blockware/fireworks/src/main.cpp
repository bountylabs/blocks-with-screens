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

#include <LIS2DW12Sensor.h>
#include <optional>

const int tlWidth = 40;
const int tlHeight = 33;
const char *username = "@twitter";
const char *years = "5 years";


Adafruit_SSD1351 tft =
  Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

std::vector<Particle> Particles;
std::optional<Particle> currRocket;

// in-memory 16-bit canvas
GFXcanvas16* canvas;
// screen dimensions
Vec2d<int> screen = Vec2d<int>();
const int FPS = floor(1000.0f / 30.0f); // (30fps)
uint16_t lastLoop = millis() - FPS + 1;

// logo position x,y
Vec2d<int> position = Vec2d<int>();
// years settings
uint8_t sizeYears = 2;
Vec2d<int> posVisYears = Vec2d<int>();
Vec2d<int> posHidYears = Vec2d<int>();
Vec2d<int> posYears = Vec2d<int>();
int yPosYears;
// username settings
uint8_t sizeUsername = 2;
Vec2d<int> posVisUsername = Vec2d<int>();
Vec2d<int> posHidUsername = Vec2d<int>();
Vec2d<int> posUsername = Vec2d<int>();
// text animation
float textAnimSpeed = 0.2f;
uint16_t textDisplayDuration = 10000;
uint16_t textCoolOffDuration = 1500;

// logo color animation
unsigned long lastLogoColorUpdate;
uint16_t sourceLogoColor = 0x0000;
uint16_t nextLogoColor = LOGO_BLUE;
uint16_t logoColorStep;
uint16_t defaultLogoColorSteps = 80;
uint16_t fastLogoColorSteps = 40;
uint16_t totalLogoColorSteps = defaultLogoColorSteps;

// Accelerometer
LIS2DW12Sensor* acc;
uint32_t accState;
bool wakeupText = true;
bool wakeupAll = true;
unsigned long lastWakeUpdate;
unsigned long lastTextEnabled;
unsigned long lastTextUpdate;
unsigned int wakeMaxDuration = 90000;

// NOTE: `loop`, `setup` and `flush` are sort of common operations
// Modify `initialize` and `tick` to customize this demo

void pickNextLogoColor() {
  if (wakeupAll && (nextLogoColor <= 0 || sourceLogoColor <= 0)) {
    nextLogoColor = std::max(sourceLogoColor, nextLogoColor);
    sourceLogoColor = 0x0000;
  }
  
  if (wakeupAll || nextLogoColor <= 0) {
    while(sourceLogoColor == nextLogoColor || nextLogoColor <= 0) {
      nextLogoColor = randomLogoColor();
    }
  } else {
    nextLogoColor = 0x0000;
  }
}

uint16_t getLogoColor() {
  // When asleep, only keep color logo on screen for 2s, otherwise 10s
  unsigned int onScreenTime = sourceLogoColor > 0 && !wakeupAll ? 2000 : 10000;
  
  // If we've just woken up, set logo color to animate in quickly
  if (wakeupAll && (nextLogoColor == 0 || sourceLogoColor == 0)) {
    uint16_t currStep = nextLogoColor == 0 ? (totalLogoColorSteps - logoColorStep) : logoColorStep;
    logoColorStep = (currStep * fastLogoColorSteps / totalLogoColorSteps) + 1; // Add 1 to make sure we animate
    totalLogoColorSteps = fastLogoColorSteps;
    pickNextLogoColor();
  }

  if (logoColorStep >= totalLogoColorSteps) {
    totalLogoColorSteps = defaultLogoColorSteps;
    sourceLogoColor = nextLogoColor;
    lastLogoColorUpdate = millis();
    logoColorStep = 0;
  } else if (logoColorStep > 0) {
    // Update color to next step
    logoColorStep += 1;
    return getColorMix(sourceLogoColor, nextLogoColor, totalLogoColorSteps, logoColorStep);
  } else if (millis() - lastLogoColorUpdate > onScreenTime) {
    // It's time to start animating the color change
    pickNextLogoColor();
    logoColorStep = 1;
  }

  return sourceLogoColor;
}

void drawLogo()
{
  uint16_t logoColor = getLogoColor();
  // Draw Logo 1 pixel at a time to allow changing the color
  int16_t larryStartY = (SCREEN_HEIGHT - tlHeight) / 2;
  int16_t larryStartX = (SCREEN_WIDTH - tlWidth) / 2;
  for (uint8_t y = 0; y < tlHeight; y++) {
    for (uint8_t x = 0; x < tlWidth; x++) {
      uint16_t color = ((uint16_t *)tl_blue)[y * tlWidth + x];
      if (color > 0) {
        uint16_t c = colorize(color, logoColor);
        canvas->drawPixel(larryStartX + x, larryStartY + y, c);
      }
    }
  }
}

void animateText(Vec2d<int> &posVec, Vec2d<int> &posVisVec, Vec2d<int> &posHidVec)
{
  int targetPos = wakeupText ? posVisVec.y : posHidVec.y;

  if (posVec.y != targetPos) {
    // lerp
    float nextPos = ((1.0f - textAnimSpeed) * posVec.y) + (textAnimSpeed * targetPos);

    // We need to call ceil or floor based on the relative direction of travel or we'll
    // never quite reach the target value.
    posVec.y = targetPos > posVec.y ? ceil(nextPos) : floor(nextPos);
  }
}

void drawText()
{
  animateText(posYears, posVisYears, posHidYears);
  animateText(posUsername, posVisUsername, posHidUsername);

  canvas->setTextColor(WHITE);

  canvas->setTextSize(sizeYears);
  canvas->setCursor(posYears.x, posYears.y);
  canvas->print(years);

  canvas->setTextSize(sizeUsername);
  canvas->setCursor(posUsername.x, posUsername.y);
  canvas->print(username);
}

void centerText(const char* text, Vec2d<int> &posVisVec, Vec2d<int> &posHidVec, uint8_t *size, int yTop, int yBottom)
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
  int16_t textX = std::max(0, (int) floor(screen.x / 2 - textWidth / 2));
  int16_t textY = yTop + std::max(0, (int) floor((yBottom - yTop) / 2 - textHeight / 2));
  posVisVec.set(textX, textY);

  int16_t hidTextY = textY > (SCREEN_HEIGHT / 2) ? SCREEN_HEIGHT : 0 - textHeight;
  posHidVec.set(textX, hidTextY);
}

void drawParticles()
{
  if (!currRocket) {
    // random chance to add new firework if there's no rocket and particles count is below threshold
    if (wakeupAll && Particles.size() < 80 && randomf() <= 0.1) {
      currRocket = Particle(randomf() * screen.x, screen.y, true, randomFireworkColor());
    }
  } else if (currRocket.value().explode(Particles)) {
    // clear the optional rocket value once it's exploded, making room for another to be fired
    currRocket.reset();
  } else {
    // Tick and draw rocket
    Particle* rocket = &currRocket.value();
    rocket->tick();
    rocket->draw(canvas);
  }

  // Destroy old firework particles
  for (int i = (int) Particles.size() - 1; i >= 0; i--) {
    Particle* p = &Particles[i];
    if (p->destroy()) {
      Particles.erase(Particles.begin() + i);
    }
  }

  // Tick and draw firework particles
  for (int i = 0, max = (int) Particles.size(); i < max; i++) {
    Particle* p = &Particles[i];
    p->tick();
    p->draw(canvas);
  }
}

void updateWakeState()
{
  unsigned long now = millis();
  unsigned long timeSinceUpdate = now - lastWakeUpdate;

  if (timeSinceUpdate > wakeMaxDuration) {
    wakeupAll = false;
  }
  if (wakeupText && now - lastTextEnabled > textDisplayDuration) {
    lastTextUpdate = now;
    wakeupText = false;
  }

  // Read accelerometer
  int16_t axes[3];
  acc->Get_X_AxesRaw(axes);
  // Scale values down to be less sensitive
  uint32_t newAccState = (axes[0] / 256 << 16) | (axes[1] / 256 << 8) | axes[2] / 256;

  // Update wake state
  bool changed = newAccState != accState;
  if (changed) {
    if (now - lastTextUpdate > textCoolOffDuration) {
      lastTextEnabled = now;
      wakeupText = true;
    }
    lastTextUpdate = now;
    wakeupAll = true;
    lastWakeUpdate = now;
    accState = newAccState;
  }
}

void initText()
{
  centerText(username, posVisUsername, posHidUsername, &sizeUsername, 0, position.y);
  centerText(years, posVisYears, posHidYears, &sizeYears, position.y + tlHeight, 128);
  posYears.x = posHidYears.x;
  posYears.y = posHidYears.y;
  posUsername.x = posHidUsername.x;
  posUsername.y = posHidUsername.y;
}

void flush()
{
  // write canvas to screen
  tft.drawRGBBitmap(0, 0, (uint16_t*)canvas->getBuffer(), screen.x, screen.y);
}

void initialize() {
  // reserve particles memory space
  Particles.reserve(EXPLOSION_PARTICLES * 2);

  // center logo
  position.set(floor(screen.x / 2 - tlWidth / 2), floor(screen.y / 2 - tlHeight / 2));

  // base
  initText();
  drawLogo();
  drawText();
}

void tick()
{
  // clear entire screen
  canvas->fillScreen(BLACK);

  // redraw entire canvas on every tick

  // base
  updateWakeState();
  drawParticles();
  drawLogo();
  drawText();
}

void loop()
{
  uint16_t now = millis();
  uint16_t time = now - lastLoop;

  //Serial.printf("frameMs=%d\n", time);
  //Serial.printf("FPS=%g\n", floor(1000.0f / (float)time));

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
  Serial.begin(SERIAL_DATA_RATE);
  tft.begin(SPI_SPEED);
  // initialize screen to black
  tft.fillScreen(BLACK);

  // save dimensions
  screen.x = tft.width();
  screen.y = tft.height();

  // initialize canvas to all black
  canvas = new GFXcanvas16(screen.x, screen.y);
  canvas->fillScreen(BLACK);

  // initialize srand
  randomSeed(ESP.getCycleCount());

  // setup accelerometer
  Wire.begin();
  acc = new LIS2DW12Sensor(&Wire, 0x31U);
  int begin_retval = acc->begin();
  Serial.printf("begin returned: %d\n", begin_retval);
  acc->Enable_X();
  // Max is 700000 in 160MHz mode and 400000 in 80MHz mode
  Wire.setClock(700000); // Run I2C at 700 KHz for faster screen updates

  // initialize contains demo specific setup
  initialize();

  flush();
}

