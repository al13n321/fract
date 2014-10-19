#include "raytracer.h"
#include "gl-util/quad-renderer.h"

namespace fract {

Raytracer::Raytracer(ConfigPtr config)
  : config_(config)
  , shader_provider_(config, "Predefined/pass.vert", {"raytracer"},
    {{"Camera-shader", "Predefined/PerspectiveCamera.frag"}}) {}

void Raytracer::TraceGrid(const RayGrid &grid, RaytracedView &target) {
  std::shared_ptr<GL::Shader> shader = shader_provider_.Get();
  target.framebuffer.BindForWriting();
  shader->Use();
  // TODO: set uniforms from grid.
  GL::QuadRenderer::defaultInstance()->Render();
  GL::Framebuffer::Unbind();
}

}
