#pragma once

#include "i-raytracer.h"
#include "util/camera.h"
#include "raytraced-view.h"

namespace fract {

// Draws the image, using the provided callback to trace the rays.
class RaytracingEngine {
 public:
  RaytracingEngine(
    std::shared_ptr<IRaytracer> tracer,
    int width, int height);

  // Invalidates lookup cube.
  void UpdatePosition(dvec3 position);
  // Can invalidate lookup cube (because of normals mostly)
  // or can do something smart.
  void UpdateScale(double scale);
  // Can keep lookup cube.
  void UpdateRotationProjectionMatrix(fmat4 mat);

  // position_delta allows rendering stereo image and reacting to oculus rift
  // head movement without invalidating lookup cube.
  const RaytracedView& Raytrace(fvec3 position_delta = fvec3(0, 0, 0));
 private:
  int width_;
  int height_;

  std::shared_ptr<IRaytracer> tracer_;
  RaytracedView view_;

  double camera_scale_;
  dvec3 camera_position_;
  fmat4 camera_rotation_projection_;
};

}
