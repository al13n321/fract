#pragma once

#include "controller.h"
#include "resources/camera.h"
#include "resources/config.h"
#include "raytracer/raytracer.h"
#include "raytracer/renderer.h"
#include "gl-util/glfw-util.h"
#include "vr/ovr-util.h"

namespace fract {

// Controller that renders to a window.
class OVRController : public Controller {
 public:
  OVRController(ConfigPtr config, Camera *camera);
  ~OVRController();

  bool CaptureMouse() override { return true; }
  void Activate() override;
  void Deactivate() override;
  void Render() override;
  glfw::Window* GetWindow() override;

 private:
  struct EyeData {
    ivec2 resolution;
    std::unique_ptr<RaytracedView> view;
    std::unique_ptr<GL::Texture2D> texture;
    std::unique_ptr<GL::Framebuffer> framebuffer;
    ovrPosef pose;

    EyeData() {}
    EyeData(const EyeData &rhs) = delete;
    EyeData& operator=(const EyeData &rhs) = delete;
    EyeData& operator=(EyeData &&rhs) {
      resolution = rhs.resolution;
      view = std::move(rhs.view);
      texture = std::move(rhs.texture);
      framebuffer = std::move(rhs.framebuffer);
      pose = rhs.pose;
      return *this;
    }
    EyeData(EyeData &&rhs) {
      *this = std::move(rhs);
    }
    explicit EyeData(ivec2 resolution)
      : resolution(resolution),
        view(new RaytracedView(resolution)),
        texture(new GL::Texture2D(resolution, GL_RGB8, GL_LINEAR)),
        framebuffer(new GL::Framebuffer({texture.get()})) {}
  };

  ConfigPtr config_;
  Camera *camera_;

  std::unique_ptr<glfw::Window> window_;
  std::unique_ptr<Raytracer> raytracer_;
  std::unique_ptr<Renderer> renderer_;

  ovr::HMD hmd_;

  EyeData eyes_[2];

  float pixel_density_;

  Config::SubscriptionPtr pixel_density_subscription_;
};

}
