#pragma once

#include "raytraced-view.h"

namespace fract {

// Renders raw raytracing result into a pretty image on screen.
class Renderer {
 public:
  Renderer(int width, int height);

  void Render(const RaytracedView &raytraced);
 private:
  
};

}
