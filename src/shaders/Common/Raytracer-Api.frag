// To create a raytracer you need to include this file and implement TraceRay function.

#include <Common/Common.frag>

// w is epsilon
RaytracerOutput TraceRay(tvec4 p, vec4 d, ftype scale);


in vec2 ScreenPosition;

layout (location = 0) out vec4 OutMain;
layout (location = 1) out vec4 OutNormal;
layout (location = 2) out vec4 OutColor;

void main() {
  Ray ray = Unproject(ScreenPosition);

  RaytracerOutput res = TraceRay(ray.origin, ray.direction, ray.scale);

  OutMain = vec4(res.error * 8 + res.converged * 4 + res.hit * 2 + res.inside,
    res.iterations, res.dist, 0.0);
  OutNormal = vec4(res.normal, res.ao);
  OutColor = res.color;
}


