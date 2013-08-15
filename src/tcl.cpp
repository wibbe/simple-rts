
#include "config.h"
#include "tcl.h"

#include <map>

namespace tcl {
  namespace {
    std::map<std::string, Procedure *> _procedures;
  }

  void init()
  {
  }

  void shutdown()
  {
  }

  void _registerProc(const char * name, Procedure * function)
  {
  }

  void _return(std::string const& value)
  {
  }
}