// Copyright 2015, 2016 Grygoriy Fuchedzhy
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GLCXX_GL_HPP
#define GLCXX_GL_HPP

#include <SFML/OpenGL.hpp>
#include <functional>
#include <sstream>
#include "utils.hpp"

namespace glcxx
{
   /// @brief error callback: function name, scope, error type
   /// @note this typedef should be visible regardless of error check presense,
   /// because library with checks can be built for client without checks
   using glerror_cb_t = std::function<void(const std::stringstream&, const char*, GLenum)>;

#ifndef GLCXX_SUPPRESS_GL_ERROR_CHECKS
   /// @brief set error callback
   void set_glerror_cb(glerror_cb_t cb);

   namespace detail
   {
      /// @brief error callback: function name, scope, error type
      extern glerror_cb_t glerror_cb;

      // gl functions doesn't use move semantics and accept only scalar types,
      // therefore no need in perfect forwarding, no need in passing by
      // reference, and this wrappers are inline anyway
      template<typename TFunction, typename... TArgs>
      inline auto call_gl_no_void_return(TFunction glFunc, TArgs... args) ->
         typename std::enable_if<std::is_void<decltype(glFunc(args...))>::value, int>::type
      {
         glFunc(args...);
         return 0;
      }

      template<typename TFunction, typename... TArgs>
      inline auto call_gl_no_void_return(TFunction glFunc, TArgs... args) ->
         typename std::enable_if<!std::is_void<decltype(glFunc(args...))>::value, decltype(glFunc(args...))>::type
      {
         return glFunc(args...);
      }

      template<typename TFunction, typename... TArgs>
      auto call_gl(TFunction glFunc, const char* glFuncName, const char* scope, TArgs... args)
      {
         auto retVal = call_gl_no_void_return(glFunc, args...);
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

#define GLCXX_STRINGIFY(x) #x
#define GLCXX_TOSTRING(x) GLCXX_STRINGIFY(x)
#define glcxx_gl(glFunc, ...) glcxx::detail::call_gl(glFunc, #glFunc, __FILE__ ":" GLCXX_TOSTRING(__LINE__), ##__VA_ARGS__)

#else // no logging
#define glcxx_gl(glFunc, ...) glFunc(__VA_ARGS__)
#endif

} // namespace

#endif // include guard
