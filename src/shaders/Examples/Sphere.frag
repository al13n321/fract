#version 410
#include <Raytracers/DE-Raytracer.frag>

uniform float radius = 1;

float DE(vec4 p) {
  return length(p) - radius;
}

void Surface(vec4 p, inout RaytracerOutput res) {
  res.normal = normalize(p.xyz);
  // place for traps, conditional paths etc.
  res.color = vec4(atan(p.y,p.x), atan(p.z,p.y), atan(p.x,p.z), 1);
}
