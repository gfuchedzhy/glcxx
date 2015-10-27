/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Program.hpp"
#include <cassert>
#include <memory>

CProgram::CProgram(CProgram&& other)
   : CProgram(static_cast<CProgram&>(other))
{
   other.mObject = 0;
}

CProgram& CProgram::operator=(CProgram&& other)
{
   assert(this != &other);
   destroy();
   *this = other;
   other.mObject = 0;
   other.mVertexShader = std::move(mVertexShader);
   other.mFragmentShader = std::move(mFragmentShader);
   return *this;
}

void CProgram::destroy()
{
   if (isValid())
   {
      Log::msg("destroying program ", mObject);
      gl(glDetachShader, mObject, mVertexShader.mObject);
      gl(glDetachShader, mObject, mFragmentShader.mObject);
      gl(glDeleteProgram, mObject);
   }
}

CProgram::~CProgram()
{
   destroy();
}

CProgram CProgram::create(const std::string& vertexSrc, const std::string& fragmentSrc)
{
   Log::msg("creating program");
   CProgram p;
   p.mVertexShader   = CShader::create(vertexSrc, GL_VERTEX_SHADER);
   p.mFragmentShader = CShader::create(fragmentSrc, GL_FRAGMENT_SHADER);

   p.mObject = gl(glCreateProgram);
   gl(glAttachShader, p.mObject, p.mVertexShader.mObject);
   gl(glAttachShader, p.mObject, p.mFragmentShader.mObject);
   gl(glLinkProgram,  p.mObject);
   GLint linked;
   gl(glGetProgramiv, p.mObject, GL_LINK_STATUS, &linked);
   if (!linked)
   {
      GLint logLength = 0;
      gl(glGetProgramiv, p.mObject, GL_INFO_LOG_LENGTH, &logLength);
      if (logLength > 0)
      {
         auto str = std::make_unique<GLchar[]>(logLength);
         GLsizei len = 0;
         gl(glGetProgramInfoLog, p.mObject, logLength, &len, str.get());
         Log::err("program link failed: ", str.get());
      }
      p.destroy();
   }
   return p;
}
