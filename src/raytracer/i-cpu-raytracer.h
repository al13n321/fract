#pragma once

#include "util/vec.h"

namespace fract {

// An abstract callback that does the actual tracing/marching of rays, on CPU.
// Must be thread-safe.
// NOTE: I can further add IOpenClRaytracer or ICudaRaytracer that accept
//       output buffers of corresponding type.
class ICpuRaytracer {
    struct Ray {
        vec3 start;
        vec3 direction; // unit length
    };

    struct TraceResult {
        uint16_t iterations; // 0 for no hit
        double distance; // 0 for no hit
        // NOTE: can add traps, normals, AO, shadows etc.
    };

    typedef std::vector<Ray> Rays;
    typedef std::vector<TraceResult> TraceResults;

    // Assume that time overhead on storing a vector of queries and results
    // is much less than tracing time. TODO: check
    virtual void traceGrid(const Rays &rays, TraceResults &out_results) = 0;
    virtual ~ICpuRaytracer() = 0;
};

}

