#include "color.h"


// Function to perform a smooth color transition between two RGB colors
RGB InterpolateColors(const RGB& a, const RGB& b, float t) {
  return {
      .r = (1.0f - t) * a.r + t * b.r,
      .g = (1.0f - t) * a.g + t * b.g,
      .b = (1.0f - t) * a.b + t * b.b
  };
}

