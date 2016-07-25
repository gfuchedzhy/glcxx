/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_SHADER_HPP
#define GLCXX_SRC_SHADER_HPP

#include <stdexcept>
#include <string>
#include "gl.hpp"

namespace glcxx
{
   /// @brief shader resource holder
   class shader_base
   {
         /// @brief disabled stuff
         shader_base(const shader_base&) = delete;
         shader_base& operator=(const shader_base& other) = delete;

      protected:
         /// @brief shader object id
         GLuint mObject = 0;

      public:
         /// @brief constructor
         shader_base(GLenum shader_type);

         /// @brief destructor
         ~shader_base();
   };

   class shader : public shader_base
   {
         /// @brief program this shader is attached to
         GLuint mProgramObject;

      public:
         /// @brief constructor
         shader(const std::string& src, GLuint program, GLenum shader_type);

         /// @brief destructor
         ~shader();
   };

   struct shader_compile_error : std::runtime_error
   {
         explicit shader_compile_error(const std::string& msg);
   };
}

#endif
