#pragma once

#include <gColor.h>

struct RGB {
  float r, g, b;

  operator gColor() const {
    return {r, g, b};
  }
};

inline RGB CreateRGB(int r, int g, int b) {
  return {r / 255.0f, g / 255.0f, b / 255.0f};
}

inline RGB CreateRGB(int val) {
  int r = (val >> 16) & 0xFF;
  int g = (val >> 8) & 0xFF;
  int b = val & 0xFF;
  return CreateRGB(r, g, b);
}

// Function to perform a smooth color transition between two RGB colors
RGB InterpolateColors(const RGB& a, const RGB& b, float t);