/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Program.hpp"
#include <cassert>
#include <memory>

CProgram::CProgram(TShaderPtr vertex, TShaderPtr fragment)
   : mObject(0)
   , mVertexShader(vertex)
   , mFragmentShader(fragment)
{
}

CProgram::~CProgram()
{
   if (isValid())
   {
      Log::msg("destroying program ", mObject);
      gl(glDetachShader, mObject, mVertexShader->mObject);
      gl(glDetachShader, mObject, mFragmentShader->mObject);
      gl(glDeleteProgram, mObject);
   }
}

TProgramPtr CProgram::create(TShaderPtr vertex, TShaderPtr fragment)
{
   if (vertex && vertex->isValid() && fragment && fragment->isValid())
   {
      Log::msg("creating program");
      auto p = std::make_unique<CProgram>(vertex, fragment);
      p->mObject = gl(glCreateProgram);
      gl(glAttachShader, p->mObject, vertex->mObject);
      gl(glAttachShader, p->mObject, fragment->mObject);
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
   return 0;
}
