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
  OVRController(Config::View *config, Camera *camera);
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
    ovrEyeRenderDesc render_desc;
    ovrPosef pose;

    void SetResolution(ivec2 res) {
      resolution = res;
      view.reset(new RaytracedView(resolution));
      framebuffer.reset();
      texture.reset(new GL::Texture2D(resolution, GL_RGB8, GL_LINEAR));
      framebuffer.reset(new GL::Framebuffer({texture.get()}));
    }
  };

  Config::ContextPtr config_;
  Camera *camera_;

  std::unique_ptr<glfw::Window> window_;
  ovr::HMD hmd_;

  std::unique_ptr<Raytracer> raytracer_;
  std::unique_ptr<Renderer> renderer_;

  EyeData eyes_[2];

  float pixel_density_;
  // If monoscopic rendering is enabled, only render left eye
  // (using average pose).
  bool is_monoscopic_;

  Config::SubscriptionPtr subscription_;
};

}
