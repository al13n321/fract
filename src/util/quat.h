#pragma once

#include "vec.h"

namespace fract {

// Quaternion.
struct quatf {
  float a{0}, b{0}, c{0}, d{0};

  quat() {}
  quat(float a, float b, float c, float d): a(a), b(b), c(c), d(d) {}

  // Angle in radians. Angle is measured clockwise
  // when viewed with view direction equal to axis.
  quat(float angle, fvec3 axis) {
    float sn = sinf(angle * .5f);
    float cs = cosf(angle * .5f);
    a = cs;
    axis.NormalizeMe();
    axis *= sn;
    b = axis.x;
    c = axis.y;
    d = axis.z;
  }

  quat operator*(const quat &q) const {
    return quat(
      a*q.a - b*q.b - c*q.c - d*q.d,
      a*q.b + b*q.a + c*q.d - d*q.c,
      a*q.c + c*q.a + d*q.b - b*q.d,
      a*q.d + d*q.a + b*q.c - c*q.b);
  }

  void NormalizeMe() {
    float z = sqrtf(a*a + b*b + c*c + d*d);
    a /= z;
    b /= z;
    c /= z;
    d /= z;
  }
};

}
