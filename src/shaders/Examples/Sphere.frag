#include <Raytracers/DE-Raytracer.frag>

uniform float radius;

float DE(vec3 p) {
  return length(p) - radius;
}

void Surface(vec3 p, float pixel_size, inout RaytracerOutput res) {
  res.normal = normalized(p);
  // place for traps, conditional paths etc.
  res.color = vec4(atan2(p.y,p.x), atan2(p.z,p.y), atan2(p.x,p.z));
}

