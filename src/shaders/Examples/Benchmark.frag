#version 410
#include <Common/Raytracer-Api.frag>

// Does about Ops floating point operations per pixel.
// Don't rely on this code too much (or at all).

uniform int Ops = 200000;

RaytracerOutput TraceRay(tvec4 origin, vec4 direction, ftype scale) {
  RaytracerOutput res;

  res.hit = 0;
  res.converged = 0;
  res.error = 0;
  res.iterations = 0;
  res.normal = vec3(0, 0, 0);
  res.color = vec4(1e20, 1e20, 1e20, 1e20);

  tvec4 a = vec4(1,2,3,4);
  tvec4 b = vec4(9,8,7,6);
  for (int i = 0; i < Ops; i += 32) {
    a += b;
    b *= a;
    a = (a + b + 1) / 3.0;
    b = (a + b + 2) / 3.0;
  }

  res.color = vec4(a);

  return res;
}
