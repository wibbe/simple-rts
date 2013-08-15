
#pragma once

#include <string>

namespace platform
{
  void messageBox(const char * title, const char * message);
  std::string resourcePath(const char * file);
}