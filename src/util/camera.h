#pragma once

#include "mat.h"
#include "json.h"

namespace fract {

// World coordinates: x right, y up, z back
// Screen coordinates: x right, y up, z forward, -1 <= z <= 1
// Translation and scaling are separated from rotation-projection matrix
// because they need double precision.
// FOV ans aspect ratio are ignored for VR rendering.
class Camera {
 public:
  Camera() = default;
  Camera(const Camera &c) = default;
  Camera& operator = (const Camera &c) = default;

  ~Camera();

  inline float yaw() const { return yaw_; }
  inline float pitch() const { return pitch_; }
  inline float roll() const { return roll_; }
  inline dvec3 position() const { return position_; }
  // horizontal FOV in degrees
  inline float field_of_view() const { return fov_; }
  // width / height
  inline float aspect_ratio() const { return aspect_ratio_; }
  inline float near_clip_plane() const { return near_clip_plane_; }
  // world magnification factor
  inline double scale() const { return scale_; }

  inline void set_yaw(float a) { yaw_ = a; }
  inline void set_pitch(float a) { pitch_ = a; }
  inline void set_roll(float a) { roll_ = a; }
  inline void set_position(dvec3 a) { position_ = a; }
  inline void set_field_of_view(float a) { fov_ = a; }
  inline void set_aspect_ratio(float a) { aspect_ratio_ = a; }
  inline void set_near_clip_plane(float a) { near_clip_plane_ = a; }
  inline void set_scale(double a) { scale_ = a; }

  // delta is the camera shift relative to camera rotation
  void MoveRelative(fvec3 delta);

  fmat4 RotationMatrix() const;
  
  // Near clip plane for projection is set arbitrarily and should be ignored.
  // To transform from world to camera coordinates:
  // y = RotationProjectionMatrix() * ((x - position()) * scale())
  fmat4 RotationProjectionMatrix() const;

  // On error logs and keeps current state.
  void FromJson(const Json::Value &value);
 private:
  float yaw_ = 0;
  float pitch_ = 0;
  float roll_ = 0;
  dvec3 position_ {0, 0, 0};
  float fov_ = 60;
  float near_clip_plane_ = 1e-3f;
  float aspect_ratio_ = 4.f / 3.f;
  double scale_ = 1;
};

}
