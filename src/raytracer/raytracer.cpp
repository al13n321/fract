#include "raytracer.h"
#include <cassert>

namespace fract {

Raytracer::Raytracer(ConfigPtr config)
  : config_(config)
  , shader_provider_(config, "Predefined/pass.vert", {"raytracer"},
    {{"Camera-shader", "Predefined/PerspectiveCamera.frag"}}) {}

void Raytracer::TraceGrid(const RayGrid &grid, RaytracedView &target) {
  assert(grid.resolution_width == target.size.x);
  assert(grid.resolution_height == target.size.y);

  std::shared_ptr<GL::Shader> shader = shader_provider_.Get();
  target.framebuffer.BindForWriting();
  glViewport(0, 0, target.size.x, target.size.y);
  if (!shader) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); CHECK_GL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT); CHECK_GL_ERROR();
    return;
  }

  shader->Use();

  shader->SetVec3("CameraPos", grid.position);
  shader->SetScalar("CameraScale", grid.scale);
  shader->SetMat4("CameraRotProjInv", grid.rotation_projection_inv);
  shader->SetVec2("Resolution", dvec2(
    1. * grid.resolution_width,
    1. * grid.resolution_height));
  shader->SetVec2("ViewportOrigin", dvec2(
    1. * grid.min_x / grid.resolution_width,
    1. * grid.min_y / grid.resolution_height));
  shader->SetVec2("ViewportSize", dvec2(
    1. * grid.size_x / grid.resolution_width,
    1. * grid.size_y / grid.resolution_height));

  quad_renderer_.Render();
  GL::Framebuffer::Unbind();
}

}
