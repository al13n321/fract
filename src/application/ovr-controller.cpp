#include "ovr-controller.h"
#include "util/json.h"
#include "util/exceptions.h"
#include "util/string-util.h"

namespace fract {

OVRController::OVRController(Config::View *config, Camera *camera)
    : config_(config->Config()->NewContext()), camera_(camera) {
  window_.reset(new glfw::Window(hmd_.GetResolution(),
    "vr"));
  window_->SetPosition(hmd_.GetWindowPos());
  window_->MakeCurrent();

  GL::InitGl3wIfNeeded();

  raytracer_.reset(new Raytracer(config));
  renderer_.reset(new Renderer(config));

  pixel_density_ = -1;

  subscription_ = config_->Subscribe({{"pixel_density"}, {"monoscopic"}},
    [this](Config::Version v) {
      double x = JsonUtil::doubleValue(v.Get({"pixel_density"}));
      if (x <= 0 || x > 10)
        throw ConfigValueFormatException(
          "invalid pixel_density: " + ToString(x));
      float was_density = pixel_density_;
      bool was_monoscopic = is_monoscopic_;
      is_monoscopic_ = v.TryGet({"monoscopic"}).asBool();
      pixel_density_ = (float)x;
      if (was_density > 0 && is_monoscopic_ != was_monoscopic)
        hmd_.FreeRendering();
      if (was_density <= 0 || is_monoscopic_ != was_monoscopic)
        hmd_.ConfigureRendering(window_.get(), is_monoscopic_);
      eyes_[0].SetResolution(hmd_.GetTextureSize(ovrEye_Left, pixel_density_));
      if (!is_monoscopic_)
        eyes_[1].SetResolution(
          hmd_.GetTextureSize(ovrEye_Right, pixel_density_));
    }, Config::SYNC_NOW);

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
  camera_->ClearPose();
}

void OVRController::Render() {
  config_->PollUpdates();

  hmd_.BeginFrame();
  hmd_.GetEyeRenderDescs({&eyes_[0].render_desc, &eyes_[1].render_desc});
  hmd_.GetEyePoses({&eyes_[0].pose, &eyes_[1].pose});

  camera_->SetPose((ovr::conv(eyes_[0].pose.Position)
                  + ovr::conv(eyes_[1].pose.Position)) / 2,
                  ((ovr::conv(eyes_[0].pose.Orientation)
                  + ovr::conv(eyes_[1].pose.Orientation)) / 2).Normalized());

  int i = -1;
  for (EyeData &eye: eyes_) {
    ++i;
    RayGrid grid;
    grid.position = camera_->Position(ovr::conv(eye.pose.Position));
    grid.rotation_projection_inv =
      camera_->Rotation(ovr::conv(eye.pose.Orientation)).ToMatrix()
      * ovr::ProjectionMatrix(eye.render_desc.Fov).Inverse();
    grid.scale = camera_->scale();
    grid.resolution = eye.resolution;

    raytracer_->TraceGrid(grid, *eye.view);

    eye.framebuffer->BindForWriting();
    renderer_->Render(*eye.view, eye.resolution);

    if (is_monoscopic_)
      break;
  }

  GL::Framebuffer::Unbind();
  glViewport(0, 0, hmd_.GetResolution().x, hmd_.GetResolution().y);
    CHECK_GL_ERROR();

  hmd_.EndFrame({eyes_[0].texture.get(),
    eyes_[is_monoscopic_ ? 0 : 1].texture.get()});
}

glfw::Window* OVRController::GetWindow() {
  return window_.get();
}

}
