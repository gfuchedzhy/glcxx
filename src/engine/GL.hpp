/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_GL_HPP
#define ENGINE_GL_HPP

#include <SFML/OpenGL.hpp>

template<typename TFunction, typename... TArgs>
inline auto callGL(TFunction   glFunc,
                   const char* glFuncName,
                   const char* filename,
                   int         line,
                   TArgs...    args) -> decltype(glFunc(args...))
{
   glFunc(args...);
   const GLenum err = glGetError();
   if (GL_NO_ERROR != err)
   {
      Log::err("gl error code ", err, " in ", glFuncName,
               '(', Log::concatWithDelim(", ", args...), ") at ", filename, ':', line);
   }
}

#define gl(glFunc, ...) callGL(glFunc, #glFunc, __FILE__, __LINE__, __VA_ARGS__)

#endif // ENGINE_GL_HPP

