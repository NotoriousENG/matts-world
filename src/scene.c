#include "scene.h"
#include "defs.h"

Scene scene_new(Resources *resources) {
  Scene scene;
  memset(&scene, 0, sizeof(Scene));
  scene.resources = resources;
  return scene;
}

void scene_begin(Scene *scene) {
  // Create Player
  scene->player.position =
      vec2_new(BASE_SCREEN_WIDTH * 0.5f, BASE_SCREEN_HEIGHT * 0.8f);
  scene->player.sprite = scene->resources->joey;
  scene->player.animator = animator_new();
  scene->player.rotation = 0;
  scene->player.scale = 1;
  scene->player.visible = 1;
  scene->playerSpeed = PLAYER_SPEED;
  scene->player.collider = collider_new(SOLID, 32, 32, 0, 16);
}

void scene_draw(SDL_Renderer *renderer, Scene *scene) {
  draw_tiled_map(renderer, scene->resources->map);
  entity_draw(renderer, scene->player);
}

void scene_logic(Scene *scene, float delta) {
  playerLogic(scene, delta);
  animator_process(&scene->player.animator, scene->player.sprite, delta);
  handle_tilemap_collisions(&scene->player, scene->resources->map);
}

void playerLogic(Scene *scene, float delta) {
  vec2 inputDir = (vec2){0, 0};
  inputDir.x = (scene->resources->keyboard[SDL_SCANCODE_RIGHT] ||
                scene->resources->keyboard[SDL_SCANCODE_D]) -
               (scene->resources->keyboard[SDL_SCANCODE_LEFT] ||
                scene->resources->keyboard[SDL_SCANCODE_A]);
  inputDir.y = (scene->resources->keyboard[SDL_SCANCODE_DOWN] ||
                scene->resources->keyboard[SDL_SCANCODE_S]) -
               (scene->resources->keyboard[SDL_SCANCODE_UP] ||
                scene->resources->keyboard[SDL_SCANCODE_W]);

  // if we get joystick input, use that instead
  if (scene->resources->joystick != NULL) {
    SDL_Joystick *joystick = scene->resources->joystick;

    inputDir.x += (float)SDL_JoystickGetAxis(joystick, 0) / 32767.0f;
    inputDir.y += (float)SDL_JoystickGetAxis(joystick, 1) / 32767.0f;

    Uint8 hatState = SDL_JoystickGetHat(joystick, 0);
    if (hatState & SDL_HAT_UP) {
      inputDir.y -= 1;
    }
    if (hatState & SDL_HAT_DOWN) {
      inputDir.y += 1;
    }
    if (hatState & SDL_HAT_LEFT) {
      inputDir.x -= 1;
    }
    if (hatState & SDL_HAT_RIGHT) {
      inputDir.x += 1;
    }
  }

  // clamp the input x and y to -1 and 1
  inputDir.x = clampf(inputDir.x, -1, 1);
  inputDir.y = clampf(inputDir.y, -1, 1);

  // get the most influential direction as an int [0 - DOWN] , [1 - LEFT] , [2 -
  // RIGTH] , [3 - UP]
  int dir = scene->player.animator.animation;
  if (inputDir.y > 0) {
    dir = 0;
  } else if (inputDir.y < 0) {
    dir = 3;
  } else if (inputDir.x < 0) {
    dir = 1;
  } else if (inputDir.x > 0) {
    dir = 2;
  }
  scene->player.animator.animation = dir;

  if (vec2_length(inputDir) > 0) {
    inputDir = vec2_normalize(inputDir);
  }

  Entity *player = &scene->player;

  player->position = vec2_add(player->position,
                              vec2_scale(inputDir, scene->playerSpeed * delta));

  // if the player is off the screen by 100 pixels, wrap them around to the
  // other side
  if (player->position.x < -WRAP_AROUND_DISTANCE) {
    player->position.x = BASE_SCREEN_WIDTH + WRAP_AROUND_DISTANCE;
  }
  if (player->position.x > BASE_SCREEN_WIDTH + WRAP_AROUND_DISTANCE) {
    player->position.x = -WRAP_AROUND_DISTANCE;
  }
  if (player->position.y < -WRAP_AROUND_DISTANCE) {
    player->position.y = BASE_SCREEN_HEIGHT + WRAP_AROUND_DISTANCE;
  }
  if (player->position.y > BASE_SCREEN_HEIGHT + WRAP_AROUND_DISTANCE) {
    player->position.y = -WRAP_AROUND_DISTANCE;
  }
}