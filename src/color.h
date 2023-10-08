#pragma once

typedef struct rgb {
  float r, g, b;
} RGB;

typedef struct hsl {
  float h, s, l;
} HSL;

/*
 * Converts an RGB color value to HSL. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes r, g, and b are contained in the set [0, 255] and
 * returns HSL in the set [0, 1].
 */
HSL rgb2hsl(float r, float g, float b);

////////////////////////////////////////////////////////////////////////

/*
 * Converts an HUE to r, g or b.
 * returns float in the set [0, 1].
 */
float hue2rgb(float p, float q, float t);

////////////////////////////////////////////////////////////////////////

/*
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns RGB in the set [0, 255].
 */
RGB hsl2rgb(float h, float s, float l);

// Function to perform a smooth color transition between two RGB colors
RGB interpolateColors(const RGB& a, const RGB& b, float t);