#include <string.h>
#include <SDL/SDL.h>
#include <iostream>
#include <emscripten.h>
#include "test.h"

#define WIDTH 320
#define HEIGHT 200
 
using namespace std;
SDL_Surface *screen;
int cc;

void renderloop() {
  SDL_Flip(screen);
  SDL_LockSurface(screen);
    
    int bpp = screen->format->BytesPerPixel;

    for (int x = 0; x < WIDTH; x++)
    {
      for (int y = 0; y < HEIGHT; y++)
      {
        Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;
        p[0] = cc;
        p[1] = 0;
        p[2] = 0;
        p[3] = 255;
      }
    }
  
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    if(event.key.keysym.sym == SDLK_c     ) { SDL_FillRect(screen,NULL,0); }
  }
  
  SDL_UnlockSurface(screen);
  cc = (cc + 1) % 256;
}

int main(int argc, char *argv[]) {
  cc = 0;
  
  if(SDL_Init(SDL_INIT_VIDEO)<0) {
    cout << "Failed SDL_Init " << SDL_GetError() << endl;
    return 1;
  }
  
  screen=SDL_SetVideoMode(WIDTH,HEIGHT,32,SDL_ANYFORMAT);
  if(screen==NULL) {
    cout << "Failed SDL_SetVideoMode: " << SDL_GetError() << endl;
    SDL_Quit();
    return 1;
  }
  
  emscripten_set_main_loop(renderloop,0,0);
  printf("hey\n");
  
  test();

  return 0;
}