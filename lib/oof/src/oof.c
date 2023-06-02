#include "oof.h"

Collider collider_new(CollisionType type, int w, int h, int offset_x,
                      int offset_y) {
  Collider collider;
  collider.type = type;
  collider.offset_x = offset_x;
  collider.offset_y = offset_y;
  collider.w = w;
  collider.h = h;
  return collider;
}