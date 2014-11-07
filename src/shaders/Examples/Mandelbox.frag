#version 410
#include <Raytracers/DE-Raytracer.frag>

// Modified code from Fragmentarium examples.

uniform int Iterations = 10;
uniform mat3 Rotation = mat3(1,0,0,0,1,0,0,0,1);
uniform float MinRad2 = 0.25;
uniform float Scale = 3.0;

vec4 scale = vec4(Scale, Scale, Scale, abs(Scale)) / MinRad2;
float absScalem1 = abs(Scale - 1.0);
ftype absScaleRaisedTo1mIters = pow(abs(Scale), 1-Iterations);

uniform float NormalDelta = 0.5;

ftype DE(tvec4 pos) {
  tvec4 p = tvec4(pos.xyz,1), p0 = p;  // p.w is the distance estimate

  for (int i=0; i<Iterations; i++) {
    p.xyz*=Rotation;
    p.xyz = clamp(p.xyz, -1.0, 1.0) * 2.0 - p.xyz;  // min;max;mad
    ftype r2 = dot(p.xyz, p.xyz);
    p *= clamp(max(MinRad2/r2, MinRad2), 0.0, 1.0);  // dp3,div,max.sat,mul
    p = p*scale + p0;
    if (r2>1000.0) break;
  }
  return ((length(p.xyz) - absScalem1) / p.w - absScaleRaisedTo1mIters);
}

void Surface(tvec4 p, inout RaytracerOutput res) {
  tvec2 d = tvec2(0, NormalDelta * p.w);
  res.normal = vec3(normalize(tvec3(
    DE(p + d.yxxx) - DE(p - d.yxxx),
    DE(p + d.xyxx) - DE(p - d.xyxx),
    DE(p + d.xxyx) - DE(p - d.xxyx))));
  res.color = vec4(0,0,0,0);
}
