/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_SHADER_HPP
#define ENGINE_SHADER_HPP

#include "GL.hpp"
#include <string>
#include <memory>

class CProgram;
class CShader;

/// @brief shader ptr
typedef std::shared_ptr<CShader> TShaderPtr;

/// @brief shader holder
class CShader
{
   public:
      /// @brief factory methods returning shader
      static TShaderPtr createVertexShader(const std::string& src)   { return create(src, GL_VERTEX_SHADER);}
      static TShaderPtr createFragmentShader(const std::string& src) { return create(src, GL_FRAGMENT_SHADER);}

      /// @brief default constructor
      CShader() = default;

      /// @brief destructor
      ~CShader();

      /// @brief @returns true if shader was created and compiled fine
      bool isValid() const;

   private:
      /// @brief disabled stuff
      CShader(const CShader&) = delete;
      CShader& operator=(const CShader& other) = delete;

      /// @brief factory method returning shader
      static TShaderPtr create(const std::string& src, GLenum shaderType);

      /// @brief shader object id
      GLuint mObject = 0;

      friend class CProgram;
};

inline bool CShader::isValid() const
{
   return mObject;
}

#endif // ENGINE_SHADER_HPP
