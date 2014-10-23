// To create a renderer include this file and implement GetColor function.

#include <Common/Structures.frag>

vec4 GetColor(RaytracerOutput ray);

uniform sampler2D MainTexture;
uniform sampler2D NormalTexture;
uniform sampler2D ColorTexture;

in vec2 ScreenPosition;

layout (location = 0) out vec4 OutColor;

void main() {
  RaytracerOutput ray;
  // TODO: fill ray with data from textures
  //OutColor = GetColor(ray);
  OutColor = vec4(ScreenPosition, 0, 1);
}
