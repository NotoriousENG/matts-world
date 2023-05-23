#include "resources.h"
#include "metadata.h"

#include <cute_tiled.h>

Resources load_resources(SDL_Renderer *renderer) {
  Resources resources;
  memset(&resources, 0, sizeof(Resources));
  resources.joey = buildJoeySheet(loadTexture(renderer, "assets/joey.png"));
  resources.map =
      load_tiled_map(renderer, &resources, "assets/tiles/test_map.tmj");
  resources.joystick = SDL_JoystickOpen(0);
  return resources;
}

void free_resources(Resources resources) {
  destroySpriteSheet(resources.joey);
  free_tiled_map(resources.map);

  if (resources.joystick != NULL) {
    SDL_JoystickClose(resources.joystick);
  }
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

void draw_tiled_map(SDL_Renderer *renderer, Tilemap map) {
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
      SDL_RenderCopy(renderer, texture, &src, &dest);
    }

    layer = layer->next;
  }
}