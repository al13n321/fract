#pragma once

#include "raytraced-view.h"
#include "gl-util/shader.h"

namespace fract {

// Renders raw raytracing result into a pretty image on screen.
// TODO: may add runtime shader reloading for fast experimentation;
//       or tweakable parameters
class Renderer {
 public:
  Renderer();

  void Render(
    const RaytracedView &raytraced, int frame_width, int frame_height);
 private:
  GL::Shader shader_;
  GLint uniform_main_texture_;
  GLint uniform_normal_texture_;
  GLint uniform_color_texture_;
};

}
