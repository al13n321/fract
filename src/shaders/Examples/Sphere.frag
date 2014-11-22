#version 410
#include <Raytracers/DE-Raytracer.frag>

uniform float Radius = 1;

float DE(vec4 p) {
  return length(p) - Radius;
}
