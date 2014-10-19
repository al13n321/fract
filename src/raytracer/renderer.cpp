#include "renderer.h"
#include "util/exceptions.h"
#include "gl-util/quad-renderer.h"

namespace fract {

Renderer::Renderer(ConfigPtr config)
  : config_(config)
  , shader_provider_(config, "Predefined/pass.vert", {"renderer"}, {}) {}

void Renderer::Render(
  const RaytracedView &raytraced, int frame_width, int frame_height
) {
  std::shared_ptr<GL::Shader> shader = shader_provider_.Get();
  shader->Use();
  shader->SetTexture("MainTexture", raytraced.main_texture, 0);
  shader->SetTexture("NormalTexture", raytraced.normal_texture, 1);
  shader->SetTexture("ColorTexture", raytraced.color_texture, 2);
  GL::QuadRenderer::defaultInstance()->Render();
}

}
