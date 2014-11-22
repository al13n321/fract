#include <Common/Raytracer-Api.frag>

uniform int RaySteps = 200;
uniform float ExtrusionCoef = 0.5;
uniform float FudgeFactor = 1;
uniform float BackStep = 0.2;
uniform float NormalEps = 0.5;

uniform float GlassFreq = 200.;
uniform float GlassMag = .01;


// These should be provided in other file.
ftype DE(tvec4 p);

#ifdef TRAPS
vec4 Traps(tvec4 p);
#endif

// May override from other file.
#ifndef BOUNDS
#define BOUNDS 100.0
#endif


vec3 Normal(tvec4 p) {
  tvec2 d = tvec2(0, NormalEps * p.w);
  return vec3(normalize(tvec3(
    DE(p + d.yxxx) - DE(p - d.yxxx),
    DE(p + d.xyxx) - DE(p - d.xyxx),
    DE(p + d.xxyx) - DE(p - d.xxyx))));
}

RaytracerOutput TraceRay(tvec4 origin, vec4 direction, ftype scale) {
  RaytracerOutput res;

  res.hit = 0;
  res.converged = 0;
  res.error = 0;
  res.iterations = 0;
  res.normal = vec3(0, 0, 0);
  res.color = vec4(1e20, 1e20, 1e20, 1e20);

  tvec4 position = origin;

  ftype de = DE(position) - position.w;

  // If we're inside the body, try to escape.
  bool started_inside = de <= 0;
  bool inside = started_inside;

  // Simple hacky glass effect to indicate that camera is inside body.
  if (inside) {
    const mat3 tr = mat3(
      1,1,-1,
      -1,1,1,
      1,-1,1);
    direction.xyz += sin(tr * direction.xyz * GlassFreq) * GlassMag;
    direction.xyz = normalize(direction.xyz);
  }

  ftype dist = abs(de);
  int i;
  for (i = 0; i < RaySteps; ++i) {
    position = origin + direction * dist;
    if (dot(direction, position) > 0.0 &&
        dot(position, position) > BOUNDS) {
      // No hit (left bounding volume).
      res.converged = 1;
      res.iterations = i;
      return res;
    }

    ftype modified_eps = position.w * ExtrusionCoef;
    de = DE(position);

    ftype fudged_de;

    if (inside) {
      if (de >= 0) {
        inside = false;
        // If DE underestimates a lot, you might want to increase 8.0.
        de = max(de, modified_eps * 8.0);
        fudged_de = de * FudgeFactor;
      } else {
        // Step aggressively when inside. It doesn't matter if we overstep.
        fudged_de = -de * 1.1 + modified_eps * 3;
      }
    } else {
      fudged_de = de * FudgeFactor;
    }

    if (!inside && de <= modified_eps) {
      dist += fudged_de;
      position = origin + direction * dist;
      break;
    }

    dist += fudged_de;
    position = origin + direction * dist;
  }

  // Hit or iteration limit reached.
  res.hit = 1;
  res.converged = i < RaySteps ? 1 : 0;
  res.inside = started_inside ? 1 : 0;
  res.iterations = i;
  res.dist = float(dist * scale);

  position -= direction * (position.w * BackStep);
  res.normal = Normal(position);
#ifdef TRAPS
  res.color = Traps(position);
#else
  res.color = vec4(0, 0, 0, 0);
#endif

  return res;
}
