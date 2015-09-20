#version 410
#include <Raytracers/DE-Raytracer.frag>

uniform ftype Radius = 5;

float DE(tvec4 p) {
  return length(p.xyz) - Radius;
}
