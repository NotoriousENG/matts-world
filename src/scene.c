#include "scene.h"
#include "components.h"
#include "defs.h"
#include <string.h>

#ifdef _WIN32
#define strdup _strdup
#endif

Scene scene_new(Resources *resources, SDL_Renderer *renderer) {
  Scene scene;
  memset(&scene, 0, sizeof(Scene));

  scene.world = ecs_init();
  ECS_COMPONENT(scene.world, Transform2D);
  ECS_COMPONENT(scene.world, Velocity2D);
  ECS_COMPONENT(scene.world, SpriteSheet);
  ECS_COMPONENT(scene.world, Animator);
  ECS_COMPONENT(scene.world, Player);

  ECS_COMPONENT(scene.world, RendererRef);
  ECS_COMPONENT(scene.world, CameraRef);
  ECS_COMPONENT(scene.world, KeyboardRef);

  ecs_singleton_set(scene.world, RendererRef, {renderer});
  ecs_singleton_set(scene.world, CameraRef, {&resources->mainCamera});
  ecs_singleton_set(scene.world, KeyboardRef, {resources->keyboard});

  ECS_SYSTEM(scene.world, draw_ecs,
             EcsOnUpdate, [in] Transform2D, [in] SpriteSheet, [in] Animator);
  ECS_SYSTEM(scene.world, move_ecs,
             EcsOnUpdate, [in] Transform2D, [in] Velocity2D);
  ECS_SYSTEM(scene.world, animate_ecs, EcsOnUpdate, Animator, [in] SpriteSheet);
  ECS_SYSTEM(scene.world, player_ecs, EcsOnUpdate, [in] Player, Velocity2D,
             Animator);

  ecs_entity_t e = ecs_entity(scene.world, {.name = "Test"});

  ecs_set(scene.world, e, Transform2D,
          {
              // set position to the center of the screen
              .position =
                  vec2_new(BASE_SCREEN_WIDTH * 0.5f, BASE_SCREEN_HEIGHT * 0.5f),
              .rotation = 0,
              .scale = 1,
          });
  ecs_set(scene.world, e, Player, {0});
  ecs_set(scene.world, e, Velocity2D, {1, 0});
  ecs_set_id(scene.world, e, ecs_id(SpriteSheet), sizeof(SpriteSheet),
             &resources->joey);
  Animator animator = animator_new();
  ecs_set_id(scene.world, e, ecs_id(Animator), sizeof(Animator), &animator);

  scene.resources = resources;
  scene.joeyDialogue.dialogueLength = 3;
  scene.joeyDialogue.dialogue =
      malloc(sizeof(char *) * scene.joeyDialogue.dialogueLength);

  scene.joeyDialogue.dialogue[0] = strdup("Hello, mamafackas!");
  scene.joeyDialogue.dialogue[1] =
      strdup("Now it is time to tell you that Revali has died!");
  scene.joeyDialogue.dialogue[2] =
      strdup("We are back in the past defeating Ganon");
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                 "Allocated %i lines of dialogue",
                 scene.joeyDialogue.dialogueLength);

  return scene;
}

void scene_free(Scene *scene) {
  dialogue_free(&scene->joeyDialogue);
  ecs_fini(scene->world); // destroy the ecs world
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

  set_position_from_tilemap(&scene->player.position, "Player",
                            scene->resources->map, vec2_new(0, 16));

  // Create NPC
  scene->npc.position =
      vec2_new(BASE_SCREEN_WIDTH * 0.5f, BASE_SCREEN_HEIGHT * 0.6f);
  scene->npc.sprite = scene->resources->joey;
  scene->npc.animator = animator_new();
  scene->npc.rotation = 0;
  scene->npc.scale = 1;
  scene->npc.visible = 1;
  scene->npc.collider = collider_new(SOLID, 32, 32, 0, 16);

  set_position_from_tilemap(&scene->npc.position, "NPC", scene->resources->map,
                            vec2_new(0, 16));

  scene->dialogueManager = dialogueManager_set_dialogue(scene->joeyDialogue);
}

void scene_draw(SDL_Renderer *renderer, Scene *scene) {
  draw_tiled_map(renderer, scene->resources->map, scene->resources->mainCamera,
                 scene->resources->debug.collisions);

  entity_draw(renderer, scene->npc, scene->resources->mainCamera,
              scene->resources->debug.collisions);
  entity_draw(renderer, scene->player, scene->resources->mainCamera,
              scene->resources->debug.collisions);

  if (scene->resources->debug.collisions) {
    // draw the player interaction area
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect playerInteractionRect = scene->playerInteractionRect;
    playerInteractionRect.x -= scene->resources->mainCamera.position.x;
    playerInteractionRect.y -= scene->resources->mainCamera.position.y;
    SDL_RenderDrawRect(renderer, &playerInteractionRect);
  }
  dialogueManager_draw(renderer, &scene->dialogueManager,
                       scene->resources->font);

  ecs_progress(scene->world, 1 /* delta_time */);
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

  handle_tilemap_collisions(&scene->player, scene->resources->map);
  handle_entity_collisions(&scene->player, &scene->npc);

  // camera should follow the player, but keep a distance of half the screen
  // width and height
  scene->resources->mainCamera.position = vec2_subtract(
      vec2_new(scene->player.position.x, scene->player.position.y),
      vec2_new(BASE_SCREEN_WIDTH / 2, BASE_SCREEN_HEIGHT / 2));

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

void move_ecs(ecs_iter_t *it) {
  Transform2D *t = ecs_field(it, Transform2D, 1);
  Velocity2D *v = ecs_field(it, Velocity2D, 2);

  for (int i = 0; i < it->count; i++) {
    t[i].position = vec2_add(
        t[i].position,
        vec2_scale(v[i].velocity, it->delta_time / 100)); // @TODO remove +1
  }
}

void draw_ecs(ecs_iter_t *it) {
  // Get fields from system query
  Transform2D *t = ecs_field(it, Transform2D, 1);
  SpriteSheet *s = ecs_field(it, SpriteSheet, 2);
  Animator *a = ecs_field(it, Animator, 3);

  ECS_COMPONENT(it->world, RendererRef);
  ECS_COMPONENT(it->world, CameraRef);

  SDL_Renderer *renderer = ecs_singleton_get(it->world, RendererRef)->renderer;
  Camera *camera = ecs_singleton_get(it->world, CameraRef)->camera;

  // Iterate matched entities
  for (int i = 0; i < it->count; i++) {
    drawTexture(renderer, s[i].texture,
                &s[i].animations[a->animation].frames[a->currentFrame],
                t[i].position.x - camera->position.x,
                t[i].position.y - camera->position.y, t[i].rotation,
                t[i].scale);
  }
}

void animate_ecs(ecs_iter_t *it) {
  Animator *a = ecs_field(it, Animator, 1);
  SpriteSheet *s = ecs_field(it, SpriteSheet, 2);

  for (int i = 0; i < it->count; i++) {
    animator_process(&a[i], s[i], it->delta_time / 100, false);
  }
}

void player_ecs(ecs_iter_t *it) {

  // Get fields from system query
  Player *p = ecs_field(it, Player, 1);
  Velocity2D *v = ecs_field(it, Velocity2D, 2);
  Animator *a = ecs_field(it, Animator, 3);

  ECS_COMPONENT(it->world, KeyboardRef);
  KeyState *k = ecs_singleton_get(it->world, KeyboardRef)->keyboard;

  vec2 inputDir = (vec2){0, 0};
  inputDir.x = (k[SDL_SCANCODE_RIGHT] || k[SDL_SCANCODE_D]) -
               (k[SDL_SCANCODE_LEFT] || k[SDL_SCANCODE_A]);
  inputDir.y = (k[SDL_SCANCODE_DOWN] || k[SDL_SCANCODE_S]) -
               (k[SDL_SCANCODE_UP] || k[SDL_SCANCODE_W]);

  // clamp the input x and y to -1 and 1
  inputDir.x = clampf(inputDir.x, -1, 1);
  inputDir.y = clampf(inputDir.y, -1, 1);

  for (int i = 0; i < it->count; i++) {

    int dir = a[i].animation;
    if (inputDir.y > 0) {
      dir = 0;
    } else if (inputDir.y < 0) {
      dir = 3;
    } else if (inputDir.x < 0) {
      dir = 1;
    } else if (inputDir.x > 0) {
      dir = 2;
    }
    // set the animation
    a[i].animation = dir;

    int isMoving = vec2_length(inputDir) > 0;

    if (isMoving) {
      inputDir = vec2_normalize(inputDir);
    }
    // set the velocity to the input dir
    v[i].velocity = vec2_scale(inputDir, PLAYER_SPEED);
  }
}