#pragma once

#include "raytraced-view.h"
#include "gl-util/quad-renderer.h"
#include "gl-util/shader.h"
#include "resources/config.h"
#include "resources/shader-provider.h"

namespace fract {

// Renders raw raytracing result into a pretty image on screen.
class Renderer {
 public:
  Renderer(Config::View *config);

  void Render(
    const RaytracedView &raytraced, ivec2 frame_size);
 private:
  Config::View *config_;
  ShaderProvider shader_provider_;
  GL::QuadRenderer quad_renderer_;
};

}
