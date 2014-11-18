#include "ovr-controller.h"
#include "util/json.h"
#include "util/exceptions.h"
#include "util/string-util.h"

namespace fract {

OVRController::OVRController(ConfigPtr config, Camera *camera)
    : config_(config), camera_(camera) {
  window_.reset(new glfw::Window(hmd_.GetResolution(),
    "vr"));
  window_->SetPosition(hmd_.GetWindowPos());
  window_->MakeCurrent();

  GL::InitGl3wIfNeeded();

  raytracer_.reset(new Raytracer(config));
  renderer_.reset(new Renderer(config));

  pixel_density_subscription_ = config_->Subscribe({{"pixel_density_"}},
    [this](Config::Version v) {
      double x = JsonUtil::doubleValue(v.Get({"pixel_density"}));
      if (x <= 0 || x > 10)
        throw ConfigValueFormatException(
          "invalid pixel_density: " + ToString(x));
      pixel_density_ = (float)x;
      eyes_[0] = EyeData(hmd_.GetTextureSize(ovrEye_Left, pixel_density_));
      eyes_[1] = EyeData(hmd_.GetTextureSize(ovrEye_Right, pixel_density_));
    }, Config::SYNC_NOW);

  hmd_.ConfigureRendering();
  
  Deactivate();
}

OVRController::~OVRController() {
  window_->MakeCurrent();
}

void OVRController::Activate() {
  window_->MakeCurrent();
  hmd_.StartTracking();
}

void OVRController::Deactivate() {
  glViewport(0, 0, hmd_.GetResolution().x, hmd_.GetResolution().y);
    CHECK_GL_ERROR();
  glClearColor(0.0, 0.0, 0.0, 1.0);CHECK_GL_ERROR();
  glClear(GL_COLOR_BUFFER_BIT);CHECK_GL_ERROR();

  window_->SwapBuffers();

  hmd_.StopTracking();
}

void OVRController::Render() {
  hmd_.BeginFrame();
  hmd_.GetEyePoses({&eyes_[0].pose, &eyes_[1].pose});

  for (EyeData &eye: eyes_) {
    RayGrid grid;
    grid.position = camera_->position();
    grid.rotation_projection_inv =
      camera_->RotationProjectionMatrix().Inverse();
    grid.scale = camera_->scale();
    grid.resolution = eye.resolution;

    raytracer_->TraceGrid(grid, *eye.view);

    eye.framebuffer->BindForWriting();
    renderer_->Render(*eye.view, eye.resolution);
  }

  GL::Framebuffer::Unbind();
  glViewport(0, 0, hmd_.GetResolution().x, hmd_.GetResolution().y);
    CHECK_GL_ERROR();

  hmd_.EndFrame({eyes_[0].texture.get(), eyes_[1].texture.get()});
}

glfw::Window* OVRController::GetWindow() {
  return window_.get();
}

}
