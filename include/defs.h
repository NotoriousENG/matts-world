#pragma once
#define MAX_LINE_LENGTH 1024

#define MAX_SOUND_CHANNELS 8
#define BASE_SCREEN_WIDTH 320
#define BASE_SCREEN_HEIGHT 320

#define PLAYER_SPEED 150

#define RENDER_SCALE 2

#define SCALED_SCREEN_WIDTH (BASE_SCREEN_WIDTH * RENDER_SCALE)
#define SCALED_SCREEN_HEIGHT (BASE_SCREEN_HEIGHT * RENDER_SCALE)

#define WRAP_AROUND_DISTANCE 50

#define FONT_SIZE 16

#ifndef GAME_NAME
#define GAME_NAME "Add Game Name to CMakeLists.txt"
#endif