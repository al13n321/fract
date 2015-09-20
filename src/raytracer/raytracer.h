#pragma once

#include <vector>
#include "gl-util/quad-renderer.h"
#include "resources/shader-provider.h"
#include "ray-grid.h"
#include "raytraced-view.h"

namespace fract {

// An abstract callback that does the actual tracing/marching of rays.
// Reloads the needed shaders and updates uniforms.
class IRaytracer {
 public:
  virtual ~IRaytracer() {}
  virtual void TraceGrid(const RayGrid &grid, RaytracedView &target) = 0;
};

class Raytracer : public IRaytracer {
 public:
  Raytracer(Config::View *config);

  void TraceGrid(const RayGrid &grid, RaytracedView &target) override;
 private:
  Config::View *config_;
  ShaderProvider shader_provider_;
  GL::QuadRenderer quad_renderer_;
};

}
