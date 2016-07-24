/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_GL_HPP
#define GLCXX_SRC_GL_HPP

#include <SFML/OpenGL.hpp>
#include <functional>
#include <sstream>
#include "utils.hpp"

//todo review this file

//todo namespace
// namespace glcxx
// {
//#if defined GL_LOG_ERR || defined GL_LOG_ALL
   /// @brief error callback: function name, scope, error type
   using glerror_cb_t = std::function<void(const std::stringstream&, const char*, GLenum)>;

   namespace detail
   {
      /// @brief error callback: function name, scope, error type
      extern glerror_cb_t glerror_cb;

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
      auto callGL(TFunction   glFunc, const char* glFuncName, const char* scope, TArgs... args)
      {
         auto retVal = substituteVoidWith0(glFunc, args...);
         if (glerror_cb)
         {
            const GLenum err = glGetError();
            if (GL_NO_ERROR != err)
            {
               std::stringstream str;
               str << glFuncName << std::tie(args...);
               glerror_cb(str, scope, err);
            }
         }
         return static_cast<decltype(glFunc(args...))>(retVal);
      }
   }

   void set_glerror_cb(glerror_cb_t cb);
// }

#define GLCXX_STRINGIFY(x) #x
#define GLCXX_TOSTRING(x) GLCXX_STRINGIFY(x)
#define gl(glFunc, ...) ::detail::callGL(glFunc, #glFunc, __FILE__ ":" GLCXX_TOSTRING(__LINE__), ##__VA_ARGS__)
// #else // no logging
// #define gl(glFunc, ...) glFunc(__VA_ARGS__)
// #endif

#endif
