#pragma once

#include "gl-util/framebuffer.h"

namespace fract {

// Raytracing result.
// Used for both screen-space images and lookup cube faces.
struct RaytracedView {
  ivec2 size;

  // 4-channel 32-bit float:
  // bitmask(hit, converged, error), iterations, scaled distance, unused.
  GL::Texture2D main_texture;
  // 4-channel 32-bit float: normal xyz, unused.
  GL::Texture2D normal_texture;
  // 4-channel 32-bit float with anything that can contribute to color
  // (attractors, branching, whatever).
  GL::Texture2D color_texture;

  // To render to all these textures. Order of declarations matters.
  GL::Framebuffer framebuffer;

  RaytracedView(ivec2 size):
    size(size),
    main_texture(size, GL_RGBA32F),
    normal_texture(size, GL_RGBA32F),
    color_texture(size, GL_RGBA32F),
    framebuffer({&main_texture, &normal_texture, &color_texture}) {}
};

}
