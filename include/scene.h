#pragma once

#include "defs.h"
#include "dialogue.h"
#include "entity.h"
#include "resources.h"
#include <flecs.h>

typedef struct Scene {
  Entity player;
  Entity npc;
  float playerSpeed;
  SDL_Rect playerInteractionRect;
  Resources *resources;
  DialogueManager dialogueManager;
  Dialogue joeyDialogue;
  ecs_world_t *world;
} Scene;

typedef struct RendererRef {
  SDL_Renderer *renderer;
} RendererRef;

typedef struct CameraRef {
  Camera *camera;
} CameraRef;

Scene scene_new(Resources *resources, SDL_Renderer *renderer);

void scene_begin(Scene *scene);

void scene_draw(SDL_Renderer *renderer, Scene *scene);

void scene_logic(Scene *scene, float delta);

void playerLogic(Scene *scene, float delta);

void scene_free(Scene *scene);

void draw_ecs(ecs_iter_t *it);

void move_ecs(ecs_iter_t *it);