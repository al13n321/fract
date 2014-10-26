#include "camera.h"
#include "exceptions.h"

namespace fract {

Camera::~Camera() {}

void Camera::MoveRelative(fvec3 delta) {
	fmat4 rot = RotationMatrix();
	position_ += rot.Transform(delta) / scale_;
}

fmat4 Camera::RotationMatrix() const {
	return fmat4::RotationMatrix(-yaw_, pitch_, -roll_);
}

fmat4 Camera::RotationProjectionMatrix() const {
  // to convert point from global coordinates to projected coordinates
  return
    // 2. project
    fmat4::PerspectiveProjectionMatrix(fov_, aspect_ratio_, near_clip_plane_, 0) *
    // 1. rotate coordinate system (transposition is same as inverse here)
	  fmat4::RotationMatrix(-yaw_, pitch_, -roll_).Transposed();
}

void Camera::FromJson(const Json::Value &value) {
  try {
    if (!value.isObject())
      throw ConfigValueFormatException("camera is not an object");
    const auto &type = value["type"];
    if (!type.isNull() && type != "fps")
      throw ConfigValueFormatException("unrecognized camera type");
    double new_fov = JsonUtil::doubleValue(value["fov"]);
    dvec3 new_pos = JsonUtil::vec3Value(value["pos"]);
    double new_yaw = JsonUtil::doubleValue(value["yaw"]);
    double new_pitch = JsonUtil::doubleValue(value["pitch"]);
    double new_scale = JsonUtil::doubleValue(value["scale"]);

    fov_ = new_fov;
    position_ = new_pos;
    yaw_ = new_yaw;
    pitch_ = new_pitch;
    scale_ = new_scale;
  } catch (...) {
    LogCurrentException();
  }
}

}
