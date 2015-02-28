#include <Common/Common.frag>

uniform tvec3 CameraPos;
uniform ftype CameraScale;
uniform mat4 CameraRotProjInv;
uniform vec2 Resolution;

Ray Unproject(vec2 screen_pos) {
  Ray ray;
  vec2 pix = screen_pos * 2.0 - 1.0;
  ray.direction = CameraRotProjInv * vec4(pix, 0.0f, 1.0f);
  ray.direction.xyz /= ray.direction.w;
  ray.direction.w = 0.0;

  vec4 a = CameraRotProjInv * vec4(
    pix - 1.0 / Resolution, 0.0f, 1.0f);
  vec4 b = CameraRotProjInv * vec4(
    pix + 1.0 / Resolution, 0.0f, 1.0f);
  a.xyz /= a.w;
  b.xyz /= b.w;
  ray.direction.w = distance(a.xyz, b.xyz) / length(ray.direction.xyz);
  ray.direction.xyz = normalize(ray.direction.xyz);

  ray.origin = tvec4(CameraPos, 0.0);
  ray.scale = CameraScale;

  return ray;
}
