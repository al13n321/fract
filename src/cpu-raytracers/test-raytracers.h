#pragma once

#include "raytracer/i-raytracer.h"

namespace fract { namespace cpu_raytracers {

  class Gradient: public IRaytracer {
   public:
    Gradient() {}
    void traceGrid(const RayGrid &grid, std::vector<float> &out_results)
      override;
  };

  class Cube: public IRaytracer {
   public:
    Cube() {}
    void traceGrid(const RayGrid &grid, std::vector<float> &out_results)
      override;
  };

}}
