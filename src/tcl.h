
#pragma once

#include <vector>
#include <string>

namespace tcl {

  typedef uint32_t RetCode;

  struct Procedure
  {
    virtual RetCode call(std::vector<std::string> const& args) = 0;
  };

  void init();
  void shutdown();

  template <typename FuncT>
  bool bind(const char * name, FuncT func);

  // Internal functions
  void _registerProc(const char * name, Procedure * function);
  void _return(std::string const& value);

}

#define PROC(name, fun) namespace { static bool __dummy##fun = tcl::bind(name, fun); }

#include "tcl.inl"