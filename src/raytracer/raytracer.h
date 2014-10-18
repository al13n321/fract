#pragma once

#include <vector>
#include "ray-grid.h"
#include "raytraced-view.h"

namespace fract {

// An abstract callback that does the actual tracing/marching of rays.
// Reloads the needed shaders and updates uniforms.
// TODO: thread safety contract for background rendering.
class Raytracer {
 public:
  // TODO: Separate methods for lookup cube and final view tracing.
  void TraceGrid(const RayGrid &grid, RaytracedView &target);
};

}

