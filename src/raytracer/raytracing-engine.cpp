#include "raytracing-engine.h"
#include "util/exceptions.h"

namespace fract {

RaytracingEngine::RaytracingEngine(ivec2 size, ConfigPtr config)
: size_(size), config_(config), tracer_(config)
, view_(size) {}

const RaytracedView& RaytracingEngine::Raytrace(
  dvec3 camera_position, double camera_scale, fmat4 camera_rotation_projection
) {
  RayGrid grid;
  grid.position = camera_position;
  grid.rotation_projection_inv = camera_rotation_projection.Inverse();
  grid.scale = camera_scale;
  grid.resolution_width = size_.x;
  grid.resolution_height = size_.y;
  grid.min_x = 0;
  grid.min_y = 0;
  grid.size_x = size_.x;
  grid.size_y = size_.y;

  static std::vector<float> data(4 * size_.x * size_.y);
  tracer_.TraceGrid(grid, view_);

  return view_;
}

}
