#pragma once

#include "raytracer/raytracer.h"

namespace fract {

class ExperimentalCPURaytracer : public IRaytracer {
 public:
  ExperimentalCPURaytracer() {}

  void TraceGrid(const RayGrid &grid, RaytracedView &target) override;
 private:
  bool done_ = false;
};

}

