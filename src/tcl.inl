
#include "meta_info.h"

namespace tcl {

  namespace impl {
    template <int ArgCount, typename FuncT, typename ReturnT, typename ParamT>
    struct CProcedure : public Procedure { };

    template <typename FuncT>
    struct CProcedure<0, FuncT, void, meta::NullType> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 1)
          return _arityError(args[0]);

        _function();
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT>
    struct CProcedure<0, FuncT, ReturnT, meta::NullType> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 1)
          return _arityError(args[0]);

        _function();
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename P1>
    struct CProcedure<1, FuncT, void, TYPELIST_1(P1)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 2)
          return _arityError(args[0]);

        _function(meta::String<P1>::from(args[1]));
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT, typename P1>
    struct CProcedure<1, FuncT, ReturnT, TYPELIST_1(P1)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 2)
          return _arityError(args[0]);

        ReturnT ret = _function(meta::String<P1>::from(args[1]));
        tcl::_return(meta::String<ReturnT>::to(ret));
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename P1, typename P2>
    struct CProcedure<2, FuncT, void, TYPELIST_2(P1, P2)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 3)
          return _arityError(args[0]);

        _function(meta::String<P1>::from(args[1]),
                  meta::String<P2>::from(args[2]));
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT, typename P1, typename P2>
    struct CProcedure<2, FuncT, ReturnT, TYPELIST_2(P1, P2)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 3)
          return _arityError(args[0]);

        ReturnT ret = _function(meta::String<P1>::from(args[1]),
                                meta::String<P2>::from(args[2]));
        tcl::_return(meta::String<ReturnT>::to(ret));
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename P1, typename P2, typename P3>
    struct CProcedure<3, FuncT, void, TYPELIST_3(P1, P2, P3)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 4)
          return _arityError(args[0]);

        _function(meta::String<P1>::from(args[1]),
                  meta::String<P2>::from(args[2]),
                  meta::String<P3>::from(args[3]));
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT, typename P1, typename P2, typename P3>
    struct CProcedure<3, FuncT, ReturnT, TYPELIST_3(P1, P2, P3)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 4)
          return _arityError(args[0]);

        ReturnT ret = _function(meta::String<P1>::from(args[1]),
                                meta::String<P2>::from(args[2]),
                                meta::String<P3>::from(args[3]));
        tcl::_return(meta::String<ReturnT>::to(ret));
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename P1, typename P2, typename P3, typename P4>
    struct CProcedure<4, FuncT, void, TYPELIST_4(P1, P2, P3, P4)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 5)
          return _arityError(args[0]);

        _function(meta::String<P1>::from(args[1]),
                  meta::String<P2>::from(args[2]),
                  meta::String<P3>::from(args[3]),
                  meta::String<P4>::from(args[4]));
        return RET_OK;
      }

      FuncT _function;
    };

    template <typename FuncT, typename ReturnT, typename P1, typename P2, typename P3, typename P4>
    struct CProcedure<4, FuncT, ReturnT, TYPELIST_4(P1, P2, P3, P4)> : public Procedure
    {
      CProcedure(FuncT function) : _function(function) { }

      ReturnCode call(std::vector<std::string> const& args)
      {
        if (args.size() != 5)
          return _arityError(args[0]);

        ReturnT ret = _function(meta::String<P1>::from(args[1]),
                                meta::String<P2>::from(args[2]),
                                meta::String<P3>::from(args[3]),
                                meta::String<P4>::from(args[4]));
        tcl::_return(meta::String<ReturnT>::to(ret));
        return RET_OK;
      }

      FuncT _function;
    };
  }

  template <typename FuncT>
  bool bind(const char * name, FuncT func)
  {
    return _registerProc(name, static_cast<Procedure *>((new impl::CProcedure<meta::Length<typename meta::FunctionInfo<FuncT>::Params>::Value,
                                                                              FuncT,
                                                                              typename meta::FunctionInfo<FuncT>::Return,
                                                                              typename meta::FunctionInfo<FuncT>::Params>(func))));
  }

}