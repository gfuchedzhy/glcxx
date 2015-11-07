/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "ProgramObject.hpp"
#include <memory>

CProgramObject::CProgramObject(const std::string& vertexSrc, const std::string& fragmentSrc)
   : mVertexShader(vertexSrc, GL_VERTEX_SHADER)
   , mFragmentShader(fragmentSrc, GL_FRAGMENT_SHADER)
{
   mObject = gl(glCreateProgram);
   Log::msg("creating program ", mObject);
   gl(glAttachShader, mObject, mVertexShader.mObject);
   gl(glAttachShader, mObject, mFragmentShader.mObject);
   gl(glLinkProgram,  mObject);
   GLint linked;
   gl(glGetProgramiv, mObject, GL_LINK_STATUS, &linked);
   if (!linked)
   {
      GLint logLength = 0;
      gl(glGetProgramiv, mObject, GL_INFO_LOG_LENGTH, &logLength);
      if (logLength > 0)
      {
         auto str = std::make_unique<GLchar[]>(logLength);
         GLsizei len = 0;
         gl(glGetProgramInfoLog, mObject, logLength, &len, str.get());
         Log::err("program link failed: ", str.get());
      }
      this->~CProgramObject();
      throw std::runtime_error{"program linking failed"};
   }
}

CProgramObject::~CProgramObject()
{
   Log::msg("destroying program ", mObject);
   gl(glDetachShader, mObject, mVertexShader.mObject);
   gl(glDetachShader, mObject, mFragmentShader.mObject);
   gl(glDeleteProgram, mObject);
}
