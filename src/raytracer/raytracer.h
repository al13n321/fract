#pragma once

#include <vector>
#include "gl-util/quad-renderer.h"
#include "resources/shader-provider.h"
#include "ray-grid.h"
#include "raytraced-view.h"

namespace fract {

// An abstract callback that does the actual tracing/marching of rays.
// Reloads the needed shaders and updates uniforms.
class Raytracer {
 public:
  Raytracer(Config::View *config);

  void TraceGrid(const RayGrid &grid, RaytracedView &target);
 private:
  Config::View *config_;
  ShaderProvider shader_provider_;
  GL::QuadRenderer quad_renderer_;
};

}

