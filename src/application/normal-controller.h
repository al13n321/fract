#pragma once

#include "controller.h"
#include "resources/config.h"
#include "util/camera.h"
#include "raytracer/raytracer.h"
#include "raytracer/renderer.h"
#include "gl-util/glfw-util.h"

namespace fract {

// Controller that renders to a window.
class NormalController : public Controller {
 public:
  NormalController(ConfigPtr config, Camera *camera);

  void MakeCurrent() override;
  void Render() override;
  glfw::Window* GetWindow() override;

 private:
  ConfigPtr config_;
  Camera *camera_;
  // Window must be destroyed after everything that can make OpenGL calls.
  std::unique_ptr<glfw::Window> window_;
  std::unique_ptr<RaytracedView> view_;
  std::unique_ptr<Raytracer> raytracer_;
  std::unique_ptr<Renderer> renderer_;

  ivec2 resolution_;

  Config::SubscriptionPtr resolution_subscription_;
};

}
