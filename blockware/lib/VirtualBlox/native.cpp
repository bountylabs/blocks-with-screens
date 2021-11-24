#include "Adafruit_SSD1351.h"
#include <SDL2/SDL.h>
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
SDL_Renderer *renderer;

uint16_t pixels565[128 * 128];

void Adafruit_SSD1351::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap,
                                     int16_t w, int16_t h) {
  memcpy(pixels565, bitmap, w * h * 2);
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

  millis_start();
  setup();

  SDL_Event e;
  bool quit = false;
  unsigned int lastRunTime = SDL_GetTicks();
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    // Frame limit so we don't murder battery unnecessarily
    unsigned int now = SDL_GetTicks();
    double delta = now - lastRunTime;
    if (delta > 1000 / 60.0) {
      lastRunTime = now;
      renderloop();
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  printf("Quit\n");
  return 0;
}
