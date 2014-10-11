#pragma once

#include <vector>
#include "ray-grid.h"

namespace fract {

// An abstract callback that does the actual tracing/marching of rays.
// Must be thread-safe.
class IRaytracer {
 public:
  // TODO: Separate methods for lookup cube and final view tracing.
  virtual void TraceGrid(const RayGrid &grid, std::vector<float> &out_results)
    = 0;
  virtual ~IRaytracer() {};
};

}

