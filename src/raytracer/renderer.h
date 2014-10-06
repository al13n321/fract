#pragma once

#include "raytraced-view.h"

namespace fract {

// Renders raw raytracing result into a pretty image on screen.
class Renderer {
 public:
  Renderer();

  void Render(
    const RaytracedView &raytraced, int frame_width, int frame_height);
 private:
  
};

}
