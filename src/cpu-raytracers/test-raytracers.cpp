#include "test-raytracers.h"
#include "util/exceptions.h"

namespace fract { namespace cpu_raytracers {

void Gradient::traceGrid(const Rays &rays, TraceResults &out_results) {
  for (size_t i = 0; i < rays.size(); ++i) {
    out_results[i] =
      (TraceResult) { 10, fabsf(rays[i].direction.Dot(fvec3(1,1,1))) };
  }
}

void Cube::traceGrid(const Rays &rays, TraceResults &out_results) {
  throw NotImplementedException();
}

}}
