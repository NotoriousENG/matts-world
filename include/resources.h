#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <sinput.h>
#include <spritel.h>

#include "defs.h"

typedef struct Resources Resources;

struct Resources {
  SpriteSheet joey;
  int keyboard[MAX_KEYBOARD_KEYS];
  SDL_Joystick *joystick;
  int running;
};

Resources load_resources(SDL_Renderer *renderer);

void free_resources(Resources resources);