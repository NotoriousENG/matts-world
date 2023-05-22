#include "resources.h"
#include "metadata.h"

Resources load_resources(SDL_Renderer *renderer) {
  Resources resources;
  memset(&resources, 0, sizeof(Resources));
  resources.joey = buildJoeySheet(loadTexture(renderer, "assets/joey.png"));
  resources.joystick = SDL_JoystickOpen(0);
  return resources;
}

void free_resources(Resources resources) {
  destroySpriteSheet(resources.joey);

  if (resources.joystick != NULL) {
    SDL_JoystickClose(resources.joystick);
  }
}
