#pragma once

#include "gl-util/texture2d.h"

namespace fract {


struct RaytracedView {
  // 4-channel 32-bit float: hit (0 or 1), iterations, scaled distance, error.
  Texture2D main_texture;
  // 4-channel 32-bit float: normal xyz, "roughness" (or unused).
  Texture2D normal_texture;
  // 4-channel 32-bit float with anything that can contribute to color
  // (attractors, branching, whatever).
  Texture2D color_texture;

  RaytracedView(int width, int height):
    main_texture(width, height, GL_RGBA32F, GL_LINEAR),
    normal_texture(width, height, GL_RGBA32F, GL_LINEAR),
    color_texture(width, height, GL_RGBA32F, GL_LINEAR) {}
};

}
