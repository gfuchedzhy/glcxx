/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Shader.hpp"
#include <cassert>
#include <memory>

CShader::CShader(CShader&& other)
   : CShader(static_cast<CShader&>(other))
{
   other.mObject = 0;
}

CShader& CShader::operator=(CShader&& other)
{
   assert(this != &other);
   destroy();
   *this = other;
   other.mObject = 0;
   return *this;
}

void CShader::destroy()
{
   if (isValid())
   {
      Log::msg("destroying shader ", mObject);
      gl(glDeleteShader, mObject);
   }
}

CShader::~CShader()
{
   destroy();
}

CShader CShader::create(const std::string& src, const GLenum shaderType)
{
   Log::msg("creating shader");
   CShader sh;
   sh.mObject = gl(glCreateShader, shaderType);
   const GLint length = src.length();
   const GLchar* source = src.c_str();
   gl(glShaderSource, sh.mObject, 1, &source, &length);
   gl(glCompileShader, sh.mObject);
   GLint compiled;
   gl(glGetShaderiv, sh.mObject, GL_COMPILE_STATUS, &compiled);
   if (!compiled)
   {
      GLint logLength = 0;
      gl(glGetShaderiv, sh.mObject, GL_INFO_LOG_LENGTH, &logLength);
      if (logLength > 0)
      {
         auto str = std::make_unique<GLchar[]>(logLength);
         GLsizei len = 0;
         gl(glGetShaderInfoLog, sh.mObject, logLength, &len, str.get());
         Log::err("shader compile failed: ", str.get());
      }
      sh.destroy();
   }
   return sh;
}
