#include "experimental-cpu-raytracer.h"
#include <iostream>

namespace fract {

static float DE(fvec3 p) {
  const size_t Iterations = 15;
  const float MinRad2 = 0.25;
  const float Scale = 3;
  const float scale = Scale / MinRad2;
  const float absScalem1 = std::abs(Scale - 1.0f);
  const float absScaleRaisedTo1mIters = pow(std::abs(Scale), 1.f-Iterations);

  fvec3 p0 = p;
  float w = 1;

  for (int i=0; i<Iterations; i++) {
    p = p.Clamp(-1.0, 1.0) * 2.0 - p;  // min;max;mad
    float r2 = p.LengthSquare();
    float t = std::min(1.f, std::max(MinRad2/r2, MinRad2));  // dp3,div,max.sat,mul
    p *= t;
    w *= t;
    p = p*scale + p0;
    w = w * scale + 1;
    if (r2>1e3) break;
  }
  return ((p.Length() - absScalem1) / w - absScaleRaisedTo1mIters);
}

static void trace(fvec3 origin, fvec3 direction, float pix_size_coef, float d0,
                  size_t step_limit,
                  bool *out_hit, size_t *out_steps, float *out_dist) {
  fvec3 position = origin;
  float w;
  float de;

  // If iteration limit is exceeded, take "best" point of the trajectory instead of the last.
  float best_dist = 0;
  float best_rel_de = 1e10;

  float dist = d0;
  size_t i;
  for (i = 0; i < step_limit; ++i) {
    position = origin + direction * dist;
    w = pix_size_coef * dist;
    if (direction.Dot(position) > 0.0 &&
        position.LengthSquare() > 100) {
      // No hit (left bounding volume).
      *out_steps = i;
      *out_hit = false;
      *out_dist = 1e30f;
      return;
    }

    float modified_eps = w * 0.5f;
    de = DE(position);

    if (de <= modified_eps) {
      dist += de;
      best_dist = dist;
      break;
    }

    float rel_de = de / w;
    if (rel_de < best_rel_de) {
      best_rel_de = rel_de;
      best_dist = dist;
    }

    dist += de;
    position = origin + direction * dist;
  }

  position = origin + direction * best_dist;

  // Hit or iteration limit reached.
  *out_hit = true;
  *out_steps = i;
  *out_dist = dist;
}

void ExperimentalCPURaytracer::TraceGrid(
    const RayGrid &grid, RaytracedView &target) {
  if (done_) {
    return;
  }
  done_ = true;

  size_t len = grid.resolution.x * grid.resolution.y * 4;
  std::vector<float> main(len);
  std::vector<float> color(len);
  std::vector<float> normal(len);

  const size_t B = 4;
  size_t total_steps = 0;
  double total_dist = 0;
  size_t cnt = 0;
  double total_dist2 = 0;
  double total_rem_dist = 0;
  double total_rem_dist_pct = 0;
  size_t total_rem_steps = 0;
  for (size_t y1 = 0; y1 < grid.resolution.y; y1 += B) {
    for (size_t x1 = 0; x1 < grid.resolution.x; x1 += B) {
      size_t x2 = std::min(x1 + B, (size_t)grid.resolution.x);
      size_t y2 = std::min(y1 + B, (size_t)grid.resolution.y);
      float mnd = 1e30f;
      for (size_t y = y1; y < y2; ++y) {
        for (size_t x = x1; x < x2; ++x) {
          fvec3 dir = grid.GetRayDirection((int)x, (int)y);
          float pix_size_coef = grid.GetPixelSizeCoefficient((int)x, (int)y);

          bool hit;
          size_t steps;
          float dist;
          trace(grid.position, dir, pix_size_coef, 0, 200, &hit, &steps, &dist);
          total_steps += steps;
          total_dist += dist;
          mnd = std::min(mnd, dist);
          ++cnt;
        }
      }

      for (size_t y = y1; y < y2; ++y) {
        for (size_t x = x1; x < x2; ++x) {
          fvec3 dir = grid.GetRayDirection((int)x, (int)y);
          float pix_size_coef = grid.GetPixelSizeCoefficient((int)x, (int)y);

          bool hit;
          size_t steps;
          float dist;
          trace(grid.position, dir, pix_size_coef, mnd, 5, &hit, &steps, &dist);
          total_rem_steps += steps;
          total_dist2 += dist;
          total_rem_dist += dist - mnd;
          total_rem_dist_pct += (dist - mnd) / dist;

          size_t ind = (y * grid.resolution.x + x) * 4;
          main[ind + 0] = 4.f + (hit ? 2.f : 0.f);
          normal[ind + 3] = std::max(0.f, 1.f - steps / 50.f);
        }
      }
    }
  }

  std::cerr << "pixels: " << cnt << std::endl;
  std::cerr << "avg steps: " << total_steps * 1. / cnt << std::endl;
  std::cerr << "avg dist: " << total_dist / cnt << std::endl;
  std::cerr << "avg dist 2: " << total_dist2 / cnt << std::endl;
  std::cerr << "(avg rem dist)%: " << total_rem_dist / total_dist2 << std::endl;
  std::cerr << "avg (rem dist%): " << total_rem_dist_pct / cnt << std::endl;
  std::cerr << "avg rem steps: " << total_rem_steps * 1. / cnt << std::endl;
  std::cerr << "(avg rem steps)%: " << total_rem_steps * 1. / total_steps
            << std::endl;
  std::cerr << std::endl;

  target.main_texture.SetPixels(GL_RGBA, GL_FLOAT, &main[0]);
  target.color_texture.SetPixels(GL_RGBA, GL_FLOAT, &color[0]);
  target.normal_texture.SetPixels(GL_RGBA, GL_FLOAT, &normal[0]);
}

}

