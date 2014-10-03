#include "camera.h"

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

}