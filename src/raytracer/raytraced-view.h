#pragma once

#include "gl-util/texture2d.h"

namespace fract {


struct RaytracedView {
  // 4-channel 32-bit float: hit (0 or 1), iterations, scaled distance, unused
  Texture2D main_texture;
  // coming soon: normal_texture

  RaytracedView(int width, int height): main_texture(
    width, height, GL_RGBA32F) {}
};

}
