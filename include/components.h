#include <clm.h>
#include <flecs.h>

typedef struct Transform2D {
  vec2 position;
  float rotation;
  float scale;
} Transform2D;

typedef struct Velocity2D {
  vec2 velocity;
} Velocity2D;