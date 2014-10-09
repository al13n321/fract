#pragma once

// Have to include glew before OpenCL on some platforms.
#include "gl-util/gl-common.h"

#ifdef WIN32
#include <CL/opencl.h>
#endif

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#endif
