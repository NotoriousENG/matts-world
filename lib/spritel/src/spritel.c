#include "spritel.h"

SpriteSheet spriteSheet_new(SDL_Texture *texture, int frameWidth,
                            int frameHeight) {
  SpriteSheet spriteSheet;
  memset(&spriteSheet, 0, sizeof(SpriteSheet));
  spriteSheet.texture = texture;
  spriteSheet.frameWidth = frameWidth;
  spriteSheet.frameHeight = frameHeight;
  spriteSheet.animationCount = 1;
  return spriteSheet;
}

void destroySpriteSheet(SpriteSheet spriteSheet) {
  for (int i = 0; i < spriteSheet.animationCount; i++) {
    free(spriteSheet.animations[i].frames);
  }
  free(spriteSheet.animations);
  SDL_DestroyTexture(spriteSheet.texture);
};

Animator animator_new() {
  Animator animator;
  memset(&animator, 0, sizeof(Animator));
  animator.animation = 0;
  animator.time = 0;
  animator.currentFrame = 0;
  return animator;
}

void animator_process(Animator *animator, SpriteSheet sprite, float delta) {
  animator->time += delta;
  Animation animation = sprite.animations[animator->animation];
  if (animator->time > animation.frameSeconds) {
    animator->time -= animation.frameSeconds;
    animator->currentFrame++;
    if (animator->currentFrame >= animation.frameCount) {
      animator->currentFrame = 0;
    }
  }
}

SDL_Texture *loadTexture(SDL_Renderer *renderer, char *filename) {
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                 "Loading %s", filename);
  SDL_Texture *texture = IMG_LoadTexture(renderer, filename);

  if (renderer == NULL) {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                   "Failed to load %s RENDERER IS NULL", filename);
    exit(1);
  }

  if (texture == NULL) {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                   "Failed to load %s", filename);
    exit(1);
  }
  return texture;
}

void destroyTexture(SDL_Texture *texture) { SDL_DestroyTexture(texture); }
// pass null for srcrect to draw the whole texture
void drawTexture(SDL_Renderer *renderer, SDL_Texture *texture,
                 const SDL_Rect *srcrect, int x, int y, float rotation,
                 float scale) {
  SDL_Rect dest;

  dest.x = x;
  dest.y = y;
  SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
  if (srcrect != NULL) {
    dest.w = srcrect->w;
    dest.h = srcrect->h;
  }

  dest.w *= scale;
  dest.h *= scale;
  SDL_RenderCopyEx(renderer, texture, srcrect, &dest, rotation, NULL,
                   SDL_FLIP_NONE);
}