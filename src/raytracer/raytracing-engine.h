#pragma once

#include "i-cpu-raytracer.h"
#include "utul/camera.h"

namespace fract {

// Draws the image, using the provided callback to trace the rays.
// NOTE: Wishlist:
//  - tracing asynchronously in background thread
//    + only start tracing when viewport changes
//    + interrupt any previous tracing if viewport changes
//    + indicate progress
//    + don't start tracing when moving?
//    	* guess movement from VP matrix updates? some timeout?
//    	* or, have interface to start and stop background tracing?
//  - progressive enhancement
//    + render some minimal resolution synchronously?
//  - multithreaded tracing (CPU)
//  - reuse traced rays if viewport rotates
//    + put them on a box
//    + still throw all away when viewport moves
//    + trace invisible sides when idle?
//  - saving checkpoints to allow rewinding trajectory without re-rendering?
//  - multi-GPU tracing?
class RaytracingEngine {
public:
	// *tracer should live longer than this RaytracingEngine.
	RaytracingCamera(ICpuRaytracer *tracer);

	
private:
	ICpuRaytracer *tracer_ = nullptr;
};

}

