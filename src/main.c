#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "scene.h"
#include <chroma.h>

#include "resources.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Resources resources;
static Scene scene;
static float currentTime = 0;
static float delta = 0;
static color_rgb clear_color = {125, 255, 134};

void cleanup() {

  scene_free(&scene);

  free_resources(resources);

  TTF_Quit();

  Mix_Quit();

  SDL_DestroyRenderer(renderer);

  SDL_DestroyWindow(window);

  IMG_Quit();

  SDL_Quit();
}

void main_loop() {
  float lastTime = SDL_GetTicks() / 1000.0f;
  float delta = lastTime - currentTime;
  currentTime = lastTime;

  SDL_RenderClear(renderer);

  doInput(resources.keyboard);

  scene_logic(&scene, delta);
  scene_draw(renderer, &scene);

  SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
  // Boilerplate code for SDL2

  int rendererFlags, windowFlags;

  rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

  windowFlags = 0;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  // frequency of 44100 (CD quality), the default format, 2 channels (stereo)
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    printf("Couldn't initialize SDL Mixer\n");
    exit(1);
  }

  if (TTF_Init() < 0) {
    printf("Couldn't initialize SDL TTF\n");
    exit(1);
  }

  Mix_AllocateChannels(MAX_SOUND_CHANNELS);

  window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCALED_SCREEN_WIDTH,
                            SCALED_SCREEN_HEIGHT, windowFlags);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

  renderer = SDL_CreateRenderer(window, -1, rendererFlags);

  if (!renderer) {
    printf("Failed to create renderer: %s\n", SDL_GetError());
    exit(1);
  }

  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0) {
    printf("Couldn't initialize SDL Image\n");
    exit(1);
  }

  if (SDL_NumJoysticks() < 1) {
    printf("No joysticks connected\n");
  }
  // End of boilerplate code

  resources = load_resources(renderer);

  // play_music_on_loop(resources.music);

  atexit(cleanup);

  scene = scene_new(&resources);
  scene_begin(&scene);

  resources.running = 1;

  SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b,
                         255);

  // scale all rendering
  SDL_RenderSetScale(renderer, RENDER_SCALE, RENDER_SCALE);
  // set blend mode
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(main_loop, 0, resources.running);
#else
  while (resources.running) {
    main_loop();
  }
#endif

  return 0;
}