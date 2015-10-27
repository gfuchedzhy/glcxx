/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_SHADER_HPP
#define ENGINE_SHADER_HPP

#include "GL.hpp"
#include <string>

class CProgram;

/// @brief shader holder
class CShader
{
   public:
      /// @brief factory method returning shader
      static CShader create(const std::string& src, GLenum shaderType);

      /// @brief move constructor
      CShader(CShader&& other);

      /// @brief move assignment
      CShader& operator=(CShader&& other);

      /// @brief destructor
      ~CShader();

      /// @brief @returns true if shader was created and compiled fine
      bool isValid() const;

   private:
      /// @brief private stuff
      CShader() = default;
      CShader(const CShader&) = default;
      CShader& operator=(const CShader& other) = default;

      /// @brief release shader associated with this object
      void destroy();

      /// @brief shader object id
      GLuint mObject = 0;
      friend class CProgram;
};

inline bool CShader::isValid() const
{
   return mObject;
}

#endif // ENGINE_SHADER_HPP
