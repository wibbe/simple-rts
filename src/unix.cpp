
#include "cube.h"

void messagebox(const char * title, const char * message)
{
  printf("%s: %s\n", title, message);
}

char * resourcepath(const char * file)
{
  return newstring(file);
}
