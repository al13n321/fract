#include "raytracing-engine.h"
#include "util/exceptions.h"

namespace fract {

RaytracingEngine::RaytracingEngine(int width, int height, ConfigPtr config)
: width_(width), height_(height), config_(config), view_(width, height) {}

const RaytracedView& RaytracingEngine::Raytrace(
  dvec3 camera_position, double camera_scale, fmat4 camera_rotation_projection
) {
  RayGrid grid;
  grid.position = camera_position;
  grid.rotation_projection_inv = camera_rotation_projection.Inverse();
  grid.scale = camera_scale;
  grid.resolution_width = width_;
  grid.resolution_height = height_;
  grid.min_x = 0;
  grid.min_y = 0;
  grid.size_x = width_;
  grid.size_y = height_;

  static std::vector<float> data(4 * width_ * height_);
  tracer_.TraceGrid(grid, view_);

  return view_;
}

}
