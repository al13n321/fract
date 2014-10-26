// To create a raytracer you need to include this file and implement TraceRay function.

#include <Common/Structures.frag>

// w is epsilon
RaytracerOutput TraceRay(vec4 p, vec4 d, float scale);

// Set from application to one of predefined files. It defines main function.
#include [Camera-shader]
