
#pragma once

#include <string>
#include <stdint.h>

namespace input
{

  void init();
  void shutdown();

  void setMousePos(int x, int y);
  void key(std::string const& name, bool down);

  int32_t mouseX();
  int32_t mouseY();
}