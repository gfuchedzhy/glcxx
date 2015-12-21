/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Shader.hpp"
#include <memory>

CShader::CShader(const std::string& src, const GLenum shaderType)
{
   mObject = gl(glCreateShader, shaderType);
   Log::msg("creating shader ", mObject);
#ifdef GL_LOG_ALL
   Log::msg(src);
#endif
   std::string versionedSrc("#version 130\n");
   versionedSrc += src;
   const GLint length = versionedSrc.length();
   const GLchar* source = versionedSrc.c_str();
   gl(glShaderSource, mObject, 1, &source, &length);
   gl(glCompileShader, mObject);
   GLint compiled;
   gl(glGetShaderiv, mObject, GL_COMPILE_STATUS, &compiled);
   if (!compiled)
   {
      GLint logLength = 0;
      gl(glGetShaderiv, mObject, GL_INFO_LOG_LENGTH, &logLength);
      if (logLength > 0)
      {
         auto str = std::make_unique<GLchar[]>(logLength);
         GLsizei len = 0;
         gl(glGetShaderInfoLog, mObject, logLength, &len, str.get());
         Log::err("shader compile failed: ", str.get());
      }
      this->~CShader();
      throw std::runtime_error{"shader compilation failed"};
   }
}

CShader::~CShader()
{
   Log::msg("destroying shader ", mObject);
   gl(glDeleteShader, mObject);
}
