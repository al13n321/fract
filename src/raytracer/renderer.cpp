#include "renderer.h"
#include "util/exceptions.h"
#include "gl-util/quad-renderer.h"

namespace fract {

Renderer::Renderer(): shader_("shaders/pass.vert", "shaders/main.frag") {
  uniform_main_texture_ = shader_.GetUniformLocation("main_texture");
}

void Renderer::Render(
  const RaytracedView &raytraced, int frame_width, int frame_height
) {
  shader_.Use();
  raytraced.main_texture.AssignToUniform(uniform_main_texture_, 0);
  GL::QuadRenderer::defaultInstance()->Render();
}

}
