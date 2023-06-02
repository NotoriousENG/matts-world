#pragma once

typedef enum CollisionType CollisionType;
enum CollisionType { TRIGGER, SOLID };

typedef struct Collider {
  CollisionType type;
  int w;
  int h;
  int offset_x;
  int offset_y;
} Collider;

Collider collider_new(CollisionType type, int w, int h, int offset_x,
                      int offset_y);