#include "renderer.h"
#include "util/exceptions.h"

namespace fract {

Renderer::Renderer(ConfigPtr config)
  : config_(config)
  , shader_provider_(config, "Predefined/pass.vert", {"renderer"}, {}) {}

void Renderer::Render(
  const RaytracedView &raytraced, int frame_width, int frame_height
) {
  std::shared_ptr<GL::Shader> shader = shader_provider_.Get();
  glViewport(0, 0, frame_width, frame_height);
  if (!shader) {
    glClearColor(0.2f,0.8f,0.2f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
  shader->Use();
  shader->SetTexture("MainTexture", raytraced.main_texture, 0);
  shader->SetTexture("NormalTexture", raytraced.normal_texture, 1);
  shader->SetTexture("ColorTexture", raytraced.color_texture, 2);
  quad_renderer_.Render();
}

}
