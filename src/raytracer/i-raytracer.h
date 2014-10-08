#pragma once

#include <vector>
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
      static_cast<float>(x) + .5f,
      static_cast<float>(y) + .5f,
      .0f)).Normalized();
  }
};

// An abstract callback that does the actual tracing/marching of rays.
// Must be thread-safe.
class IRaytracer {
 public:
  // TODO: Separate methods for lookup cube and final view tracing.
  virtual void traceGrid(const RayGrid &grid, std::vector<float> &out_results) = 0;
  virtual ~IRaytracer() {};
};

}

