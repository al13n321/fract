#version 410

#define BOUNDS 1000.0

#include <Raytracers/DE-Raytracer.frag>


uniform ftype Radius = .3;

float DE(tvec4 p0) {
  tvec3 p = p0.xyz - vec3(0, -5, -10);
  float t0 = sin(length(p.xz) * .5 + Now * 2);
  float t = t0 * .2;
  p.y += t0;
  p.xz = mat2(sin(t), cos(t), -cos(t), sin(t)) * p.xz;
  p.xz = fract(p.xz) - .5;
  return max(length(p.xyz) - Radius, length(p0.xyz) - sqrt(BOUNDS));
}
