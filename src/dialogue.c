#include "dialogue.h"
#include "defs.h"
#include "resources.h"

DialogueManager dialogueManager_set_dialogue(Dialogue dialogue) {
  DialogueManager dialogueManager = {
      dialogue,
      0,
      0,
  };
  return dialogueManager;
}

void dialogueManager_start(DialogueManager *dialogueManager) {
  dialogueManager->dialogueActive = 1;
  dialogueManager->currentDialogueIndex = 0;
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                 dialogueManager->currentDialogue
                     .dialogue[dialogueManager->currentDialogueIndex]);
}

void dialogueManager_stop(DialogueManager *dialogueManager) {
  dialogueManager->dialogueActive = 0;
  dialogueManager->currentDialogueIndex = 0;
}

void dialogueManager_next(DialogueManager *dialogueManager) {
  dialogueManager->currentDialogueIndex++;
  if (dialogueManager->currentDialogueIndex >=
      dialogueManager->currentDialogue.dialogueLength) {
    dialogueManager->currentDialogueIndex = 0;
    dialogueManager->dialogueActive = 0;
  }
}

void dialogueManager_draw(SDL_Renderer *renderer,
                          DialogueManager *dialogueManager, TTF_Font *font) {
  if (dialogueManager->dialogueActive) {
    SDL_Rect dialogueRect = {0, 0, BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT / 4};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &dialogueRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &dialogueRect);
    SDL_Color color = {255, 255, 255, 255};
    drawText(renderer,
             dialogueManager->currentDialogue
                 .dialogue[dialogueManager->currentDialogueIndex],
             16, 0, font);
  }
}

void dialogueManager_logic(DialogueManager *dialogueManager, float delta,
                           KeyState *keyboard) {
  if (dialogueManager->dialogueActive &&
      keyboard[SDL_SCANCODE_SPACE] == JUST_PRESSED) {
    dialogueManager_next(dialogueManager);

    // also log the new dialogue
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                   dialogueManager->currentDialogue
                       .dialogue[dialogueManager->currentDialogueIndex]);
  }
}