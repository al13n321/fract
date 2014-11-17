#pragma once

#include "util/mat.h"
#include "util/quat.h"
#include "util/json.h"
#include "config.h"

namespace fract {

// World coordinates: x right, y up, z back
// Screen coordinates: x right, y up, z forward, -1 <= z <= 1
// Translation and scaling are separated from rotation-projection matrix
// because they need double precision.
// FOV ans aspect ratio are ignored for VR rendering.
class Camera {
 public:
  enum Mode {
    YAW_PITCH, // Roll is locked to 0.
    SPACE,     // No up and down, like you're in space.
  };

  Camera(ConfigPtr config);

  inline dvec3 position() const { return position_; }
  // world magnification factor
  inline double scale() const { return scale_; }

  inline void set_position(dvec3 a) { position_ = a; }
  inline void set_scale(double a) { scale_ = a; }

  // These are unused for VR.

  // horizontal FOV in degrees
  inline float field_of_view() const { return fov_; }
  // width / height
  inline float aspect_ratio() const { return aspect_ratio_; }

  inline void set_field_of_view(float a) { fov_ = a; }
  inline void set_aspect_ratio(float a) { aspect_ratio_ = a; }

  // Usually bound to mouse movement.
  void TurnRelative(fvec2 delta);

  // delta is the camera shift relative to camera rotation
  void MoveRelative(fvec3 delta);

  // Usually bound to mouse wheel.
  void ScaleRelative(double factor);

  // Revert to values from config.
  void Reset();

  fquat Rotation() const;

  // Near clip plane is set arbitrarily and should be ignored.
  // To transform from world to camera coordinates:
  // y = RotationProjectionMatrix() * ((x - position()) * scale())
  fmat4 RotationProjectionMatrix() const;

 private:
  Mode mode_;

  dvec3 position_ {0, 0, 0};
  double scale_ = 1;

  float fov_ = 60;
  float near_clip_plane_ = 1e-3f;
  float aspect_ratio_ = 4.f / 3.f;

  // Unused in SPACE mode.
  float yaw_ = 0;
  float pitch_ = 0;

  // Used only in SPACE mode.
  fquat quat_{1, 0, 0, 0};

  float move_speed_ = 1;
  float turn_speed_ = 1;
  float scale_speed_ = 1;

  ConfigPtr config_;

  Config::SubscriptionPtr position_subscription_;
  Config::SubscriptionPtr scale_subscription_;
  Config::SubscriptionPtr rotation_subscription_;
  Config::SubscriptionPtr passive_subscription_;

  void ResetPosition();
  void ResetScale();
  void ResetRotation();
  void ResetPassive();
};

}
