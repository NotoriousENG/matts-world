#include "resources.h"
#include "metadata.h"

#include <cute_tiled.h>
#include <entity.h>

Resources load_resources(SDL_Renderer *renderer) {
  Resources resources;
  memset(&resources, 0, sizeof(Resources));
  resources.font = loadFont("assets/Pokemon.ttf", FONT_SIZE);
  resources.joey = buildJoeySheet(loadTexture(renderer, "assets/joey.png"));
  resources.map =
      load_tiled_map(renderer, &resources, "assets/tiles/test_map.tmj");
  resources.joystick = SDL_JoystickOpen(0);
  return resources;
}

TTF_Font *loadFont(char *filename, int size) {
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                 "Loading %s", filename);
  TTF_Font *font = TTF_OpenFont(filename, size);
  if (font == NULL) {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                   "Failed to load font: %s", TTF_GetError());
    exit(1);
  }
  return font;
}

void free_resources(Resources resources) {
  TTF_CloseFont(resources.font);
  destroySpriteSheet(resources.joey);
  free_tiled_map(resources.map);

  if (resources.joystick != NULL) {
    SDL_JoystickClose(resources.joystick);
  }
}

void drawText(SDL_Renderer *renderer, char *text, int x, int y,
              TTF_Font *font) {
  SDL_Color color = {0, 0, 0};
  int wrapLength = BASE_SCREEN_WIDTH;
  SDL_Surface *surface =
      TTF_RenderText_Solid_Wrapped(font, text, color, wrapLength);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
  SDL_RenderCopy(renderer, texture, NULL, &dest);
  SDL_DestroyTexture(texture);
}

Tilemap load_tiled_map(SDL_Renderer *renderer, Resources *resources,
                       const char *path) {
  Tilemap map;
  memset(&map, 0, sizeof(Tilemap));
  map.map = cute_tiled_load_map_from_file(path, NULL);
  // get the texture path *It must be embedded in the tmj to work*
  const char *texture_path = map.map->tilesets->image.ptr;

  // get the directory in the path probably @TODO make this a function later
  char tilesetDir[256];
  strcpy(tilesetDir, path);
  char *lastSlash = strrchr(tilesetDir, '/');
  if (lastSlash != NULL) {
    lastSlash[1] = '\0';
  } else {
    tilesetDir[0] = '\0';
  }

  // get the full relative path to the texture
  char full_path[256];
  strcpy(full_path, tilesetDir);
  // append the texture path to the directory
  strcat(full_path, texture_path);

  // load the texture
  map.texture = loadTexture(renderer, full_path);

  return map;
}

void free_tiled_map(Tilemap map) {
  cute_tiled_free_map(map.map);
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                 "Freed tiled map");
  SDL_DestroyTexture(map.texture);
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                 "Freed map texture");
}

void draw_tiled_map(SDL_Renderer *renderer, Tilemap map, Camera camera,
                    int debug_collisions) {
  // loop over the map's layers
  cute_tiled_layer_t *layer = map.map->layers;
  while (layer) {
    int *data = layer->data;
    int data_count = layer->data_count;

    // draw the tiles
    for (int i = 0; i < data_count; i++) {
      int tile = data[i];
      if (tile == 0) {
        continue;
      }

      // get the tileset
      cute_tiled_tileset_t *tileset = map.map->tilesets;
      if (tileset == NULL) {
        continue;
      }

      // get the tileset image
      SDL_Texture *texture = map.texture;
      if (texture == NULL) {
        continue;
      }

      // get the fields needed to draw the tile
      int tile_width = tileset->tilewidth;
      int tile_height = tileset->tileheight;
      int tileset_columns = tileset->columns;
      int tileset_margin = tileset->margin;
      int tileset_spacing = tileset->spacing;
      int tileset_tile_offset = tileset->firstgid;
      int tileset_tile_id = tile - tileset_tile_offset;
      int tileset_tile_x = tileset_tile_id % tileset_columns;
      int tileset_tile_y = tileset_tile_id / tileset_columns;
      int tileset_tile_x_px =
          tileset_tile_x * (tile_width + tileset_spacing) + tileset_margin;
      int tileset_tile_y_px =
          tileset_tile_y * (tile_height + tileset_spacing) + tileset_margin;

      // draw
      SDL_Rect src = {tileset_tile_x_px, tileset_tile_y_px, tile_width,
                      tile_height};
      SDL_Rect dest = {(i % layer->width) * tile_width,
                       (i / layer->width) * tile_height, tile_width,
                       tile_height};

      // adjust for camera
      dest.x -= camera.position.x;
      dest.y -= camera.position.y;

      SDL_RenderCopy(renderer, texture, &src, &dest);

      if (debug_collisions) {
        // draw all collision tiles as green
        cute_tiled_tile_descriptor_t *tiles = tileset->tiles;
        while (tiles) {
          if (tiles->tile_index == tileset_tile_id) {
            if (strcmp(tiles->type.ptr, "Collision") == 0) {
              SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128);
              SDL_RenderFillRect(renderer, &dest);
            }
          }
          tiles = tiles->next;
        }
      }
    }

    layer = layer->next;
  }
}

void handle_tilemap_collisions(Entity *entity, Tilemap map) {
  // check if the collider overlaps any of the tiles corresponding dest rects
  // for tiles who have a type of collision log the tile id and the type of
  // collision

  if (entity->collider.type != SOLID) {
    return;
  }

  // loop over the map's layers
  cute_tiled_layer_t *layer = map.map->layers;
  while (layer) {
    int *data = layer->data;
    int data_count = layer->data_count;

    for (int i = 0; i < data_count; i++) {
      int tile = data[i];
      if (tile == 0) {
        continue;
      }

      // get the tileset
      cute_tiled_tileset_t *tileset = map.map->tilesets;
      if (tileset == NULL) {
        continue;
      }

      int tile_width = tileset->tilewidth;
      int tile_height = tileset->tileheight;
      int tileset_tile_offset = tileset->firstgid;
      int tileset_tile_id = tile - tileset_tile_offset;

      SDL_Rect dest = {(i % layer->width) * tile_width,
                       (i / layer->width) * tile_height, tile_width,
                       tile_height};

      SDL_Rect entity_collider = entity_get_collision_rect(*entity);
      vec2 *entity_position = &entity->position;

      // check if the collider overlaps the tile
      if (SDL_HasIntersection(&entity_collider, &dest)) {
        // log the tile id and the type of collision
        cute_tiled_tile_descriptor_t *tiles = tileset->tiles;
        while (tiles) {
          if (tiles->tile_index == tileset_tile_id) {
            if (strcmp(tiles->type.ptr, "Collision") == 0) {
              // Calculate the horizontal and vertical distances between the
              // centers of the collider and dest rectangles
              int centerColliderX = entity_collider.x + entity_collider.w / 2;
              int centerColliderY = entity_collider.y + entity_collider.h / 2;
              int centerDestX = dest.x + dest.w / 2;
              int centerDestY = dest.y + dest.h / 2;
              int distanceX = centerColliderX - centerDestX;
              int distanceY = centerColliderY - centerDestY;

              int minDistanceX = entity_collider.w / 2 + dest.w / 2;
              int minDistanceY = entity_collider.h / 2 + dest.h / 2;

              // Check if the absolute values of the distances are less than the
              // minimum distances
              if (abs(distanceX) < minDistanceX &&
                  abs(distanceY) < minDistanceY) {
                // Collision detected
                int overlapX = minDistanceX - abs(distanceX);
                int overlapY = minDistanceY - abs(distanceY);

                // Resolve the collision based on the direction of overlap
                if (overlapX < overlapY) {
                  // Horizontal collision
                  if (distanceX > 0) {
                    // Collision from the right
                    entity_position->x += overlapX;
                  } else {
                    // Collision from the left
                    entity_position->x -= overlapX;
                  }
                } else {
                  // Vertical collision
                  if (distanceY > 0) {
                    // Collision from below
                    entity_position->y += overlapY;
                  } else {
                    // Collision from above
                    entity_position->y -= overlapY;
                  }
                }
              }
            }
          }
          tiles = tiles->next;
        }
      }
    }

    layer = layer->next;
  }
}