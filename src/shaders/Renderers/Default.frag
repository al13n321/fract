#version 410

#include <Common/Renderer-Api.frag>

uniform vec3 BaseColor = vec3(1);
uniform vec3 BackgroundColor = vec3(.4, .4, .9);
uniform vec3 LightDir = normalize(vec3(-1, -2, -3));
uniform mat4x4 TrapColors = mat4x4(
  131., 230., 131., 0, // RGB of trap x
  131., 181., 230., 0, // RGB of trap y
  230., 131., 230., 0, // RGB of trap z
  230., 181., 131., 0) // RGB of trap w
  /255.;


vec4 GetColor(Ray ray, RaytracerOutput traced) {
  vec3 res;

  if (traced.error > .5) {
    res = vec3(1, 0, 0);
  } else if (traced.hit > .5) {
    vec4 t = sin(traced.color) * 2;
    float s = dot(t, vec4(1));
    vec3 surface_color = mix(BaseColor, (TrapColors * t).xyz / s,
      min(1., s));
    vec3 diffuse_light =
      surface_color * (max(0., dot(traced.normal, -LightDir)));
    vec3 camera_light =
      vec3(1) * (max(0., dot(traced.normal, -ray.direction.xyz)));
    vec3 ambient = surface_color;
    res = diffuse_light * .4 + camera_light * .1 + ambient * traced.ao * .4;
    if (traced.inside > .5)
      res.y += .0;
  } else {
    res = BackgroundColor;
  }

  return vec4(res, 1.);
}
