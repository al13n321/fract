#version 410

#define BOUNDS 1000.0

#include <Raytracers/DE-Raytracer.frag>


uniform float Radius = .3;

float DE(vec4 p) {
  vec4 p0 = p;
  float t0 = sin(length(p.xz) * .5);
  float t = t0 * .2;
  p.y += sin(t0 * .1) * 10;
  p.xz = mat2(sin(t), cos(t), -cos(t), sin(t)) * p.xz;
  p.xz = fract(p.xz) - .5;
  return max(length(p.xyz) - Radius, length(p0.xyz) - sqrt(BOUNDS));
}
