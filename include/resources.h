#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <clm.h>
#include <cute_tiled.h>
#include <sinput.h>
#include <spritel.h>

#include "defs.h"

typedef struct Entity Entity; // forward declaration

typedef struct Debug {
  int collisions;
} Debug;

typedef struct Tilemap {
  cute_tiled_map_t *map;
  SDL_Texture *texture; // this should be managed somewhere else
} Tilemap;

typedef struct Camera {
  vec2 position;
} Camera;

typedef struct Resources {
  TTF_Font *font;
  SpriteSheet joey;
  Tilemap map;
  KeyState keyboard[MAX_KEYBOARD_KEYS];
  SDL_Joystick *joystick;
  int running;
  Debug debug;
  Camera mainCamera;
} Resources;

Resources load_resources(SDL_Renderer *renderer);

void free_resources(Resources resources);

TTF_Font *loadFont(char *filename, int size);

// draw text with SDL_ttf with specified font
void drawText(SDL_Renderer *renderer, char *text, int x, int y, TTF_Font *font);

Tilemap load_tiled_map(SDL_Renderer *renderer, Resources *resources,
                       const char *path);

void free_tiled_map(Tilemap map);

void draw_tiled_map(SDL_Renderer *renderer, Tilemap map, Camera camera,
                    int debug_collisions);

void handle_tilemap_collisions(Entity *entity, Tilemap map);

void set_position_from_tilemap(vec2 *position, char *objectType, Tilemap map,
                               vec2 offset);