/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_GL_HPP
#define ENGINE_GL_HPP

#include <SFML/OpenGL.hpp>
#include "Log.hpp"

template<typename TFunction, typename... TArgs>
inline auto substituteVoidWith0(TFunction glFunc, TArgs&&... args) ->
   typename std::enable_if<std::is_void<decltype(glFunc(args...))>::value, int>::type
{
   glFunc(std::forward<TArgs>(args)...);
   return 0;
}

template<typename TFunction, typename... TArgs>
inline auto substituteVoidWith0(TFunction glFunc, TArgs&&... args) ->
   typename std::enable_if<!std::is_void<decltype(glFunc(args...))>::value, decltype(glFunc(args...))>::type
{
   return glFunc(std::forward<TArgs>(args)...);
}

template<typename TFunction, typename... TArgs>
inline auto callGL(TFunction   glFunc,
                   const char* glFuncName,
                   const char* filename,
                   int         line,
                   TArgs&&...  args) -> decltype(glFunc(args...))
{
   // Log::msg(glFuncName, '(', Log::concatWithDelim(", ", std::forward<TArgs>(args)...), ')');
   auto retVal = substituteVoidWith0(glFunc, std::forward<TArgs>(args)...);
   const GLenum err = glGetError();
   if (GL_NO_ERROR != err)
   {
      Log::err("gl error code ", err, " in ", glFuncName,
               '(', Log::concatWithDelim(", ", std::forward<TArgs>(args)...), ") at ", filename, ':', line);
   }
   return static_cast<decltype(glFunc(args...))>(retVal);
}

#define gl(glFunc, ...) callGL(glFunc, #glFunc, __FILE__, __LINE__, ##__VA_ARGS__)

#endif // ENGINE_GL_HPP

