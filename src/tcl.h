
#pragma once

#include <vector>
#include <string>

namespace tcl {

  enum ReturnCode
  {
    RET_ERROR,
    RET_OK,
    RET_RETURN,
    RET_BREAK,
    RET_CONTINUE
  };

  struct Procedure
  {
    virtual ReturnCode call(std::vector<std::string> const& args) = 0;
  };

  void init();
  void shutdown();
  void reset();

  template <typename FuncT>
  bool bind(const char * name, FuncT func);

  ReturnCode evaluate(std::string const& code);

  // -- Internal functions --

  bool _registerProc(const char * name, Procedure * function);
  ReturnCode _arityError(std::string const& command);
  ReturnCode _reportError(std::string const& _error);
  void _return(std::string const& value);
}

/// Used to register function to the tcl interpreter
#define PROC(name, fun) namespace { static bool __dummy##fun = tcl::bind(name, fun); }

#include "tcl.inl"
