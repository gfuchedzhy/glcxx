/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Shader.hpp"
#include <cassert>

CShader::~CShader()
{
   if (isValid())
   {
      Log::msg("destroying shader ", mObject);
      gl(glDeleteShader, mObject);
   }
}

TShaderPtr CShader::create(const std::string& src, const GLenum shaderType)
{
   Log::msg("creating shader");
   auto sh = std::make_shared<CShader>();
   sh->mObject = gl(glCreateShader, shaderType);
   const GLint length = src.length();
   const GLchar* source = src.c_str();
   gl(glShaderSource, sh->mObject, 1, &source, &length);
   gl(glCompileShader, sh->mObject);
   GLint compiled;
   gl(glGetShaderiv, sh->mObject, GL_COMPILE_STATUS, &compiled);
   if (!compiled)
   {
      GLint logLength = 0;
      gl(glGetShaderiv, sh->mObject, GL_INFO_LOG_LENGTH, &logLength);
      if (logLength > 0)
      {
         auto str = std::make_unique<GLchar[]>(logLength);
         GLsizei len = 0;
         gl(glGetShaderInfoLog, sh->mObject, logLength, &len, str.get());
         Log::err("shader compile failed: ", str.get());
      }
      return 0;
   }
   return sh;
}
