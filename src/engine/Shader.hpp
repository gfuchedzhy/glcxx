/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_SHADER_HPP
#define ENGINE_SHADER_HPP

#include "GL.hpp"
#include <string>

/// @brief shader holder
class CShader
{
   public:
      /// @brief constructor
      CShader(const std::string& src, GLenum shaderType);

      /// @brief destructor
      ~CShader();

   private:
      /// @brief disabled stuff
      CShader(const CShader&) = delete;
      CShader& operator=(const CShader& other) = delete;

      /// @brief shader object id
      GLuint mObject = 0;

      friend class CProgramObject;
};

#endif // ENGINE_SHADER_HPP
