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

  if (DEBUG_COLLISIONS) {
    // draw the collider as a red box over the texture
    SDL_Rect collisionRect = entity_get_collision_rect(entity);
    // set the color to red for SOLID, else BLUE
    if (entity.collider.type == SOLID) {
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    } else {
      SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    }
    SDL_RenderDrawRect(renderer, &collisionRect);
  }
}

SDL_Rect entity_get_collision_rect(Entity entity) {
  SDL_Rect collisionRect = {entity.position.x + entity.collider.offset_x,
                            entity.position.y + entity.collider.offset_y,
                            entity.collider.w, entity.collider.h};
  return collisionRect;
}