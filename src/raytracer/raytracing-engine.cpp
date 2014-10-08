#include "raytracing-engine.h"
#include "util/exceptions.h"

namespace fract {

RaytracingEngine::RaytracingEngine(
  std::shared_ptr<IRaytracer> tracer,
  int width, int height)
: tracer_(tracer), view_(width, height) {}

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
  // TODO: implementation
  return view_;
}

}
