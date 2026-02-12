#pragma once

#include "engine/math/vec4.h"
#include "engine/math/vec3.h"

#include <algorithm>
#include <cmath>
#include <iterator>

namespace engine::math {

struct Mat4 {
  float m[16] = {
      1.0F, 0.0F, 0.0F, 0.0F,
      0.0F, 1.0F, 0.0F, 0.0F,
      0.0F, 0.0F, 1.0F, 0.0F,
      0.0F, 0.0F, 0.0F, 1.0F,
  };
};

inline Mat4 identity() {
  return Mat4{};
}

inline Mat4 multiply(const Mat4& a, const Mat4& b) {
  Mat4 out{};
  for (int c = 0; c < 4; ++c) {
    for (int r = 0; r < 4; ++r) {
      out.m[(c * 4) + r] = (a.m[(0 * 4) + r] * b.m[(c * 4) + 0]) +
                           (a.m[(1 * 4) + r] * b.m[(c * 4) + 1]) +
                           (a.m[(2 * 4) + r] * b.m[(c * 4) + 2]) +
                           (a.m[(3 * 4) + r] * b.m[(c * 4) + 3]);
    }
  }
  return out;
}

inline Vec3 multiply_point(const Mat4& m, const Vec3& v) {
  return {
      (m.m[0] * v.x) + (m.m[4] * v.y) + (m.m[8] * v.z) + m.m[12],
      (m.m[1] * v.x) + (m.m[5] * v.y) + (m.m[9] * v.z) + m.m[13],
      (m.m[2] * v.x) + (m.m[6] * v.y) + (m.m[10] * v.z) + m.m[14],
  };
}

inline Vec4 multiply_vec4(const Mat4& m, const Vec4& v) {
  return {
      (m.m[0] * v.x) + (m.m[4] * v.y) + (m.m[8] * v.z) + (m.m[12] * v.w),
      (m.m[1] * v.x) + (m.m[5] * v.y) + (m.m[9] * v.z) + (m.m[13] * v.w),
      (m.m[2] * v.x) + (m.m[6] * v.y) + (m.m[10] * v.z) + (m.m[14] * v.w),
      (m.m[3] * v.x) + (m.m[7] * v.y) + (m.m[11] * v.z) + (m.m[15] * v.w),
  };
}

inline Mat4 translation(const Vec3& t) {
  Mat4 out = identity();
  out.m[12] = t.x;
  out.m[13] = t.y;
  out.m[14] = t.z;
  return out;
}

inline Mat4 scale(const Vec3& s) {
  Mat4 out = identity();
  out.m[0] = s.x;
  out.m[5] = s.y;
  out.m[10] = s.z;
  return out;
}

inline Mat4 rotation_x(const float r) {
  const float c = std::cos(r);
  const float s = std::sin(r);
  Mat4 out = identity();
  out.m[5] = c;
  out.m[6] = s;
  out.m[9] = -s;
  out.m[10] = c;
  return out;
}

inline Mat4 rotation_y(const float r) {
  const float c = std::cos(r);
  const float s = std::sin(r);
  Mat4 out = identity();
  out.m[0] = c;
  out.m[2] = -s;
  out.m[8] = s;
  out.m[10] = c;
  return out;
}

inline Mat4 rotation_z(const float r) {
  const float c = std::cos(r);
  const float s = std::sin(r);
  Mat4 out = identity();
  out.m[0] = c;
  out.m[1] = s;
  out.m[4] = -s;
  out.m[5] = c;
  return out;
}

inline Mat4 rotation_euler_xyz(const Vec3& euler) {
  return multiply(multiply(rotation_z(euler.z), rotation_y(euler.y)), rotation_x(euler.x));
}

inline Mat4 trs(const Vec3& t, const Vec3& r, const Vec3& s) {
  return multiply(translation(t), multiply(rotation_euler_xyz(r), scale(s)));
}

inline Mat4 look_at(const Vec3& eye, const Vec3& target, const Vec3& world_up) {
  const Vec3 forward = normalize(target - eye);
  const Vec3 right = normalize(cross(forward, world_up));
  const Vec3 up = cross(right, forward);

  Mat4 out = identity();
  out.m[0] = right.x;
  out.m[1] = up.x;
  out.m[2] = -forward.x;
  out.m[4] = right.y;
  out.m[5] = up.y;
  out.m[6] = -forward.y;
  out.m[8] = right.z;
  out.m[9] = up.z;
  out.m[10] = -forward.z;
  out.m[12] = -dot(right, eye);
  out.m[13] = -dot(up, eye);
  out.m[14] = dot(forward, eye);
  return out;
}

inline Mat4 perspective(const float fov_radians, const float aspect, const float near_plane, const float far_plane) {
  const float tan_half_fov = std::tan(fov_radians / 2.0F);
  Mat4 out{};
  std::fill(std::begin(out.m), std::end(out.m), 0.0F);
  out.m[0] = 1.0F / (aspect * tan_half_fov);
  out.m[5] = 1.0F / tan_half_fov;
  out.m[10] = -(far_plane + near_plane) / (far_plane - near_plane);
  out.m[11] = -1.0F;
  out.m[14] = -(2.0F * far_plane * near_plane) / (far_plane - near_plane);
  return out;
}

} // namespace engine::math
