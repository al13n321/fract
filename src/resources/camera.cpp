#include "camera.h"
#include "util/exceptions.h"

namespace fract {

const static float kPI = 3.1415926535897932384626433832795f;
const static float kDegToRad = kPI / 180.0f;

Camera::Camera(Config::View *config): config_(config) {
  position_subscription_ = config_->Subscribe({
      {"camera", "pos"}
    }, [&](Config::Version v) { ResetPosition(); }, Config::SYNC_NOW);

  scale_subscription_ = config_->Subscribe({
      {"camera", "scale"}
    }, [&](Config::Version v) { ResetScale(); }, Config::SYNC_NOW);

  rotation_subscription_ = config_->Subscribe({
      {"camera", "yaw"},
      {"camera", "pitch"},
      {"camera", "quat"}
    }, [&](Config::Version v) { ResetRotation(); }, Config::SYNC_NOW);

  passive_subscription_ = config_->Subscribe({
      {"camera", "mode"},
      {"camera", "fov"},
      {"camera", "quat"},
      {"camera", "move_speed"},
      {"camera", "turn_speed"},
      {"camera", "scale_speed"}
    }, [&](Config::Version v) { ResetPassive(); }, Config::SYNC_NOW);
}

void Camera::ResetPosition() {
  position_ = JsonUtil::vec3Value(config_->Current().Get({"camera", "pos"}));
}
void Camera::ResetScale() {
  scale_ = JsonUtil::doubleValue(config_->Current().Get({ "camera", "scale" }));
}
void Camera::ResetRotation() {
  float new_yaw = yaw_;
  float new_pitch = pitch_;
  fquat new_quat = quat_;

  auto x = config_->Current().TryGet({ "camera", "yaw" });
  if (!x.isNull())
    new_yaw = (float)JsonUtil::doubleValue(x);
  x = config_->Current().TryGet({ "camera", "pitch" });
  if (!x.isNull())
    new_pitch = (float)JsonUtil::doubleValue(x);
  x = config_->Current().TryGet({ "camera", "quat" });
  if (!x.isNull())
    new_quat = JsonUtil::quatValue(x);

  yaw_ = new_yaw;
  pitch_ = new_pitch;
  quat_ = new_quat;
}
void Camera::ResetPassive() {
  std::string s = config_->Current().GetString({ "camera", "mode" });
  Mode new_mode;
  if (s == "fps") {
    new_mode = YAW_PITCH;
  } else if (s == "space") {
    new_mode = SPACE;
  } else
    throw ConfigValueFormatException("unknown camera mode: " + s);

  float new_fov = (float)JsonUtil::doubleValue(
    config_->Current().Get({ "camera", "fov" }));

  float new_move_speed = (float)JsonUtil::doubleValue(
    config_->Current().Get({ "camera", "move_speed" }));
  float new_turn_speed = (float)JsonUtil::doubleValue(
    config_->Current().Get({ "camera", "turn_speed" }));
  float new_scale_speed = (float)JsonUtil::doubleValue(
    config_->Current().Get({ "camera", "scale_speed" }));

  mode_ = new_mode;
  fov_ = new_fov * kDegToRad;
  move_speed_ = new_move_speed * kDegToRad;
  turn_speed_ = new_turn_speed * kDegToRad;
  scale_speed_ = new_scale_speed * kDegToRad;
}

void Camera::Reset() {
  try {
    ResetPosition();
    ResetScale();
    ResetRotation();
    ResetPassive();
  } catch (...) {
    LogCurrentException();
  }
}

void Camera::TurnRelative(fvec2 delta) {
  if (fabs(delta.x) + fabs(delta.y) < 1e-5)
    return;

  if (mode_ == YAW_PITCH) {
    yaw_   += delta.x * turn_speed_;
    pitch_ = std::max((float)-M_PI/2, std::min((float)M_PI/2, pitch_
      + delta.y * turn_speed_));
  } else {
    quat_ = quat_ * head_orientation_
      * fquat((float)delta.Length() * turn_speed_,
              fvec3(delta.y, -delta.x, 0))
      * head_orientation_.Inverse();
    quat_.NormalizeMe();
  }
}

void Camera::MoveRelative(fvec3 delta) {
  position_ += dvec3(Rotation(head_orientation_).Transform(delta)) / scale_;
}

void Camera::ScaleRelative(double factor) {
  position_ = Position(head_position_);
  scale_ *= exp(factor * scale_speed_);
  position_ = Position(-head_position_);
}

dvec3 Camera::Position(fvec3 eye_position) {
  return position_ + dvec3(Rotation().Transform(eye_position)) / scale_;
}

fquat Camera::Rotation(fquat eye_orientation) const {
  fquat res;
  if (mode_ == SPACE)
    res = quat_;
  else if (have_head_pose_)
    res = fquat(yaw_, fvec3(0, -1, 0));
  else
    res = fquat(yaw_, fvec3(0, -1, 0)) * fquat(pitch_, fvec3(1, 0, 0));
  return res * eye_orientation;
}

fmat4 Camera::RotationProjectionMatrix() const {
  // to convert point from global coordinates to projected coordinates
  return
    // 2. project
    fmat4::PerspectiveProjectionMatrix(
      fov_, aspect_ratio_, near_clip_plane_, 0) *
    // 1. rotate coordinate system (transposition is same as inverse here)
    Rotation().Inverse().ToMatrix();
}

void Camera::SetPose(fvec3 position, fquat orientation) {
  head_position_ = position;
  head_orientation_ = orientation;
  have_head_pose_ = true;
}

void Camera::ClearPose() {
  position_ = Position(head_position_);
  head_position_ = fvec3(0, 0, 0);
  if (mode_ == SPACE) {
    quat_ *= head_orientation_;
    quat_.NormalizeMe();
  } else {
    // TODO: extract yaw and pitch from head_orientation
    //       and update yaw_ and pitch_
  }
  head_orientation_ = fquat(1, 0, 0, 0);
  have_head_pose_ = false;
}

}
