/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include <memory>
#include "shader.hpp"

glcxx::shader_base::shader_base(GLenum shader_type)
   : mObject(gl(glCreateShader, shader_type))
{}

glcxx::shader_base::~shader_base()
{
   gl(glDeleteShader, mObject);
}

glcxx::shader::shader(const std::string& src, GLuint program, const GLenum shader_type)
   : shader_base(shader_type)
   , mProgramObject(program)
{
   const GLint length = src.length();
   const GLchar* source = src.c_str();
   gl(glShaderSource, mObject, 1, &source, &length);
   gl(glCompileShader, mObject);
   GLint compiled;
   gl(glGetShaderiv, mObject, GL_COMPILE_STATUS, &compiled);
   if (!compiled)
   {
      GLint log_length = 0;
      gl(glGetShaderiv, mObject, GL_INFO_LOG_LENGTH, &log_length);
      auto error_msg = std::make_unique<GLchar[]>(log_length);
      if (log_length > 0)
      {
         GLsizei len = 0;
         gl(glGetShaderInfoLog, mObject, log_length, &len, error_msg.get());
      }
      throw shader_compile_error(std::string(error_msg.get()) + "full source:\n" + src);
   }
   gl(glAttachShader, mProgramObject, mObject);
}

glcxx::shader::~shader()
{
   gl(glDetachShader, mProgramObject, mObject);
}

glcxx::shader_compile_error::shader_compile_error(const std::string& msg)
   : std::runtime_error(std::string("shader compilation failed:\n") + msg)
{}
