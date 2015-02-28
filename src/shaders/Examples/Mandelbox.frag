#version 410
#include <Raytracers/DE-Raytracer.frag>

// Modified code from Fragmentarium examples.

uniform int Iterations = 20;
uniform float MinRad2 = 0.25;
uniform float Scale = 3.0;

float scale = Scale / MinRad2;
float absScalem1 = abs(Scale - 1.0);
ftype absScaleRaisedTo1mIters = pow(abs(Scale), 1-Iterations);

ftype DE(tvec4 pos) {
  tvec4 p = tvec4(pos.xyz,1), p0 = p;  // p.w is the distance estimate

  for (int i=0; i<Iterations; i++) {
    p.xyz = clamp(p.xyz, -1.0, 1.0) * 2.0 - p.xyz;  // min;max;mad
    ftype r2 = dot(p.xyz, p.xyz);
    p *= clamp(max(MinRad2/r2, MinRad2), 0.0, 1.0);  // dp3,div,max.sat,mul
    p = p*scale + p0;
    if (r2>1000.0) break;
  }
  return ((length(p.xyz) - absScalem1) / p.w - absScaleRaisedTo1mIters);
}
