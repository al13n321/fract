#include "normal-controller.h"
#include "util/json.h"

namespace fract {

NormalController::NormalController(Config::View *config, Camera *camera)
    : config_(config->Config()->NewContext()), camera_(camera) {
  resolution_subscription_ = config_->Subscribe({{"resolution"}},
    [this](Config::Version v) {
      resolution_ = JsonUtil::sizeValue(v.Get({"resolution"}));
      if (view_->size == resolution_)
        return;
      view_.reset(new RaytracedView(resolution_));
      window_->SetSize(resolution_);
      camera_->set_aspect_ratio((float)resolution_.x / resolution_.y);
    }, Config::SYNC);

  resolution_  = JsonUtil::sizeValue(config->Current().Get({"resolution"}));
  window_.reset(new glfw::Window(resolution_, "window"));
  window_->MakeCurrent();
  window_->SetPosition(ivec2(20, 40));

  GL::InitGl3wIfNeeded();

  camera->set_aspect_ratio((float)resolution_.x / resolution_.y);

  view_.reset(new RaytracedView(resolution_));

  raytracer_.reset(new Raytracer(config));
  renderer_.reset(new Renderer(config));
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

  raytracer_->TraceGrid(grid, *view_);

  glViewport(0, 0, resolution_.x, resolution_.y);CHECK_GL_ERROR();
  renderer_->Render(*view_, resolution_);

  window_->SwapBuffers();
}

glfw::Window* NormalController::GetWindow() {
  return window_.get();
}

}
