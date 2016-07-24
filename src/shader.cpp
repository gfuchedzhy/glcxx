/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include <memory>
#include "shader.hpp"

//review this file

shader::shader(const std::string& src, const GLenum shaderType)
{
   const char* strType
      = (GL_VERTEX_SHADER == shaderType) ? "VERTEX"
      : (GL_GEOMETRY_SHADER == shaderType) ? "GEOMETRY" : "FRAGMENT";
   // Log::msg("creating ", strType, " shader");
   mObject = gl(glCreateShader, shaderType);
   std::string fullSrc("#version 330\n"
                       "#define ");
   fullSrc += strType;
   fullSrc += '\n';
   fullSrc += src;
// #ifdef GL_LOG_ALL
//    Log::msg(fullSrc);
// #endif
   const GLint length = fullSrc.length();
   const GLchar* source = fullSrc.c_str();
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
         // Log::err("shader compile failed: ", str.get());
      }
      this->~shader();
      throw std::runtime_error{"shader compilation failed"};
   }
}

shader::~shader()
{
   gl(glDeleteShader, mObject);
}
