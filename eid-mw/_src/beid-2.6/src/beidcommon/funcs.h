#ifndef _FUNCS_H
#define _FUNCS_H

#ifdef _STLP_NEW_PLATFORM_SDK
#ifdef __in
    #undef __in
#endif
#ifdef __out
    #undef __out
#endif
#endif

#include <sstream>

template <class T> bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
    std::istringstream iss(s);
    return !(iss >> f >> t).fail();
}

template <class T> std::string to_string(T t, std::ios_base & (*f)(std::ios_base&))
{
  std::ostringstream oss;
  oss << f << t;
  return oss.str();
}

#endif // _FUNCS_H

