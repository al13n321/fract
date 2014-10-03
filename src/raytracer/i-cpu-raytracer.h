#pragma once

#include <vector>
#include "util/vec.h"

namespace fract {

// An abstract callback that does the actual tracing/marching of rays, on CPU.
// Must be thread-safe.
// NOTE: I can further add IOpenClRaytracer or ICudaRaytracer that accept
//       output buffers of corresponding type.
class ICpuRaytracer {
  struct Ray {
    dvec3 start;
    fvec3 direction; // unit length
  };

  struct TraceResult {
    int16_t iterations; // -iterations if hit limit without converging
    float distance; // -1 for no hit
    // NOTE: can add traps, normals, AO, shadows etc.
  };

  typedef std::vector<Ray> Rays;
  typedef std::vector<TraceResult> TraceResults;

  // Assume that time overhead on storing a vector of queries and results
  // is much less than tracing time. TODO: check
  virtual void traceGrid(const Rays &rays, TraceResults &out_results) = 0;
  virtual ~ICpuRaytracer() {};
};

}

