#pragma once

#include "raytracer/i-cpu-raytracer.h"

namespace fract { namespace cpu_raytracers {

  class Gradient: public ICpuRaytracer {
   public:
    Gradient() {}
    void traceGrid(const Rays &rays, TraceResults &out_results) override;
  };

  class Cube: public ICpuRaytracer {
   public:
    Cube() {}
    void traceGrid(const Rays &rays, TraceResults &out_results) override;
  };

}}
