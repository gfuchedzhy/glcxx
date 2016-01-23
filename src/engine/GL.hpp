/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_GL_HPP
#define ENGINE_GL_HPP

#include <SFML/OpenGL.hpp>
#include "Log.hpp"

#if defined GL_LOG_ERR || defined GL_LOG_ALL
namespace detail
{
   // gl functions doesn't use move semantics and accept only scalar types,
   // therefore no need in perfect forwarding, no need in passing by reference,
   // and this wrappers are inline anyway
   template<typename TFunction, typename... TArgs>
   inline auto substituteVoidWith0(TFunction glFunc, TArgs... args) ->
      typename std::enable_if<std::is_void<decltype(glFunc(args...))>::value, int>::type
   {
      glFunc(args...);
      return 0;
   }

   template<typename TFunction, typename... TArgs>
   inline auto substituteVoidWith0(TFunction glFunc, TArgs... args) ->
      typename std::enable_if<!std::is_void<decltype(glFunc(args...))>::value, decltype(glFunc(args...))>::type
   {
      return glFunc(args...);
   }

   template<typename TFunction, typename... TArgs>
   inline auto callGL(TFunction   glFunc,
                      const char* glFuncName,
                      const char* filename,
                      int         line,
                      TArgs...    args)
   {
#ifdef GL_LOG_ALL
      Log::msg(glFuncName, std::make_tuple(args...));
#endif
      auto retVal = substituteVoidWith0(glFunc, args...);
      const GLenum err = glGetError();
      if (GL_NO_ERROR != err)
      {
         Log::err("gl error code ", err, " in ",
                  glFuncName, std::make_tuple(args...),
                  " at ", filename, ':', line);
      }
      return static_cast<decltype(glFunc(args...))>(retVal);
   }
}
#define gl(glFunc, ...) ::detail::callGL(glFunc, #glFunc, __FILE__, __LINE__, ##__VA_ARGS__)
#else // no logging
#define gl(glFunc, ...) glFunc(__VA_ARGS__)
#endif

#endif // ENGINE_GL_HPP
