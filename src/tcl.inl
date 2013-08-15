
#include "meta_info.h"

namespace tcl {

  namespace impl {
    template <int ArgCount, typename FuncT, typename ReturnT, typename ParamT>
    struct CProcedure : public Procedure { };

    template <typename FuncT>
    struct CProcedure<0, FuncT, void, meta::NullType> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      RetCode call(std::vector<std::string> const& args)
      {
        _function();
        return 0;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT>
    struct CProcedure<0, FuncT, ReturnT, meta::NullType> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      RetCode call(std::vector<std::string> const& args)
      {
        _function();
        return 0;
      }

      FuncT _function;
    };

    template <typename FuncT, typename P1>
    struct CProcedure<1, FuncT, void, TYPELIST_1(P1)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      RetCode call(std::vector<std::string> const& args)
      {
        _function(meta::String<P1>::from(args[1]));
        return 0;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT, typename P1>
    struct CProcedure<1, FuncT, ReturnT, TYPELIST_1(P1)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      RetCode call(std::vector<std::string> const& args)
      {
        ReturnT ret = _function(meta::String<P1>::from(args[1]));
        tcl::_return(meta::String<ReturnT>::to(ret));
        return 0;
      }

      FuncT _function;
    };

    template <typename FuncT, typename P1, typename P2>
    struct CProcedure<2, FuncT, void, TYPELIST_2(P1, P2)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      RetCode call(std::vector<std::string> const& args)
      {
        _function(meta::String<P1>::from(args[1]),
                  meta::String<P2>::from(args[2]));
        return 0;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT, typename P1, typename P2>
    struct CProcedure<2, FuncT, ReturnT, TYPELIST_2(P1, P2)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      RetCode call(std::vector<std::string> const& args)
      {
        ReturnT ret = _function(meta::String<P1>::from(args[1]),
                                meta::String<P2>::from(args[2]));
        tcl::_return(meta::String<ReturnT>::to(ret));
        return 0;
      }

      FuncT _function;
    };

    template <typename FuncT, typename P1, typename P2, typename P3>
    struct CProcedure<3, FuncT, void, TYPELIST_3(P1, P2, P3)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      RetCode call(std::vector<std::string> const& args)
      {
        _function(meta::String<P1>::from(args[1]),
                  meta::String<P2>::from(args[2]));
        return 0;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT, typename P1, typename P2, typename P3>
    struct CProcedure<3, FuncT, ReturnT, TYPELIST_3(P1, P2, P3)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      RetCode call(std::vector<std::string> const& args)
      {
        ReturnT ret = _function(meta::String<P1>::from(args[1]),
                                meta::String<P2>::from(args[2]),
                                meta::String<P3>::from(args[3]));
        tcl::_return(meta::String<ReturnT>::to(ret));
        return 0;
      }

      FuncT _function;
    };
  }

  template <typename FuncT>
  bool bind(const char * name, FuncT func)
  {
    _registerProc(name, static_cast<Procedure *>((new impl::CProcedure<meta::Length<typename meta::FunctionInfo<FuncT>::Params>::Value,
                                                                       FuncT,
                                                                       typename meta::FunctionInfo<FuncT>::Return,
                                                                       typename meta::FunctionInfo<FuncT>::Params>(func))));
    return true;
  }

}