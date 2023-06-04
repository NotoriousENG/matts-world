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

#ifdef DEBUG_COLLISIONS
  {
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
#endif
}

SDL_Rect entity_get_collision_rect(Entity entity) {
  SDL_Rect collisionRect = {entity.position.x + entity.collider.offset_x,
                            entity.position.y + entity.collider.offset_y,
                            entity.collider.w, entity.collider.h};
  return collisionRect;
}

void handle_entity_collisions(Entity *entity, Entity *other) {
  SDL_Rect collisionRect = entity_get_collision_rect(*entity);
  SDL_Rect otherCollisionRect = entity_get_collision_rect(*other);

  if (SDL_HasIntersection(&collisionRect, &otherCollisionRect)) {
    if (entity->collider.type == SOLID) {
      // Calculate the horizontal and vertical distances between the
      // centers of the collider rectangles
      int h_dist = (collisionRect.x + collisionRect.w / 2) -
                   (otherCollisionRect.x + otherCollisionRect.w / 2);
      int v_dist = (collisionRect.y + collisionRect.h / 2) -
                   (otherCollisionRect.y + otherCollisionRect.h / 2);

      // Check if the absolute values of the distances are less than the
      // minimum distances
      int min_dist_x = (collisionRect.w + otherCollisionRect.w) / 2;
      int min_dist_y = (collisionRect.h + otherCollisionRect.h) / 2;

      if (abs(h_dist) < min_dist_x && abs(v_dist) < min_dist_y) {
        // Collision detected
        int overlap_x = min_dist_x - abs(h_dist);
        int overlap_y = min_dist_y - abs(v_dist);

// Log the collision
#ifdef DEBUG_COLLISIONS
        {
          SDL_Log("Collision! overlap_x: %d, overlap_y: %d", overlap_x,
                  overlap_y);
        }
#endif

        // Resolve the collision based on the direction of overlap
        if (overlap_x < overlap_y) {
          // Horizontal collision
          if (h_dist > 0) {
            // Collision from the right
            entity->position.x += overlap_x;
          } else {
            // Collision from the left
            entity->position.x -= overlap_x;
          }
        } else {
          // Vertical collision
          if (v_dist > 0) {
            // Collision from below
            entity->position.y += overlap_y;
          } else {
            // Collision from above
            entity->position.y -= overlap_y;
          }
        }
      }
    }
  }
}