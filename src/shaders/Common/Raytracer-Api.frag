// To create a raytracer you need to include this file and implement TraceRay function.

#include <Common/Common.frag>

// w is epsilon
RaytracerOutput TraceRay(tvec4 p, vec4 d, ftype scale);

// Set from application to one of predefined files. It defines main function.
#include [Camera-shader]
