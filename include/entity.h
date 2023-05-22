#pragma once

#include <SDL.h>
#include <clm.h>

#include "resources.h"

typedef struct Entity Entity;
struct Entity {
  vec2 position;
  float rotation;
  float scale;
  SpriteSheet sprite;
  Animator animator;
  int visible;
};

void entity_draw(SDL_Renderer *renderer, Entity entity);