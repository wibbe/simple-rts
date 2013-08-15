
#include "util.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>

namespace util
{

  std::string loadFileStr(const char * filename, uint32_t * size)
  {
    FILE * f = fopen(platform::resourcePath(filename).c_str(), "rb");
    if (!f) return "";
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char * buf = (char *)malloc(len + 1);
    if (!buf) return "";
    buf[len] = 0;
    size_t rlen = fread(buf, 1, len, f);
    fclose(f);
    if(len != rlen || len <= 0)
    {
        free(buf);
        return "";
    };

    if (size != NULL) *size = len;

    std::string data(buf);
    free(buf);

    return data;
  };

}