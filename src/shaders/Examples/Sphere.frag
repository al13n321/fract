#version 410

#include <Raytracers/DE-Raytracer.frag>

uniform float radius;

float DE(vec3 p) {
  return length(p) - radius;
}

void Surface(vec3 p, float pixel_size, inout RaytracerOutput res) {
  res.normal = normalize(p);
  // place for traps, conditional paths etc.
  res.color = vec4(atan(p.y,p.x), atan(p.z,p.y), atan(p.x,p.z), 1);
}

