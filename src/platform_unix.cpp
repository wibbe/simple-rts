
#include <stdio.h>
#include <string>

namespace platform
{

  void messageBox(const char * title, const char * message)
  {
    printf("%s: %s\n", title, message);
  }

  std::string resourcePath(const char * file)
  {
    return std::string(file);
  }

}