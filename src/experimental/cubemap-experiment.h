#pragma once

#include <vector>
#include <cassert>
#include "util/vec.h"
#include "util/mat.h"
#include "util/stopwatch.h"
#include "raytracer/raytraced-view.h"
#include "raytracer/ray-grid.h"

namespace fract {

static inline int sign(float x) {
  return x > 0 ? 1 : -1;
}

static inline float PositiveDiv(float a, float b) {
  return a * b > 0 ? a / b : 1e30;
}

// Test if lookup cubemap is a good idea.
// Just a CPU implementation of mandelbox with cubemap lookup.
// Should be slow as hell but give some estimate of performance benefits
// and visual artefacts of using lookup cube.
// Mandelbox code mostly copied from Fragmentarium samples.
class CubemapExperiment {
 public:
  struct PerformanceCounters {
    #define FOR_EACH_FIELD(macro) \
      macro(uint64_t, traced_rays) \
      macro(uint64_t, trace_steps) \
      macro(uint64_t, trace_step_limits) \
      macro(uint64_t, trace_misses) \
      macro(uint64_t, de_calls) \
      macro(uint64_t, de_step_limits) \
      macro(uint64_t, iterations) \
      macro(double, rays_distance) \
      macro(double, time)

    #define DECLARE(type, name) type name = 0;
    FOR_EACH_FIELD(DECLARE)
    #undef DECLARE

    void Clear() {
      *this = PerformanceCounters();
    }

    void Log() {
      #define PRINT(type, name) std::cerr << #name << ": " << name << '\n';
      FOR_EACH_FIELD(PRINT)
      #undef PRINT
      std::cerr << std::endl;
    }
    #undef FOR_EACH_FIELD
  };

private:
  struct Image {
    int width, height;
    std::vector<float> main_buf;
    std::vector<float> normal_buf;
    std::vector<float> color_buf;

    // A little different from dist channel of main_buf.
    // Depth is projected distance.
    std::vector<float> depth_buf;

    void Resize(int wid, int hei) {
      width = wid;
      height = hei;
      main_buf.resize(width * height * 4);
      normal_buf.resize(width * height * 4);
      color_buf.resize(width * height * 4);
      depth_buf.resize(width * height);
    }
  };

  Image image_;
  RaytracedView view_;

  Image cube_[6];
  fmat4 face_rotations_[6];
  fmat4 cube_projection_;
  int cube_resolution_;

  fvec3 cube_center_;

  inline float DE(fvec3 p0, fvec3 &traps, PerformanceCounters &counters) {
    ++counters.de_calls;

    fvec3 p = p0;
    float w = 1;
    traps = fvec3(1e5, 1e5, 1e5);

    const int It = 10;
    int i;
    for (i=0; i<It; i++) {
      ++counters.iterations;
      p = p.Clamp(-1.0f, 1.0f) * 2.0f - p;  // min;max;mad
      float r2 = p.LengthSquare();
      if (i<3) traps = traps.Min(p.Abs());
      float t = std::min(std::max(.25f/r2, .25f), 1.0f);  // dp3,div,max.sat,mul
      p *= t;
      w *= t;
      p = p*12.0f + p0;
      w = w*12.0f + 1;
      if (r2>1000.0) break;
    }
    if (i == It)
      ++counters.de_step_limits;
    return ((p.Length() - 2.0f) / w - pow(3.0f, 1 - It));
  }

  inline void TraceRay(
    fvec3 origin, fvec3 direction, float pixel_size_coefficient,
    float initial_dist, int initial_iters,
    float *out_main, float *out_normal, float *out_color,
    PerformanceCounters &counters
  ) {
    ++counters.traced_rays;

    float dist = initial_dist;
    fvec3 position = origin + direction * dist;
    float pixel_size = dist * pixel_size_coefficient;
    int i;
    fvec3 traps;
    const float extrusion_coef = 1;
    const int step_limit = 500;
    for (i = std::min(step_limit - 1, initial_iters); i < step_limit; ++i) {
      ++counters.trace_steps;
      if ((position.x * direction.x > 0 && std::abs(position.x) > 1000) ||
          (position.y * direction.y > 0 && std::abs(position.y) > 1000) ||
          (position.z * direction.z > 0 && std::abs(position.z) > 1000)) {
        // No hit (left bounding volume).
        out_main[0] = 0;
        out_main[1] = i;
        out_main[2] = dist;
        out_main[3] = 0;
        ++counters.trace_misses;
        return;
      }

      float de = DE(position, traps, counters) - pixel_size * extrusion_coef;
      if (fabsf(de) <= pixel_size * .5f || // .5f is arbitrary.
          (de <= 0 && i == 0))
        break;

      dist += de;
      counters.rays_distance += de;
      position += direction * de;
      pixel_size += pixel_size_coefficient * de;
    }

    // Hit or iteration limit reached.
    out_main[0] = 1;
    out_main[1] = i;
    out_main[2] = dist;
    out_main[3] = i == step_limit;

    if (i == step_limit)
      ++counters.trace_step_limits;

    out_color[0] = traps.x;
    out_color[1] = traps.y;
    out_color[2] = traps.z;
    
    position -= direction * pixel_size * 2.f;
    float t = pixel_size * .5f;
    fvec3 n = fvec3(
      DE(position + fvec3(t,0,0), traps, counters) -
      DE(position - fvec3(t,0,0), traps, counters),
      DE(position + fvec3(0,t,0), traps, counters) -
      DE(position - fvec3(0,t,0), traps, counters),
      DE(position + fvec3(0,0,t), traps, counters) -
      DE(position - fvec3(0,0,t), traps, counters));
    n.NormalizeMe();
    out_normal[0] = n.x;
    out_normal[1] = n.y;
    out_normal[2] = n.z;
  }

  // Infinite pyramid with origin at zero, 90 degrees FOV and
  // going in negative Z direction. In other words: z <= -max(|x|, |y|).
  std::pair<float, float> RayFrustumIntersection(fvec3 p, fvec3 d) {
    std::pair<float, float> res(0, 1e20);
    
    // Intersecting 4 half-spaces. This code sucks.

    if (d.z > d.x) {
      // A numeric-stability-paranoid way to do
      //  res.second = min(res.second, (p.x - p.z) / (d.z - d.x)).
      if (p.x - p.z < res.second * (d.z - d.x))
        res.second = (p.x - p.z) / (d.z - d.x);
    } else {
      if (p.x - p.z < res.first * (d.z - d.x))
        res.first = (p.x - p.z) / (d.z - d.x);
    }

    if (d.z > -d.x) {
      if (-p.x - p.z < res.second * (d.z + d.x))
        res.second = (-p.x - p.z) / (d.z + d.x);
    } else {
      if (-p.x - p.z < res.first * (d.z + d.x))
        res.first = (-p.x - p.z) / (d.z + d.x);
    }

    if (d.z > d.y) {
      if (p.y - p.z < res.second * (d.z - d.y))
        res.second = (p.y - p.z) / (d.z - d.y);
    } else {
      if (p.y - p.z < res.first * (d.z - d.y))
        res.first = (p.y - p.z) / (d.z - d.y);
    }

    if (d.z > -d.y) {
      if (-p.y - p.z < res.second * (d.z + d.y))
        res.second = (-p.y - p.z) / (d.z + d.y);
    } else {
      if (-p.y - p.z < res.first * (d.z + d.y))
        res.first = (-p.y - p.z) / (d.z + d.y);
    }

    return res;
  }

  inline void TraceRayThroughCubeFace(
    const Image &face_image,
    fvec3 origin, fvec3 direction, std::pair<float, float> range,
    float *out_main, float *out_normal, float *out_color,
    bool &out_hit, bool &out_good_hit,
    PerformanceCounters &counters
  ) {
    float dist = range.first;
    fvec3 p = origin + direction * dist;

    if (p.z > -1e-5) {
      int x = static_cast<int>((direction.x / -direction.z + 1.f) / 2.f
        * cube_resolution_);
      int y = static_cast<int>((direction.y / -direction.z + 1.f) / 2.f
        * cube_resolution_);
      int index = y * cube_resolution_ + x;
      if (x >= 0 && x < cube_resolution_ &&
          y >= 0 && y < cube_resolution_ &&
          face_image.main_buf[index * 4 + 0] > .5f) {
        out_hit = true;
        out_good_hit = true;
        out_main[0] = 1;
        out_main[1] = face_image.main_buf[index * 4 + 1];
        out_main[2] = face_image.main_buf[index * 4 + 2];
        out_main[3] = face_image.main_buf[index * 4 + 3];
        out_normal[0] = face_image.normal_buf[index * 4 + 0];
        out_normal[1] = face_image.normal_buf[index * 4 + 1];
        out_normal[2] = face_image.normal_buf[index * 4 + 2];
        out_color[0] = face_image.color_buf[index * 4 + 0];
        out_color[1] = face_image.color_buf[index * 4 + 1];
        out_color[2] = face_image.color_buf[index * 4 + 2];
      } else
        out_hit = false;
      return;
    }

    // This code sucks.

    int x = static_cast<int>((p.x / -p.z + 1.f) / 2.f * cube_resolution_);
    x = std::max(0, std::min(cube_resolution_ - 1, x));
    int y = static_cast<int>((p.y / -p.z + 1.f) / 2.f * cube_resolution_);
    y = std::max(0, std::min(cube_resolution_ - 1, y));
    int dx = sign(p.x * direction.z - p.z * direction.x);
    int dy = sign(p.y * direction.z - p.z * direction.y);
    while (true) {
      int index = y * cube_resolution_ + x;
      float z = face_image.depth_buf[index];
      if (z > 0 && -p.z > z) {
        // We hit the side of pixel (x, y).
        out_hit = true;
        out_good_hit = false;
        out_main[0] = face_image.main_buf[index * 4 + 0];
        out_main[1] = face_image.main_buf[index * 4 + 1];
        out_main[2] = dist;
        out_main[3] = face_image.main_buf[index * 4 + 3];
        out_normal[0] = face_image.normal_buf[index * 4 + 0];
        out_normal[1] = face_image.normal_buf[index * 4 + 1];
        out_normal[2] = face_image.normal_buf[index * 4 + 2];
        out_color[0] = face_image.color_buf[index * 4 + 0];
        out_color[1] = face_image.color_buf[index * 4 + 1];
        out_color[2] = face_image.color_buf[index * 4 + 2];
        return;
      }

      float k;

      k = (x + (dx+1)/2) * 2.f / cube_resolution_ - 1.f;
      float tx = PositiveDiv(-k*p.z-p.x, direction.x + k*direction.z);

      k = (y + (dy+1)/2) * 2.f / cube_resolution_ - 1.f;
      float ty = PositiveDiv(-k*p.z-p.y, direction.y + k*direction.z);

      float tz = z < 0 ? 1e30 : PositiveDiv(-z - p.z, direction.z);

      if (tx > 1e20 && ty > 1e20 && tz > 1e20)
        break;

      if (tz < tx && tz < ty) {
        // We hit the front of pixel (x, y).
        dist += tz;
        out_hit = true;
        out_good_hit = true;
        out_main[0] = face_image.main_buf[index * 4 + 0];
        out_main[1] = face_image.main_buf[index * 4 + 1];
        out_main[2] = dist;
        out_main[3] = face_image.main_buf[index * 4 + 3];
        out_normal[0] = face_image.normal_buf[index * 4 + 0];
        out_normal[1] = face_image.normal_buf[index * 4 + 1];
        out_normal[2] = face_image.normal_buf[index * 4 + 2];
        out_color[0] = face_image.color_buf[index * 4 + 0];
        out_color[1] = face_image.color_buf[index * 4 + 1];
        out_color[2] = face_image.color_buf[index * 4 + 2];
        return;
      }

      if (tx > 0 && (ty < 0 || tx < ty)) {
        dist += tx;
        x += dx;
        if (x < 0 || x >= cube_resolution_)
          break;
      } else {
        dist += ty;
        y += dy;
        if (y < 0 || y >= cube_resolution_)
          break;
      }

      p = origin + direction * dist;
    }

    out_hit = false;
    out_main[0] = 0;
    out_main[1] = 0;
    out_main[2] = 0;
    out_main[3] = 0;
  }

  inline void TraceRayUsingCube(
    fvec3 origin, fvec3 direction, float pixel_size_coefficient,
    bool fallback_to_real_tracing,
    float *out_main, float *out_normal, float *out_color,
    PerformanceCounters &counters
  ) {
    struct RayOnFace {
      std::pair<float, float> intersection;
      fvec3 origin;
      fvec3 direction;
      int face_index;

      bool operator<(const RayOnFace &rhs) const {
        return intersection.first < rhs.intersection.first;
      }
    };

    RayOnFace face_candidates[6];
    int candidate_count = 0;
    for (int i = 0; i < 1; ++i) {
      RayOnFace &candidate = face_candidates[candidate_count];
      candidate.face_index = i;
      candidate.origin = face_rotations_[i].Transform(origin - cube_center_);
      candidate.direction = face_rotations_[i].Transform(direction);
      candidate.intersection = RayFrustumIntersection(
        candidate.origin, candidate.direction);
      if (candidate.intersection.first < candidate.intersection.second)
        ++candidate_count;
    }
    std::sort(face_candidates, face_candidates + candidate_count);
    for (int i = 0; i < candidate_count; ++i) {
      RayOnFace &candidate = face_candidates[i];
      bool hit, good_hit;
      TraceRayThroughCubeFace(
        cube_[candidate.face_index],
        candidate.origin,
        candidate.direction,
        candidate.intersection,
        out_main,
        out_normal,
        out_color,
        hit, good_hit,
        counters);
      if (hit) {
        if (good_hit)
          return;
        if (!fallback_to_real_tracing) {
          out_main[0] = 0;
          out_main[1] = 0;
          out_main[2] = 0;
          out_main[3] = 1;
          return;
        }
        TraceRay(
          origin, direction, pixel_size_coefficient,
          out_main[2], static_cast<int>(out_main[1]),
          out_main,
          out_normal,
          out_color,
          counters);
        return;
      }
    }
    out_main[0] = 0;
    out_main[1] = 0;
    out_main[2] = 0;
    out_main[3] = 0;
  }
 public:
  CubemapExperiment(int width, int height): view_(width, height) {
    image_.Resize(width, height);

    cube_resolution_ = std::max(width, height) * 3;
    cube_projection_ = fmat4::PerspectiveProjectionMatrix(90, 1, 1, 0);
    face_rotations_[0] = fmat4::IdentityMatrix();
    face_rotations_[1] = fmat4::RotationMatrixZ(180);
    face_rotations_[2] = fmat4::RotationMatrixX(90);
    face_rotations_[3] = fmat4::RotationMatrixX(-90);
    face_rotations_[4] = fmat4::RotationMatrixY(90);
    face_rotations_[5] = fmat4::RotationMatrixY(-90);

    for (int i = 0; i < 6; ++i) {
      cube_[i].Resize(cube_resolution_, cube_resolution_);
    }
  }

  void PrecalcCube(fvec3 position, PerformanceCounters &counters) {
    Stopwatch stopwatch;

    cube_center_ = position;
    for (int face_index = 0; face_index < 6; ++face_index) {
      RayGrid grid;
      grid.position = cube_center_;
      grid.rotation_projection_inv =
        (cube_projection_ * face_rotations_[face_index]).Inverse();
      grid.scale = 1;
      grid.resolution_width = cube_resolution_;
      grid.resolution_height = cube_resolution_;
      grid.min_x = 0;
      grid.min_y = 0;
      grid.size_x = cube_resolution_;
      grid.size_y = cube_resolution_;

      Image &face_image = cube_[face_index];
      fvec3 face_direction = face_rotations_[face_index].Inverse()
        .Transform(fvec3(0,0,-1));

      for (int y = 0; y < cube_resolution_; ++y) {
        for (int x = 0; x < cube_resolution_; ++x) {
          fvec3 dir = grid.GetRayDirection(x, y);
          float pix_size_coef = grid.GetPixelSizeCoefficient(x, y);

          int index = y * cube_resolution_ + x;
          TraceRay(
            cube_center_, dir, pix_size_coef, 0, 0,
            &face_image.main_buf[index * 4],
            &face_image.normal_buf[index * 4],
            &face_image.color_buf[index * 4],
            counters);
          float dist_coef = dir.Dot(face_direction);
          bool hit = face_image.main_buf[index * 4 + 0] > .5f;
          float dist = face_image.main_buf[index * 4 + 2];
          face_image.depth_buf[index] = hit ? dist * dist_coef : -1;
        }
      }
    }

    counters.time += stopwatch.TimeSinceRestart();
  }

  const RaytracedView& Raytrace(
    fvec3 position, fmat4 rotation_projection,
    bool use_cube, bool fallback_to_real_tracing,
    PerformanceCounters &counters
  ) {
    Stopwatch stopwatch;

    RayGrid grid;
    grid.position = position;
    grid.rotation_projection_inv = rotation_projection.Inverse();
    grid.scale = 1;
    grid.resolution_width = image_.width;
    grid.resolution_height = image_.height;
    grid.min_x = 0;
    grid.min_y = 0;
    grid.size_x = image_.width;
    grid.size_y = image_.height;

    for (int y = 0; y < image_.height; ++y) {
      for (int x = 0; x < image_.width; ++x) {
        fvec3 dir = grid.GetRayDirection(x, y);
        float pix_size_coef = grid.GetPixelSizeCoefficient(x, y);

        int index = y * image_.width * 4 + x * 4;
        if (use_cube)
          TraceRayUsingCube(
            position, dir, pix_size_coef,
            fallback_to_real_tracing,
            &image_.main_buf[index],
            &image_.normal_buf[index],
            &image_.color_buf[index],
            counters);
        else
          TraceRay(
            position, dir, pix_size_coef, 0, 0,
            &image_.main_buf[index],
            &image_.normal_buf[index],
            &image_.color_buf[index],
            counters);
      }
    }

    counters.time += stopwatch.TimeSinceRestart();

    view_.main_texture.SetPixels(GL_RGBA, GL_FLOAT, &image_.main_buf[0]);
    view_.normal_texture.SetPixels(GL_RGBA, GL_FLOAT, &image_.normal_buf[0]);
    view_.color_texture.SetPixels(GL_RGBA, GL_FLOAT, &image_.color_buf[0]);
    return view_;
  }
};

}
