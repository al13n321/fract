#include "test-raytracers.h"
#include "util/exceptions.h"

namespace fract { namespace cpu_raytracers {

void Gradient::traceGrid(const RayGrid &grid, std::vector<float> &out_results) {
  for (int y = grid.min_y; y < grid.min_y + grid.size_y; ++y) {
    for (int x = grid.min_x; x < grid.min_x + grid.size_x; ++x) {
      fvec3 dir = grid.GetRayDirection(x, y);
      out_results[(y*grid.size_x + x)*4 + 0] = 1;
      out_results[(y*grid.size_x + x)*4 + 1] = 100;
      out_results[(y*grid.size_x + x)*4 + 2] = fabsf(dir.Dot(fvec3(1, 1, 1)));
      out_results[(y*grid.size_x + x)*4 + 3] = 0;
    }
  }
}

void Cube::traceGrid(const RayGrid &grid, std::vector<float> &out_results) {
  throw NotImplementedException();
}

}}
