#include "sinput.h"

void doKeyUp(int *keyboard, SDL_KeyboardEvent *event) {
  if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS) {
    keyboard[event->keysym.scancode] = 0;
  }
}

void doKeyDown(int *keyboard, SDL_KeyboardEvent *event) {
  if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS) {
    keyboard[event->keysym.scancode] = 1;
  }
}

void doInput(int *keyboard) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      exit(0);
      break;
    case SDL_KEYDOWN:
      doKeyDown(keyboard, &event.key);
      break;
    case SDL_KEYUP:
      doKeyUp(keyboard, &event.key);
      break;
    default:
      break;
    }
  }
}