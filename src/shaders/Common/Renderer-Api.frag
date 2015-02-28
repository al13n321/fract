// To create a renderer include this file and implement GetColor function.

#include <Common/Common.frag>

vec4 GetColor(Ray ray, RaytracerOutput traced);

uniform sampler2D MainTexture;
uniform sampler2D NormalTexture;
uniform sampler2D ColorTexture;

in vec2 ScreenPosition;

layout (location = 0) out vec4 OutColor;

void main() {
  Ray ray = Unproject(ScreenPosition);
  RaytracerOutput traced;

  vec4 tex_main = texture(MainTexture, ScreenPosition);

  traced.inside = mod(tex_main.x, 2.0);
	traced.hit = mod(floor(tex_main.x * 0.5), 2.0);
  traced.converged = mod(floor(tex_main.x * 0.25), 2.0);
  traced.error = mod(floor(tex_main.x * 0.125), 2.0);
  traced.iterations = tex_main.y;
  traced.dist = tex_main.z;

  vec4 tex_normal = texture(NormalTexture, ScreenPosition);
  traced.normal = tex_normal.xyz;
  traced.ao = tex_normal.w;
	traced.color = texture(ColorTexture, ScreenPosition);

  OutColor = GetColor(ray, traced);
}
