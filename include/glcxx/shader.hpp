/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SHADER_HPP
#define GLCXX_SHADER_HPP

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
         GLuint _object = 0;

      public:
         /// @brief constructor
         shader_base(GLenum shader_type);

         /// @brief destructor
         ~shader_base();
   };

   class shader : public shader_base
   {
         /// @brief program this shader is attached to
         GLuint _program_object;

      public:
         /// @brief constructor
         shader(const std::string& glsl_version, std::string src, GLuint program, GLenum shader_type);

         /// @brief destructor
         ~shader();
   };
}

#endif
