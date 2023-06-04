#pragma once

#include <SDL.h>
#include <SDL_image.h>

typedef struct Animation {
  SDL_Rect *frames;
  int frameCount;
  float frameSeconds;
} Animation;

typedef struct SpriteSheet {
  SDL_Texture *texture;
  Animation *animations;
  int animationCount;
  int frameWidth;
  int frameHeight;
  float frameSeconds;
} SpriteSheet;

typedef struct Animator {
  int animation;
  float time;
  int currentFrame;
} Animator;

SpriteSheet spriteSheet_new(SDL_Texture *texture, int frameWidth,
                            int frameHeight);

void destroySpriteSheet(SpriteSheet spriteSheet);

Animator animator_new();

void animator_process(Animator *animator, SpriteSheet sprite, float delta,
                      int stopLoopingOnIdleFrames);

SDL_Texture *loadTexture(SDL_Renderer *renderer, char *filename);

void destroyTexture(SDL_Texture *texture);

void drawTexture(SDL_Renderer *renderer, SDL_Texture *texture,
                 const SDL_Rect *srcrect, int x, int y, float rotation,
                 float scale);