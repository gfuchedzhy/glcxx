/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "shader.hpp"
#include <memory>
#include "except.hpp"

glcxx::shader_base::shader_base(GLenum shader_type)
   : mObject(glcxx_gl(glCreateShader, shader_type))
{}

glcxx::shader_base::~shader_base()
{
   glcxx_gl(glDeleteShader, mObject);
}

namespace
{
   const char* shader_type_to_str(const GLenum shader_type)
   {
      return GL_VERTEX_SHADER == shader_type ? "VERTEX" :
         GL_GEOMETRY_SHADER == shader_type ? "GEOMETRY" : "FRAGMENT";
   }
}

glcxx::shader::shader(const std::string& glsl_version, std::string src, GLuint program, const GLenum shader_type)
   : shader_base(shader_type)
   , mProgramObject(program)
{
   src = glsl_version + "\n#define " + shader_type_to_str(shader_type) + "\n" + src;
   const GLint length = src.length();
   const GLchar* source = src.c_str();
   glcxx_gl(glShaderSource, mObject, 1, &source, &length);
   glcxx_gl(glCompileShader, mObject);
   GLint compiled;
   glcxx_gl(glGetShaderiv, mObject, GL_COMPILE_STATUS, &compiled);
   if (!compiled)
   {
      GLint log_length = 0;
      glcxx_gl(glGetShaderiv, mObject, GL_INFO_LOG_LENGTH, &log_length);
      auto error_msg = std::make_unique<GLchar[]>(log_length);
      if (log_length > 0)
      {
         GLsizei len = 0;
         glcxx_gl(glGetShaderInfoLog, mObject, log_length, &len, error_msg.get());
      }
      throw shader_compilation_error(std::string(shader_type_to_str(shader_type)) + " shader complilation failed: " + error_msg.get());
   }
   glcxx_gl(glAttachShader, mProgramObject, mObject);
}

glcxx::shader::~shader()
{
   glcxx_gl(glDetachShader, mProgramObject, mObject);
}
