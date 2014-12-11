#version 410

#include <Common/Renderer-Api.frag>

uniform vec3 BaseColor = vec3(.9, .9, .9);
uniform vec3 BackgroundColor = vec3(.4, .4, .9);
uniform vec3 LightDir = normalize(vec3(1, 2, 3));
uniform mat4x4 TrapColors = mat4x4(
  131., 230., 131., 0, // RGB of trap x
  131., 181., 230., 0, // RGB of trap y
  230., 131., 230., 0, // RGB of trap z
  230., 181., 131., 0) // RGB of trap w
  /255.;


vec4 GetColor(RaytracerOutput ray) {
  vec3 res;

  if (ray.error > .5) {
    res = vec3(1, 0, 0);
  } else if (ray.hit > .5) {
    vec3 color = BaseColor + (transpose(TrapColors) * ray.color).xyz;
    res = color * max(0., dot(ray.normal, LightDir));
    if (ray.inside > .5)
      res.y += .0;
  } else {
    res = BackgroundColor;
  }

  return vec4(res, 1.);
}
