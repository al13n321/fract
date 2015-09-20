#version 410
#include <Raytracers/DE-Raytracer.frag>

uniform ftype Radius = 5;
uniform ftype Scale = 3;

ftype DE(tvec4 p) {
  tvec4 p0 = p;
  p.y += 2;
  ftype res = 1e20;
  ftype scale = 1;

  for (int i = 0; i < 2; ++i) {
    res = min(res, p.y * scale);
    p.xz = abs(p.xz);
    p.y = (dot(tvec3(.354, .866, .354), p.xyz) - .7) * Scale;/*
    p.xyz = mat3(.707, .0  ,-.707,
                 .354, .866, .354,
                -.612, .5  ,-.612) * p.xyz * Scale;*/
    scale /= Scale;
  }

  return max(res, length(p0.xyz) - 10);
}
