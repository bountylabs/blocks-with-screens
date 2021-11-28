#if defined(EMSCRIPTEN)
#include "Adafruit_SSD1351.h"
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

extern void setup();
extern void loop();

FakeSerial Serial;
FakeESP ESP;
void *SPI;

#define SCALE 2
#define WIDTH 128
#define HEIGHT 128

using namespace std;
using namespace emscripten;
SDL_Renderer *renderer;

int addr_x;
int addr_y;
int addr_w;
int addr_h;

int16_t _last_x_accel = 0;
int16_t _last_y_accel = 0;
int16_t _last_z_accel = 0;

uint16_t pixels565[128 * 128];

int last_millis;
int frame_count;

void Adafruit_SSD1351::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap,
                                     int16_t w, int16_t h) {
  memcpy(pixels565, bitmap, w * h * 2);
}

void Adafruit_SSD1351::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  pixels565[y*WIDTH + x] = color;
}

void Adafruit_SSD1351::setAddrWindow(int x, int y, int w, int h)
{
  addr_x = x;
  addr_y = y;
  addr_w = w;
  addr_h = h;
}

void Adafruit_SSD1351::SPI_WRITE16(uint16_t w)
{
  pixels565[addr_y*WIDTH + addr_x] = w;
}

void updateAccelerometer(int x, int y, int z)
{
  _last_x_accel = x;
  _last_y_accel = y;
  _last_z_accel = z;

  printf("got %d %d %d\n", x, y, z);
}

void getAccelerometerData(int16_t* x, int16_t* y, int16_t* z)
{
  *x = _last_x_accel;
  *y = _last_y_accel;
  *z = _last_z_accel;
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("updateAccelerometer", &updateAccelerometer);
}

void renderloop() {
  loop();

  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      uint16_t *pixel = &pixels565[y * WIDTH + x];
      Uint8 red = Uint8((*pixel & 0xF800) >> 8);
      Uint8 green = Uint8((*pixel & 0x07E0) >> 3);
      Uint8 blue = Uint8((*pixel & 0x001F) << 3);
      SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }
  SDL_RenderPresent(renderer);

  frame_count++;
  int time_now_millis = millis();

  if ((time_now_millis - last_millis) >= 1000)
  {
    printf("FPS: %d\n", frame_count);
    frame_count = 0;
    last_millis = time_now_millis;
  }
}

int main(int argc, char *argv[]) {
  memset(pixels565, 0, WIDTH * HEIGHT * 2);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "Failed SDL_Init " << SDL_GetError() << endl;
    return 1;
  }

  SDL_Window *window;

  SDL_CreateWindowAndRenderer(WIDTH * SCALE, WIDTH * SCALE, 0, &window,
                              &renderer);
  SDL_RenderSetScale(renderer, SCALE, SCALE);

  if (renderer == NULL) {
    fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
    return 1;
  }

  emscripten_set_main_loop(renderloop, 0, 0);

  millis_start();
  setup();
  last_millis = millis();
  frame_count = 0;

  return 0;
}
#endif
