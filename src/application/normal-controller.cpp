#include "normal-controller.h"
#include "util/json.h"

namespace fract {

NormalController::NormalController(ConfigPtr config, Camera *camera)
    : config_(config), camera_(camera) {
  resolution_subscription_ = config_->Subscribe({{"resolution"}},
    [this](Config::Version v) {
      resolution_ = JsonUtil::sizeValue(v.Get({"resolution"}));
      if (view_->size.x < resolution_.x ||
          view_->size.y < resolution_.y)
        view_.reset(new RaytracedView(resolution_));
      window_->SetSize(resolution_);
      camera_->set_aspect_ratio((float)resolution_.x / resolution_.y);
    }, Config::SYNC);

  resolution_  = JsonUtil::sizeValue(config->Current().Get({"resolution"}));
  window_.reset(new glfw::Window(resolution_, "upchk"));
  window_->MakeCurrent();
  window_->SetPosition(ivec2(20, 40));

  GL::InitGl3wIfNeeded();
  
  camera->set_aspect_ratio((float)resolution_.x / resolution_.y);

  view_.reset(new RaytracedView(resolution_));

  raytracer_.reset(new Raytracer(config));
  renderer_.reset(new Renderer(config));
}

void NormalController::MakeCurrent() {
  window_->MakeCurrent();
}

void NormalController::Render() {
  RayGrid grid;
  grid.position = camera_->position();
  grid.rotation_projection_inv = camera_->RotationProjectionMatrix().Inverse();
  grid.scale = camera_->scale();
  grid.resolution_width = resolution_.x;
  grid.resolution_height = resolution_.y;
  grid.min_x = 0;
  grid.min_y = 0;
  grid.size_x = resolution_.x;
  grid.size_y = resolution_.y;

  raytracer_->TraceGrid(grid, *view_);

  glViewport(0, 0, resolution_.x, resolution_.y);CHECK_GL_ERROR();
  renderer_->Render(*view_, resolution_);

  window_->SwapBuffers();
}

glfw::Window* NormalController::GetWindow() {
  return window_.get();
}

}
