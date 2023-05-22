#include <chroma.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

color_rgb hue_shift(color_rgb color, float delta) {
  color_hsv hsv = rgb_to_hsv(color);
  // add delta to hue, then wrap it around
  hsv.h = fmodf(hsv.h + delta * 360.0f, 360.0f);
  color_rgb result = hsv_to_rgb(hsv);
  return result;
}

color_rgb hsv_to_rgb(color_hsv hsv) {
  float h = hsv.h / 360.0f;
  float s = hsv.s / 100.0f;
  float v = hsv.v / 100.0f;
  float r, g, b;
  int i = (int)(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);
  switch (i % 6) {
  case 0:
    r = v, g = t, b = p;
    break;
  case 1:
    r = q, g = v, b = p;
    break;
  case 2:
    r = p, g = v, b = t;
    break;
  case 3:
    r = p, g = q, b = v;
    break;
  case 4:
    r = t, g = p, b = v;
    break;
  case 5:
    r = v, g = p, b = q;
    break;
  }
  float red = (r * 255);
  float green = (g * 255);
  float blue = (b * 255);
  return (color_rgb){red, green, blue};
}

color_hsv rgb_to_hsv(color_rgb rgb) {
  float r = rgb.r / 255.0f;
  float g = rgb.g / 255.0f;
  float b = rgb.b / 255.0f;
  float max = fmaxf(fmaxf(r, g), b);
  float min = fminf(fminf(r, g), b);
  float h = 0;
  float s = 0;
  float v = max;
  float d = max - min;
  s = max == 0 ? 0 : d / max;
  if (max == min) {
    h = 0;
  } else {
    if (max == r) {
      h = (g - b) / d + (g < b ? 6 : 0);
    } else if (max == g) {
      h = (b - r) / d + 2;
    } else if (max == b) {
      h = (r - g) / d + 4;
    }
    h /= 6;
  }
  float hue = (h * 360);
  float saturation = (s * 100);
  float value = (v * 100);
  return (color_hsv){hue, saturation, value};
}
