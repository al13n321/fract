struct RaytracingOutput {
  float hit; // 0 or 1
  float converged; // 0 or 1; 0 if raymarch step limit is hit
  float error; // 0 or 1; 1 if something that should never happen happened
  float dist;
  vec3 normal;
  vec4 color;
};
