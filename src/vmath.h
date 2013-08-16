
#pragma once

#include "vmath_types.h"

namespace math
{

  // -- Vector3 --

  namespace vec3
  {
    inline float dot(Vector3 const& a, Vector3 const& b)
    {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }
  }

  inline Vector3 operator / (float scalar, Vector3 v)
  {
    return Vector3(scalar / v.x, scalar / v.y, scalar / v.z);
  }

  inline Vector3 operator / (Vector3 v, float scalar)
  {
    return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
  }

  inline Vector3 operator * (Vector3 v, float scalar)
  {
    return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
  }

  inline Vector3 operator + (Vector3 a, Vector3 b)
  {
    return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
  }

  inline Vector3 operator - (Vector3 a, Vector3 b)
  {
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
  }

  // -- Vector4 --

  namespace vec4
  {
    inline float dot(Vector4 const& a, Vector4 const& b)
    {
      return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
  }

  // -- Box --

  namespace box
  {
    inline bool contains(Box const& box, Vector3 const& point)
    {
      return (point.x >= box.min.x) &&
             (point.y >= box.min.y) &&
             (point.z >= box.min.z) &&
             (point.x <= box.max.x) &&
             (point.y <= box.max.y) &&
             (point.z <= box.max.z);
    }

    inline bool intersect(Box const& a, Box const& b)
    {
      return !((a.min.x > b.max.x || a.max.x < b.min.x) ||
               (a.min.y > b.max.y || a.max.y < b.min.y) ||
               (a.min.z > b.max.z || a.max.z < b.min.z));
    }
  }

  // -- Ray --

  namespace ray
  {
    bool insersect(Ray const& ray, Box const& box);
  }

}
