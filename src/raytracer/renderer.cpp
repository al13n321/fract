#include "renderer.h"
#include "util/exceptions.h"
#include "gl-util/quad-renderer.h"

namespace fract {

Renderer::Renderer(): shader_("shaders/pass.vert", "shaders/main.frag") {
  uniform_main_texture_ = shader_.GetUniformLocation("main_texture");
  uniform_normal_texture_ = shader_.GetUniformLocation("normal_texture");
  uniform_color_texture_ = shader_.GetUniformLocation("color_texture");
}

void Renderer::Render(
  const RaytracedView &raytraced, int frame_width, int frame_height
) {
  shader_.Use();
  raytraced.main_texture.AssignToUniform(uniform_main_texture_, 0);
  raytraced.normal_texture.AssignToUniform(uniform_normal_texture_, 1);
  raytraced.color_texture.AssignToUniform(uniform_color_texture_, 2);
  GL::QuadRenderer::defaultInstance()->Render();
}

}
