
#include "cube.h"

void messagebox(const char * title, const char * message)
{
  #ifdef WIN32
    MessageBox(NULL, msg, "Cube RTS fatal error", MB_OK|MB_SYSTEMMODAL);
  #else
    #error "This file should not be built on non Windows platforms"
  #endif
}

char * resourcepath(const char * file)
{
  return newstring(file);
}
