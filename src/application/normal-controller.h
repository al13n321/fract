#pragma once

#include "controller.h"
#include "resources/camera.h"
#include "resources/config.h"
#include "raytracer/raytracer.h"
#include "raytracer/renderer.h"
#include "gl-util/glfw-util.h"

namespace fract {

// Controller that renders to a window.
class NormalController : public Controller {
 public:
  NormalController(Config::View *config, Camera *camera);
  ~NormalController();

  void Activate() override;
  void Deactivate() override;
  void Render() override;
  glfw::Window* GetWindow() override;

  std::unique_ptr<IRaytracer> ExchangeRaytracer(std::unique_ptr<IRaytracer> rt);

 private:
  Config::ContextPtr config_;
  Camera *camera_;
  // Window must be destroyed after everything that can make OpenGL calls.
  std::unique_ptr<glfw::Window> window_;
  std::unique_ptr<RaytracedView> view_;
  std::unique_ptr<IRaytracer> raytracer_;
  std::unique_ptr<Renderer> renderer_;

  ivec2 resolution_;
  // Window framebuffer size. Different from resolution_ on retina display.
  ivec2 viewport_;

  Config::SubscriptionPtr resolution_subscription_;
};

}
