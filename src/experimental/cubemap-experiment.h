#pragma once

#include <vector>
#include "util/vec.h"
#include "util/mat.h"
#include "raytracer/raytraced-view.h"
#include "raytracer/ray-grid.h"

namespace fract {

// Test if lookup cubemap is a good idea.
// Mandelbox code mostly copied from Fragmentarium samples.
class CubemapExperiment {
 private:
  struct Image {
    int width, height;
    std::vector<float> main_buf;
    std::vector<float> normal_buf;
    std::vector<float> color_buf;

    void Resize(int wid, int hei) {
      width = wid;
      height = hei;
      main_buf.resize(width * height * 4);
      normal_buf.resize(width * height * 4);
      color_buf.resize(width * height * 4);
    }
  };

  inline float DE(fvec3 p0, fvec3 &traps) {
    fvec3 p = p0;
    float w = 1;
    traps = fvec3(1e5, 1e5, 1e5);

    const int It = 100;
    for (int i=0; i<It; i++) {
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
    return ((p.Length() - 2.0f) / w - pow(3.0f, 1 - It));
  }

  inline void TraceRay(
    fvec3 origin, fvec3 direction, float pixel_size_coefficient,
    float *out_main, float *out_normal, float *out_color
  ) {
    fvec3 position = origin;
    float pixel_size = 0;
    float dist = 0;
    int i;
    fvec3 traps;
    const int step_limit = 100;
    for (i = 0; i < step_limit; ++i) {
      if ((position.x * direction.x > 0 && position.x > 10) ||
          (position.y * direction.y > 0 && position.y > 10) ||
          (position.z * direction.z > 0 && position.z > 10)) {
        // No hit (left bounding volume).
        out_main[0] = 0;
        out_main[1] = i;
        out_main[2] = dist;
        out_main[3] = 0;
        return;
      }

      float de = DE(position, traps) + pixel_size;
      if (fabsf(de) <= pixel_size * .5f ||
          (de <= 0 && i == 0)) // .5f is arbitrary.
        break;

      dist += de;
      position += direction * de;
      pixel_size += pixel_size_coefficient * de;
    }

    // Hit or iteration limit reached.
    out_main[0] = 1;
    out_main[1] = i;
    out_main[2] = dist;
    out_main[3] = i == step_limit ? 1000 : 0;

    out_color[0] = traps.x;
    out_color[1] = traps.y;
    out_color[2] = traps.z;
    
    position -= direction * pixel_size * 2.f;
    float t = pixel_size * .5f;
    fvec3 n = fvec3(
      DE(position + fvec3(t,0,0), traps) -
      DE(position - fvec3(t,0,0), traps),
      DE(position + fvec3(0,t,0), traps) -
      DE(position - fvec3(0,t,0), traps),
      DE(position + fvec3(0,0,t), traps) -
      DE(position - fvec3(0,0,t), traps));
    n.NormalizeMe();
    out_normal[0] = n.x;
    out_normal[1] = n.y;
    out_normal[2] = n.z;
  }

  Image image_;
  Image cube_[6];
  RaytracedView view_;

  fvec3 cube_center_;
 public:
  CubemapExperiment(int width, int height): view_(width, height) {
    image_.Resize(width, height);
    for (int i = 0; i < 6; ++i) {
      cube_[i].Resize(width, height);
    }
  }

  void PrecalcCube(fvec3 position) {
    cube_center_ = position;
    // ...
  }

  const RaytracedView& Raytrace(fvec3 position, fmat4 rotation_projection) {
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
        TraceRay(
          position, dir, pix_size_coef,
          &image_.main_buf[index],
          &image_.normal_buf[index],
          &image_.color_buf[index]);
      }
    }

    view_.main_texture.SetPixels(GL_RGBA, GL_FLOAT, &image_.main_buf[0]);
    view_.normal_texture.SetPixels(GL_RGBA, GL_FLOAT, &image_.normal_buf[0]);
    view_.color_texture.SetPixels(GL_RGBA, GL_FLOAT, &image_.color_buf[0]);
    return view_;
  }
};

}
