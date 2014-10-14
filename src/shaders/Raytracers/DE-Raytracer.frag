#include <Common/Api.frag>

struct SurfaceInfo {
  vec3 normal;
  // not necessarily color itself, just anything contributing to it
  // (traps, conditional paths, whatever)
  vec4 color;
}


