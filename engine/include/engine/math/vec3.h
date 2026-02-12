#pragma once

#include <cmath>

namespace engine::math {

struct Vec3 {
  float x = 0.0F;
  float y = 0.0F;
  float z = 0.0F;
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3 operator-(const Vec3& a, const Vec3& b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 operator*(const Vec3& v, const float s) {
  return {v.x * s, v.y * s, v.z * s};
}

inline float dot(const Vec3& a, const Vec3& b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
  return {
      (a.y * b.z) - (a.z * b.y),
      (a.z * b.x) - (a.x * b.z),
      (a.x * b.y) - (a.y * b.x),
  };
}

inline float length(const Vec3& v) {
  return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3& v) {
  const float len = length(v);
  if (len <= 0.000001F) {
    return {0.0F, 0.0F, 0.0F};
  }
  return {v.x / len, v.y / len, v.z / len};
}

} // namespace engine::math
