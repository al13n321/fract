#pragma once

#include <vector>
#include "ray-grid.h"

namespace fract {

// An abstract callback that does the actual tracing/marching of rays.
// TODO: thread safety contract for background rendering.
class IRaytracer {
 public:
  // TODO: Separate methods for lookup cube and final view tracing.
  virtual void TraceGrid(const RayGrid &grid, RaytracedView &target)
    = 0;
  virtual ~IRaytracer() {};
};

}

