#include "raytracer.h"
#include <cassert>

namespace fract {

Raytracer::Raytracer(Config::View *config)
  : config_(config)
  , shader_provider_(config_, "Predefined/pass.vert", {"raytracer"},
    {{"Camera-shader", "Predefined/PerspectiveCamera.frag"}}) {}

void Raytracer::TraceGrid(const RayGrid &grid, RaytracedView &target) {
  assert(grid.resolution == target.size);

  std::shared_ptr<GL::Shader> shader = shader_provider_.Get();
  target.framebuffer.BindForWriting();
  if (!shader) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); CHECK_GL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT); CHECK_GL_ERROR();
    return;
  }

  shader->Use();

  grid.AssignToUniforms(*shader);

  quad_renderer_.Render();
  GL::Framebuffer::Unbind();
}

}
