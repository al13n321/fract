#include "normal-controller.h"
#include "util/json.h"

namespace fract {

NormalController::NormalController(Config::View *config, Camera *camera)
    : config_(config->Config()->NewContext()), camera_(camera) {
  resolution_subscription_ = config_->Subscribe({{"resolution"}},
    [this](Config::Version v) {
      resolution_ = JsonUtil::sizeValue(
        v.TryGet({"resolution"}), ivec2(500, 500));
      if (view_->size == resolution_)
        return;
      view_.reset(new RaytracedView(resolution_));
      window_->SetSize(resolution_);
      camera_->set_aspect_ratio((float)resolution_.x / resolution_.y);
    }, Config::SYNC);

  resolution_ = JsonUtil::sizeValue(
    config_->Current().Get({"resolution"}), ivec2(500, 500));
  window_.reset(new glfw::Window(ivec2(20, 40), resolution_, "window", false));
  window_->MakeCurrent();

  GL::InitGl3wIfNeeded();

  camera->set_aspect_ratio((float)resolution_.x / resolution_.y);

  view_.reset(new RaytracedView(resolution_));

  raytracer_.reset(new Raytracer(config_.get()));
  renderer_.reset(new Renderer(config_.get()));
}

NormalController::~NormalController() {
  window_->MakeCurrent();
}

void NormalController::Activate() {
  window_->MakeCurrent();
}

void NormalController::Deactivate() {
  glViewport(0, 0, resolution_.x, resolution_.y);CHECK_GL_ERROR();
  glClearColor(0.0, 0.0, 0.0, 1.0);CHECK_GL_ERROR();
  glClear(GL_COLOR_BUFFER_BIT);CHECK_GL_ERROR();

  window_->SwapBuffers();
}

void NormalController::Render() {
  config_->PollUpdates();

  RayGrid grid;
  grid.position = camera_->Position();
  grid.rotation_projection_inv = camera_->RotationProjectionMatrix().Inverse();
  grid.scale = camera_->scale();
  grid.resolution = resolution_;
  // Time since something like midnight today. Time since epoch
  // is too big to be represented in 32-bit flost precisely enough.
  grid.time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::high_resolution_clock::now().time_since_epoch()).count()
      % (24ll * 3600 * 1000000) * 0.000001;

  raytracer_->TraceGrid(grid, *view_);

  // XXX: * 2 is a hack for retina; get rid of it as soon as I have internet connection
  glViewport(0, 0, resolution_.x * 2, resolution_.y * 2);CHECK_GL_ERROR();
  renderer_->Render(grid, *view_);

  window_->SwapBuffers();
}

glfw::Window* NormalController::GetWindow() {
  return window_.get();
}

}
