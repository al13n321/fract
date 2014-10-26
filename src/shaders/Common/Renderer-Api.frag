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
  
  vec4 tex_main = texture(MainTexture, ScreenPosition);
	
	ray.hit = mod(tex_main.x, 2.0);
  ray.converged = mod(floor(tex_main.x * 0.5), 2.0);
  ray.error = mod(floor(tex_main.x * 0.25), 2.0);
  ray.iterations = tex_main.y;
  ray.dist = tex_main.z;

  ray.normal = texture(NormalTexture, ScreenPosition).xyz;
	ray.color = texture(ColorTexture, ScreenPosition);

  OutColor = GetColor(ray);
}
