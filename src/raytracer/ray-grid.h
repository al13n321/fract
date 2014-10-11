#pragma once

#include "util/vec.h"
#include "util/mat.h"

namespace fract {

// Specifies a bunch of rays in space, produced by a camera.
// Also scale (it can be considered a part of ray specification, like "speed").
// The near and far clip planes of the projection matrix should be ignored
//  (all rays should start at camera position and terminate at infinity).
struct RayGrid {
  // Camera.
  dvec3 position;
  fmat4 rotation_projection_inv;

  // World scale.
  double scale;

  // Resolution of the grid.
  int resolution_width;
  int resolution_height;

  // Rectangle of the grid.
  int min_x;
  int min_y;
  int size_x;
  int size_y;

  inline fvec3 GetRayDirection(int x, int y) const {
    return rotation_projection_inv.Transform(fvec3(
      (static_cast<float>(x) + .5f)
        / static_cast<float>(resolution_width) * 2 - 1,
      (static_cast<float>(y) + .5f)
        / static_cast<float>(resolution_height) * 2 - 1,
      .0f)).Normalized();
  }

  // Approximate world-space diameter of screen-space pixel at distance 1.
  inline float GetPixelSizeCoefficient(int x, int y) const {
    fvec3 a = rotation_projection_inv.Transform(fvec3(
      static_cast<float>(x)
        / static_cast<float>(resolution_width) * 2 - 1,
      static_cast<float>(y)
        / static_cast<float>(resolution_height) * 2 - 1,
      .0f));
    fvec3 b = rotation_projection_inv.Transform(fvec3(
      (static_cast<float>(x) + .1f)
        / static_cast<float>(resolution_width) * 2 - 1,
      (static_cast<float>(y) + .1f)
        / static_cast<float>(resolution_height) * 2 - 1,
      .0f));
    return sqrtf(a.DistanceSquare(b) / a.LengthSquare());
  }
};

}
