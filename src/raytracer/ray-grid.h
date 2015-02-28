#pragma once

#include "util/vec.h"
#include "util/mat.h"
#include "gl-util/shader.h"

namespace fract {

// Specifies a bunch of rays in space, produced by a camera.
// Also scale (it can be considered a part of ray specification, like "speed").
// The near and far clip planes of the projection matrix should be ignored
//  (all rays should start at camera position and terminate at infinity).
struct RayGrid {
  // Camera.
  dvec3 position;
  fmat4 rotation_projection_inv;

  // World scale.
  double scale;

  // Resolution of the grid.
  ivec2 resolution;

  // Seconds since some fixed point in time.
  float time;

  inline fvec3 GetRayDirection(int x, int y) const {
    return rotation_projection_inv.Transform(fvec3(
      (static_cast<float>(x) + .5f)
        / static_cast<float>(resolution.x) * 2 - 1,
      (static_cast<float>(y) + .5f)
        / static_cast<float>(resolution.y) * 2 - 1,
      .0f)).Normalized();
  }

  // Approximate world-space diameter of screen-space pixel at distance 1.
  inline float GetPixelSizeCoefficient(int x, int y) const {
    fvec3 a = rotation_projection_inv.Transform(fvec3(
      static_cast<float>(x)
        / static_cast<float>(resolution.x) * 2 - 1,
      static_cast<float>(y)
        / static_cast<float>(resolution.y) * 2 - 1,
      .0f));
    fvec3 b = rotation_projection_inv.Transform(fvec3(
      (static_cast<float>(x) + .1f)
        / static_cast<float>(resolution.x) * 2 - 1,
      (static_cast<float>(y) + .1f)
        / static_cast<float>(resolution.y) * 2 - 1,
      .0f));
    return sqrtf(a.DistanceSquare(b) / a.LengthSquare());
  }

  void AssignToUniforms(GL::Shader &shader) const {
    shader.SetVec3("CameraPos", position);
    shader.SetScalar("CameraScale", scale);
    shader.SetMat4("CameraRotProjInv", rotation_projection_inv);
    shader.SetVec2("Resolution", dvec2(resolution));

    shader.SetScalar("Now", time);
  }
};

}
