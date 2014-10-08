#include "test-raytracers.h"
#include "util/exceptions.h"

namespace fract { namespace cpu_raytracers {

void Gradient::TraceGrid(const RayGrid &grid, std::vector<float> &out_results) {
  for (int y = grid.min_y; y < grid.min_y + grid.size_y; ++y) {
    for (int x = grid.min_x; x < grid.min_x + grid.size_x; ++x) {
      out_results[(y*grid.size_x + x)*4 + 0] = 1;
      out_results[(y*grid.size_x + x)*4 + 1] = x * 10;
      out_results[(y*grid.size_x + x)*4 + 2] = y * 10;
      out_results[(y*grid.size_x + x)*4 + 3] = 0;
    }
  }
}

// De - distance estimate: float DE(fvec3 position, float pixel_size)
// Bnd - bounding volume check: bool BND(fvec3 position, fvec3 direction)
template <typename DE_FUNC, typename BND_FUNC>
static inline void Raymarch(DE_FUNC De, BND_FUNC Bnd,
  fvec3 origin, fvec3 direction, float scale, float pixel_size_coefficient,
  int step_limit, float *out
) {
  fvec3 position = origin;
  float pixel_size = 0;
  float dist = 0;
  for (int i = 0; i < step_limit; ++i) {
    if (!Bnd(position, direction)) {
      // No hit (left bounding volume).
      out[0] = 0;
      out[1] = i;
      out[2] = dist;
      out[3] = 0;
      return;
    }
    float de = De(position, pixel_size);
    if (fabsf(de) <= pixel_size * .5f ||
      (de <= 0 && i == 0)) { // .5f is arbitrary.
      // Hit.
      out[0] = 1;
      out[1] = i;
      out[2] = dist;
      out[3] = 0;
      return;
    }
    dist += de;
    position += direction * de;
    pixel_size += pixel_size_coefficient * de;
  }

  // Iteration limit reached. Count it as a hit.
  out[0] = 1;
  out[1] = step_limit;
  out[2] = dist;
  out[3] = 1000;
}

static inline float CubeDE(fvec3 position, float pixel_size) {
  // Only a lower bound.
  return position.Abs().MaxComponent() - 1 - pixel_size;
}

static inline bool CubeBND(fvec3 position, fvec3 direction) {
  return position.Abs().MaxComponent() <= 1 || !position.AllSameSign(direction);
}

void Cube::TraceGrid(const RayGrid &grid, std::vector<float> &out_results) {
  for (int y = grid.min_y; y < grid.min_y + grid.size_y; ++y) {
    for (int x = grid.min_x; x < grid.min_x + grid.size_x; ++x) {
      fvec3 dir = grid.GetRayDirection(x, y);
      float pix_size_coef = grid.GetPixelSizeCoefficient(x, y);
      Raymarch(
        CubeDE, CubeBND,
        static_cast<fvec3>(grid.position),
        dir,
        static_cast<float>(grid.scale),
        pix_size_coef,
        100,
        &out_results[(y*grid.size_x + x)*4]);
    }
  }
}

}}
