/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "ProgramObject.hpp"
#include <cassert>
#include <memory>

CProgramObject::~CProgramObject()
{
   if (isValid())
   {
      gl(glDetachShader, mObject, mVertexShader->mObject);
      gl(glDetachShader, mObject, mFragmentShader->mObject);
      gl(glDeleteProgram, mObject);
      Log::msg("destroying program ", mObject);
   }
}

TProgramObjectPtr CProgramObject::create(const std::string& vertexSrc, const std::string& fragmentSrc)
{
   auto p = std::make_unique<CProgramObject>();
   if (!(p->mVertexShader = CShader::createVertexShader(vertexSrc)) ||
       !(p->mFragmentShader = CShader::createFragmentShader(fragmentSrc)))
   {
      return 0;
   }
   p->mObject = gl(glCreateProgram);
   Log::msg("creating program ", p->mObject);
   gl(glAttachShader, p->mObject, p->mVertexShader->mObject);
   gl(glAttachShader, p->mObject, p->mFragmentShader->mObject);
   gl(glLinkProgram,  p->mObject);
   GLint linked;
   gl(glGetProgramiv, p->mObject, GL_LINK_STATUS, &linked);
   if (!linked)
   {
      GLint logLength = 0;
      gl(glGetProgramiv, p->mObject, GL_INFO_LOG_LENGTH, &logLength);
      if (logLength > 0)
      {
         auto str = std::make_unique<GLchar[]>(logLength);
         GLsizei len = 0;
         gl(glGetProgramInfoLog, p->mObject, logLength, &len, str.get());
         Log::err("program link failed: ", str.get());
      }
      return 0;
   }
   return p;
}
