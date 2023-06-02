#pragma once
#include <SDL.h>

#define MAX_KEYBOARD_KEYS 350 // world for now

void doKeyUp(int *keyboard, SDL_KeyboardEvent *event);

void doKeyDown(int *keyboard, SDL_KeyboardEvent *event);

void doInput(int *keyboard);