$uniform float radius

// this unif_* syntax is outrageously ugly; need to do something about it!

float impl(vec3 p, $unif_decl) {
  return length(p) - unif_radius;
}

float DE(vec3 p, $unif_decl) {
  return impl($unif_pass);
}

vec4 Normal(vec3 p) {
  return normalize(p);
}

// traps, conditional paths etc.
// TODO (low pri): named returns, arbitrary number of them,
//   preprocessor definitions indicating which of them are actually used by shader,
//   to avoid computing the unused ones; a general mechanism for such things
//   (that sounds cool but unreasonably complicated)
vec4 SurfaceInfo(vec3 p) {
  ...
}

