#include "raytracing-engine.h"
#include "util/exceptions.h"

namespace fract {

RaytracingEngine::RaytracingEngine(
  std::shared_ptr<ICpuRaytracer> tracer,
  int width, int height)
: tracer_(tracer), view_(width, height) {}

void RaytracingEngine::UpdatePositionAndScale(dvec3 position, double scale) {
  camera_position_ = position;
  camera_scale_ = scale;
}

void RaytracingEngine::UpdateRotationProjectionMatrix(fmat4 mat) {
  camera_rotation_projection_ = mat;
}

const RaytracedView& RaytracingEngine::Raytrace() {
  throw NotImplementedException();
  return view_;
}

}
