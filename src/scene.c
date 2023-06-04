#include "scene.h"
#include "defs.h"

Scene scene_new(Resources *resources) {
  Scene scene;
  memset(&scene, 0, sizeof(Scene));
  scene.resources = resources;
  scene.joeyDialogue = (Dialogue){
      .dialogueLength = 3,
      .dialogue = malloc(sizeof(char *) * scene.joeyDialogue.dialogueLength),
  };
  scene.joeyDialogue.dialogue[0] = "Hello, mamafackas!";
  scene.joeyDialogue.dialogue[1] =
      "Now it is time to tell you that Revali has died!";
  scene.joeyDialogue.dialogue[2] = "We are back in the past defeating Ganon";

  return scene;
}

void scene_free(Scene *scene) { free(scene->joeyDialogue.dialogue); }

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

  // Create NPC
  scene->npc.position =
      vec2_new(BASE_SCREEN_WIDTH * 0.5f, BASE_SCREEN_HEIGHT * 0.6f);
  scene->npc.sprite = scene->resources->joey;
  scene->npc.animator = animator_new();
  scene->npc.rotation = 0;
  scene->npc.scale = 1;
  scene->npc.visible = 1;
  scene->npc.collider = collider_new(SOLID, 32, 32, 0, 16);

  scene->dialogueManager = dialogueManager_set_dialogue(scene->joeyDialogue);
}

void scene_draw(SDL_Renderer *renderer, Scene *scene) {
  draw_tiled_map(renderer, scene->resources->map,
                 scene->resources->debug.collisions);

  entity_draw(renderer, scene->npc, scene->resources->debug.collisions);
  entity_draw(renderer, scene->player, scene->resources->debug.collisions);

  if (scene->resources->debug.collisions) {
    // draw the player interaction area
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderDrawRect(renderer, &scene->playerInteractionRect);
  }
  dialogueManager_draw(renderer, &scene->dialogueManager,
                       scene->resources->font);
}

void scene_logic(Scene *scene, float delta) {

  int dialogueActive =
      scene->dialogueManager
          .dialogueActive; // dialogueActive for this duration of this frame

  dialogueManager_logic(&scene->dialogueManager, delta,
                        scene->resources->keyboard);

  if (!dialogueActive) {
    playerLogic(scene, delta);
  }

  // if 1 is pressed, toggle debug mode
  if (scene->resources->keyboard[SDL_SCANCODE_1] == JUST_PRESSED) {
    scene->resources->debug.collisions =
        !scene->resources->debug.collisions; // toggle debug mode
  }
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

  int isMoving = vec2_length(inputDir) > 0;

  if (isMoving) {
    inputDir = vec2_normalize(inputDir);
  }

  animator_process(&scene->player.animator, scene->player.sprite, delta,
                   !isMoving);

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

  handle_tilemap_collisions(&scene->player, scene->resources->map);
  handle_entity_collisions(&scene->player, &scene->npc);

  // create a player trigger w/h pixels in front of the player (based on the dir
  // (most influential direction))
  vec2 trigger_position =
      vec2_new(player->position.x + player->collider.offset_x,
               player->position.y + player->collider.offset_y);
  switch (dir) {
  case 0:
    trigger_position.y += player->collider.h;
    break;
  case 1:
    trigger_position.x -= player->collider.w;
    break;
  case 2:
    trigger_position.x += player->collider.w;
    break;
  case 3:
    trigger_position.y -= player->collider.h;
    break;
  default: // do nothing
    break;
  }

  scene->playerInteractionRect =
      (SDL_Rect){trigger_position.x, trigger_position.y, 32, 32};

  // check if the player is in range of the npc
  // get the collision rect for the npc
  SDL_Rect npc_rect = (SDL_Rect){
      scene->npc.position.x + scene->npc.collider.offset_x,
      scene->npc.position.y + scene->npc.collider.offset_y,
      scene->npc.collider.w,
      scene->npc.collider.h,
  };
  if (SDL_HasIntersection(&scene->playerInteractionRect, &npc_rect)) {
    // if the player presses the interact key, start the dialogue
    if (scene->resources->keyboard[SDL_SCANCODE_SPACE] == JUST_PRESSED &&
        !scene->dialogueManager.dialogueActive) {
      dialogueManager_start(&scene->dialogueManager);
      // set the npc to face the player this is the oppossite of the player dir
      switch (dir) {
      case 0:
        scene->npc.animator.animation = 3;
        break;
      case 1:
        scene->npc.animator.animation = 2;
        break;
      case 2:
        scene->npc.animator.animation = 1;
        break;
      case 3:
        scene->npc.animator.animation = 0;
        break;
      default:
        break;
      }
    }
  }
}