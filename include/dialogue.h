#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

typedef uint8_t KeyState;

typedef struct Dialogue {
  int dialogueLength;
  char **dialogue;
} Dialogue;

typedef struct DialogueManager {
  Dialogue currentDialogue;
  int currentDialogueIndex;
  int dialogueActive;
} DialogueManager;

DialogueManager dialogueManager_set_dialogue(Dialogue dialogue);

void dialogueManager_start(DialogueManager *dialogueManager);

void dialogueManager_stop(DialogueManager *dialogueManager);

void dialogueManager_next(DialogueManager *dialogueManager);

void dialogueManager_draw(SDL_Renderer *renderer,
                          DialogueManager *dialogueManager, TTF_Font *font);

void dialogueManager_logic(DialogueManager *dialogueManager, float delta,
                           KeyState *keyboard);

void dialogue_free(Dialogue *dialogue);