
#pragma once

#include <string>
#include <stdint.h>

namespace util
{

  std::string loadFileStr(const char * filename, uint32_t * size = NULL);

}