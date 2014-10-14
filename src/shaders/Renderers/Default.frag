#include <Common/Renderer-Api.frag>

vec4 GetColor(RaytracerOutput ray) {
  vec4 background = vec4(.4, .4, .9, 1.);
  vec4 error_color = vec4(1, 0, 0, 1);
  vec3 diffuse_color = vec3(.9, .9, .9);
  vec3 light_dir = normalize(vec3(1,2,3));

  vec4 foreground = vec4(diffuse_color * max(0., dot(ray.Normal, light_dir)), 1);
  return mix(mix(background, foreground, smoothstep(0., 1., ray.hit)),
             error_color, smoothstep(0., 1., ray.error));
}

