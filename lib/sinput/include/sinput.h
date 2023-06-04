#pragma once
#include <SDL.h>

#define MAX_KEYBOARD_KEYS 350 // world for now

typedef uint8_t KeyState;
enum {
  NOT_PRESSED = 0,
  JUST_PRESSED = 1,
  PRESSED = 2,
};

void doInput(KeyState *keyboard);