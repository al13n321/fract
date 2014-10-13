#pragma once

#include "gl-util/texture2d.h"

namespace fract {

// Raytracing result.
// Used for both screen-space images and lookup cube faces.
struct RaytracedView {
  // TODO: make them render targets.
  // 4-channel 32-bit float: hit (0 or 1), iterations, scaled distance, error.
  GL::Texture2D main_texture;
  // 4-channel 32-bit float: normal xyz, "roughness" (or unused).
  GL::Texture2D normal_texture;
  // 4-channel 32-bit float with anything that can contribute to color
  // (attractors, branching, whatever).
  GL::Texture2D color_texture;

  RaytracedView(int width, int height):
    main_texture(width, height, GL_RGBA32F, GL_LINEAR),
    normal_texture(width, height, GL_RGBA32F, GL_LINEAR),
    color_texture(width, height, GL_RGBA32F, GL_LINEAR) {}
};

}
