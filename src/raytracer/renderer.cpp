#include "renderer.h"
#include "util/exceptions.h"

namespace fract {

Renderer::Renderer(Config::View *config)
  : config_(config)
  , shader_provider_(config_, "Predefined/pass.vert", {"renderer"},
    {{"Camera-shader", "Predefined/PerspectiveCamera.frag"}}) {}

void Renderer::Render(const RayGrid &grid, const RaytracedView &raytraced) {
  std::shared_ptr<GL::Shader> shader = shader_provider_.Get();
  if (!shader) {
    glClearColor(0.2f,0.8f,0.2f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
  shader->Use();
  shader->SetTexture("MainTexture", raytraced.main_texture, 0);
  shader->SetTexture("NormalTexture", raytraced.normal_texture, 1);
  shader->SetTexture("ColorTexture", raytraced.color_texture, 2);
  grid.AssignToUniforms(*shader);
  quad_renderer_.Render();
}

}
