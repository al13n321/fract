#include <Common/Raytracer-Api.frag>

uniform int RaySteps = 100;
uniform float ExtrusionCoef = 0.5;
uniform float SurfaceBackStep = 0.2;
uniform float FudgeFactor = 1;
float BoundingSphere = 100; // May override from other file.

// These should be provided in other file.
ftype DE(tvec4 p);
void Surface(tvec4 p, inout RaytracerOutput res);


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
  if (de <= 0) {
    // We're inside the body.
    res.converged = 1;
    res.dist = 0;
    return res;
  }

  ftype dist = de;
  int i;
  for (i = 0; i < RaySteps; ++i) {
    position = origin + direction * dist;
    if (dot(direction, position) > 0.0 &&
        dot(position, position) > BoundingSphere) {
      // No hit (left bounding volume).
      res.converged = 1;
      res.iterations = i;
      return res;
    }

    de = DE(position) * FudgeFactor;
    ftype modified_eps = position.w * ExtrusionCoef;
    if (de <= modified_eps) {
      dist += de;
      position = origin + direction * dist;
      break;
    }

    dist += de;
    position = origin + direction * dist;
  }

  // Hit or iteration limit reached.
  res.hit = 1;
  res.converged = i < RaySteps ? 1 : 0;
  res.iterations = i;
  res.dist = float(dist * scale);

  Surface(position - direction * (position.w * SurfaceBackStep), res);

  return res;
}
