
#pragma once

#include <stdint.h>

namespace collide
{

  void reset(uint32_t width, uint32_t height);

  void set(float x, float z, bool on);
  void setI(uint32_t x, uint32_t z, bool on);

  bool check(float x, float z);
  bool checkI(uint32_t x, uint32_t z);

}