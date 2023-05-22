#include "entity.h"
#include "spritel.h"

void entity_draw(SDL_Renderer *renderer, Entity entity) {
  if (entity.visible) {
    drawTexture(renderer, entity.sprite.texture,
                &entity.sprite.animations[entity.animator.animation]
                     .frames[entity.animator.currentFrame],
                entity.position.x, entity.position.y, entity.rotation,
                entity.scale);
  }
}