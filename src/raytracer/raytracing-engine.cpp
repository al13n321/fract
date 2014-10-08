#include "raytracing-engine.h"
#include "util/exceptions.h"

namespace fract {

RaytracingEngine::RaytracingEngine(
  std::shared_ptr<IRaytracer> tracer,
  int width, int height)
: width_(width), height_(height), tracer_(tracer), view_(width, height) {}

void RaytracingEngine::UpdatePosition(dvec3 position) {
  camera_position_ = position;
}

void RaytracingEngine::UpdateScale(double scale) {
  camera_scale_ = scale;
}

void RaytracingEngine::UpdateRotationProjectionMatrix(fmat4 mat) {
  camera_rotation_projection_ = mat;
}

const RaytracedView& RaytracingEngine::Raytrace(fvec3 position_delta) {
  RayGrid grid;
  grid.position = camera_position_;
  grid.rotation_projection_inv = camera_rotation_projection_.Inverse();
  grid.scale = camera_scale_;
  grid.resolution_width = width_;
  grid.resolution_height = height_;
  grid.min_x = 0;
  grid.min_y = 0;
  grid.size_x = width_;
  grid.size_y = height_;

  static std::vector<float> data(4 * width_ * height_);
  tracer_->TraceGrid(grid, data);
  view_.main_texture.SetPixels(GL_RGBA, GL_FLOAT, &data[0]);

  return view_;
}

}
