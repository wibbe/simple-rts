
#pragma once

namespace math
{

  struct Vector3
  {
    Vector3();
    Vector3(float x, float y, float z);
    Vector3(Vector3 const& v);

    float x, y, z;
  };

  struct Vector4
  {
    Vector4();
    Vector4(float x, float y, float z, float w);
    Vector4(Vector3 const& v);
    Vector4(Vector4 const& v);

    float x, y, z, w;
  };

  struct Ray
  {
    Ray(Vector3 const& start, Vector3 const& dir);

    Vector3 start;
    Vector3 dir;
  };

  struct Box
  {
    Box();
    Box(Box const& box);
    Box(Vector3 const& min, Vector3 const& max);

    Vector3 min;
    Vector3 max;
  };

}
