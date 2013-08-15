
#pragma once

#include <stdint.h>

namespace unit
{

  typedef uint32_t ID;

  ID spawn();
  void kill(ID id);

}