//#define DOUBLE_PRECISION

#ifdef DOUBLE_PRECISION
#define ftype double
#define tvec2 dvec2
#define tvec3 dvec3
#define tvec4 dvec4
#else
#define ftype float
#define tvec2 vec2
#define tvec3 vec3
#define tvec4 vec4
#endif

// This is all that goes from raytracer to renderer.
// It's quite limiting since you can't have arbitrary data here,
// but it has to this way to make lookup cube possible.
// TODO: can use some code generation and automatic packing into textures to
// allow arbitrary stuff here.
struct RaytracerOutput {
  float hit; // 0 or 1
  float converged; // 0 or 1; 0 if raymarch step limit is hit
  float error; // 0 or 1; 1 if something that should never happen happened
  float iterations;
  float dist;
  vec3 normal;
  vec4 color;
};
