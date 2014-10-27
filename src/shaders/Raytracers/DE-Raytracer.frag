#include <Common/Raytracer-Api.frag>

uniform int RaySteps = 100;
uniform float ExtrusionCoef = 0.2;
uniform float SurfaceBackStep = 0.2;
uniform float FudgeFactor = 1;
float BoundingSphere = 100; // May override from other file.

// These should be provided in other file.
float DE(vec4 p);
void Surface(vec4 p, inout RaytracerOutput res);


RaytracerOutput TraceRay(vec4 origin, vec4 direction, float scale) {
  RaytracerOutput res;

  res.hit = 0;
  res.converged = 0;
  res.error = 0;
  res.dist = 0;
  res.iterations = 0;
  res.normal = vec3(0, 0, 0);
  res.color = vec4(1e20, 1e20, 1e20, 1e20);

  vec4 position = origin;

  float de = DE(position) - position.w;
  if (de <= 0) {
    // We're inside the body.
    res.converged = 1;
    return res;
  }

  int i;
  for (i = 0; i < RaySteps; ++i) {
    if (dot(direction, position) > 0.0 &&
        dot(position, position) > BoundingSphere) {
      // No hit (left bounding volume).
      res.converged = 1;
      res.iterations = i;
      return res;
    }

    float de = DE(position) * FudgeFactor;
    float modified_eps = position.w * ExtrusionCoef;
    if (de <= modified_eps) {
      res.dist += de;
      position += direction * de;
      break;
    }

    res.dist += de;
    position += direction * de;
  }

  // Hit or iteration limit reached.
  res.hit = 1;
  res.converged = i < RaySteps ? 1 : 0;
  res.iterations = i;

  Surface(position - direction * (position.w * SurfaceBackStep), res);

  return res;
}
