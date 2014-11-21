#include "raytracer.h"
#include <cassert>

namespace fract {

Raytracer::Raytracer(Config::View *config)
  : config_(config)
  , shader_provider_(config, "Predefined/pass.vert", {"raytracer"},
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

  shader->SetVec3("CameraPos", grid.position);
  shader->SetScalar("CameraScale", grid.scale);
  shader->SetMat4("CameraRotProjInv", grid.rotation_projection_inv);
  shader->SetVec2("Resolution", dvec2(grid.resolution));

  quad_renderer_.Render();
  GL::Framebuffer::Unbind();
}

}
