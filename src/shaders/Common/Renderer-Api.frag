// To create a renderer include this file and implement GetColor function.

#include <Common/Structures.frag>

vec4 GetColor(RaytracerOutput ray);

sampler2D MainTexture;
sampler2D NormalTexture;
sampler2D ColorTexture;

in vec2 ScreenPosition;

void main() {
  RaytracerOutput ray;
  // TODO: fill ray with data from textures
  gl_FragColor = GetColor(ray);
}

