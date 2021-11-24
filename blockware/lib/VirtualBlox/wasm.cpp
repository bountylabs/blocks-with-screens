#if defined(EMSCRIPTEN)
#include "Adafruit_SSD1351.h"
#include <SDL/SDL.h>
#include <emscripten.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

extern void setup();
extern void loop();

FakeSerial Serial;
FakeESP ESP;
void *SPI;

#define WIDTH 128
#define HEIGHT 128

using namespace std;
SDL_Surface *sim_screen;
int cc;

uint16_t pixels565[128 * 128];

void Adafruit_SSD1351::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap,
                                     int16_t w, int16_t h) {
  memcpy(pixels565, bitmap, w * h * 2);
}

void renderloop() {
  loop();
  SDL_Flip(sim_screen);
  SDL_LockSurface(sim_screen);

  int bpp = sim_screen->format->BytesPerPixel;
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      Uint8 *p = (Uint8 *)sim_screen->pixels + y * sim_screen->pitch + x * bpp;
      uint16_t *pixel = &pixels565[y * 128 + x];

      p[0] = Uint8((*pixel & 0xF800) >> 8); // red
      p[1] = Uint8((*pixel & 0x07E0) >> 3);
      p[2] = Uint8((*pixel & 0x001F) << 3);
    }
  }
  SDL_UnlockSurface(sim_screen);
}

int main(int argc, char *argv[]) {
  cc = 0;

  memset(pixels565, 0, 128 * 128 * 2);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "Failed SDL_Init " << SDL_GetError() << endl;
    return 1;
  }

  sim_screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_ANYFORMAT);
  if (sim_screen == NULL) {
    cout << "Failed SDL_SetVideoMode: " << SDL_GetError() << endl;
    SDL_Quit();
    return 1;
  }

  emscripten_set_main_loop(renderloop, 0, 0);

  millis_start();
  setup();

  // renderloop();
  return 0;
}
#endif
