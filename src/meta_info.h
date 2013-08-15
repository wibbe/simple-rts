
#pragma once

#include <cstdlib>
#include <stdio.h>

namespace meta {

  // -- Type Lists --

  class NullType { };

  #define TYPELIST_1(T1) \
    meta::TypeList<T1, meta::NullType>
  #define TYPELIST_2(T1, T2) \
    meta::TypeList<T1, TYPELIST_1(T2) >
  #define TYPELIST_3(T1, T2, T3) \
    meta::TypeList<T1, TYPELIST_2(T2, T3) >
  #define TYPELIST_4(T1, T2, T3, T4) \
    meta::TypeList<T1, TYPELIST_3(T2, T3, T4) >
  #define TYPELIST_5(T1, T2, T3, T4, T5) \
    meta::TypeList<T1, TYPELIST_4(T2, T3, T4, T5) >

  template <class T, class U>
  struct TypeList
  {
    typedef T Head;
    typedef U Tail;
  };

  template <class TList> struct Length;
  template <> struct Length<NullType> { enum { Value = 0 }; };
  template <class T, class U> struct Length< TypeList<T, U> > { enum { Value = 1 + Length<U>::Value }; };

  // -- Function Info --

  template <typename Fun>
  struct FunctionInfo;

  template <typename R>
  struct FunctionInfo<R (*)()>
  {
    typedef R Return;
    typedef NullType Params;
  };

  template <typename R, typename P1>
  struct FunctionInfo<R (*)(P1)>
  {
    typedef R Return;
    typedef TYPELIST_1(P1) Params;
  };

  template <typename R, typename P1, typename P2>
  struct FunctionInfo<R (*)(P1, P2)>
  {
    typedef R Return;
    typedef TYPELIST_2(P1, P2) Params;
  };

  template <typename R, typename P1, typename P2, typename P3>
  struct FunctionInfo<R (*)(P1, P2, P3)>
  {
    typedef R Return;
    typedef TYPELIST_3(P1, P2, P3) Params;
  };

  template <typename R, typename P1, typename P2, typename P3, typename P4>
  struct FunctionInfo<R (*)(P1, P2, P3, P4)>
  {
    typedef R Return;
    typedef TYPELIST_4(P1, P2, P3, P4) Params;
  };

  template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
  struct FunctionInfo<R (*)(P1, P2, P3, P4, P5)>
  {
    typedef R Return;
    typedef TYPELIST_5(P1, P2, P3, P4, P5) Params;
  };

  // To from string values

  template <typename T> struct String;

  template <> struct String<const char *>
  {
    static const char * from(std::string const& str)
    {
      return str.c_str();
    }

    static std::string to(const char * str)
    {
      return std::string(str);
    }
  };

  template <> struct String<std::string>
  {
    static std::string from(std::string const& str)
    {
      return str;
    }

    static std::string to(std::string str)
    {
      return str;
    }
  };

  template <> struct String<std::string const&>
  {
    static std::string from(std::string const& str)
    {
      return str;
    }

    static std::string to(std::string str)
    {
      return str;
    }
  };

  template <> struct String<bool>
  {
    static bool from(std::string const& str)
    {
      return std::atoi(str.c_str()) != 0;
    }

    static std::string to(bool val)
    {
      return val ? "1" : "0";
    }
  };

  template <> struct String<float>
  {
    static float from(std::string const& str)
    {
      return std::atof(str.c_str());
    }

    static std::string to(float val)
    {
      char buf[32];
      snprintf(buf, 32, "%f", val);
      return std::string(buf);
    }
  };

  #define STRING_INT(TYPE) \
    template <> struct String<TYPE> \
    { \
      static TYPE from(std::string const& str) \
      { \
        return std::atof(str.c_str()); \
      } \
      static std::string to(TYPE val) \
      { \
        char buf[32]; \
        snprintf(buf, 32, "%d", val); \
        return std::string(buf); \
      } \
    };

  STRING_INT(int8_t)
  STRING_INT(uint8_t)
  STRING_INT(int16_t)
  STRING_INT(uint16_t)
  STRING_INT(int32_t)
  STRING_INT(uint32_t)

  #undef STRING_INT

}