#pragma once

#include "raytracer.h"
#include "util/camera.h"
#include "raytraced-view.h"
#include "resources/config.h"

namespace fract {

class RaytracingEngine {
 public:
  RaytracingEngine(ivec2 size, ConfigPtr config);

  // TODO:
  //void SetCubeEnabled(bool enabled);
  //void RepositionCube(dvec3 position);

  const RaytracedView& Raytrace(
    dvec3 camera_position,
    double camera_scale,
    fmat4 camera_rotation_projection_);
 private:
  ivec2 size_;
  ConfigPtr config_;

  Raytracer tracer_;
  RaytracedView view_;
};

}
