#include "metadata.h"

SpriteSheet buildJoeySheet(SDL_Texture *texture) {
  SpriteSheet joeySheet;
  memset(&joeySheet, 0, sizeof(SpriteSheet));
  joeySheet.texture = texture;
  joeySheet.frameWidth = 32;
  joeySheet.frameHeight = 48;
  joeySheet.animationCount = 4;
  joeySheet.animations = malloc(sizeof(Animation) * joeySheet.animationCount);
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                 "Allocated %d animations", joeySheet.animationCount);
  for (int i = 0; i < joeySheet.animationCount; i++) {
    joeySheet.animations[i].frameCount = 4;
    joeySheet.animations[i].frameSeconds = 0.25f;
    joeySheet.animations[i].frames =
        malloc(sizeof(SDL_Rect) * joeySheet.animations[i].frameCount);
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                   "Allocated %d frames for animation %d",
                   joeySheet.animations[i].frameCount, i);
  }

  joeySheet.animations[0].frames[0] = (SDL_Rect){0, 0, 32, 48};
  joeySheet.animations[0].frames[1] = (SDL_Rect){32, 0, 32, 48};
  joeySheet.animations[0].frames[2] = (SDL_Rect){64, 0, 32, 48};
  joeySheet.animations[0].frames[3] = (SDL_Rect){96, 0, 32, 48};

  joeySheet.animations[1].frames[0] = (SDL_Rect){0, 48, 32, 48};
  joeySheet.animations[1].frames[1] = (SDL_Rect){32, 48, 32, 48};
  joeySheet.animations[1].frames[2] = (SDL_Rect){64, 48, 32, 48};
  joeySheet.animations[1].frames[3] = (SDL_Rect){96, 48, 32, 48};

  joeySheet.animations[2].frames[0] = (SDL_Rect){0, 96, 32, 48};
  joeySheet.animations[2].frames[1] = (SDL_Rect){32, 96, 32, 48};
  joeySheet.animations[2].frames[2] = (SDL_Rect){64, 96, 32, 48};
  joeySheet.animations[2].frames[3] = (SDL_Rect){96, 96, 32, 48};

  joeySheet.animations[3].frames[0] = (SDL_Rect){0, 144, 32, 48};
  joeySheet.animations[3].frames[1] = (SDL_Rect){32, 144, 32, 48};
  joeySheet.animations[3].frames[2] = (SDL_Rect){64, 144, 32, 48};
  joeySheet.animations[3].frames[3] = (SDL_Rect){96, 144, 32, 48};

  return joeySheet;
}
