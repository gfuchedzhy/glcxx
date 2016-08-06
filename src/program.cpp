/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "glcxx/program.hpp"

#include <algorithm>
#include "glcxx/except.hpp"

glcxx::program_res_holder::program_res_holder()
   : mObject(glcxx_gl(glCreateProgram))
{}

glcxx::program_res_holder::~program_res_holder()
{
   glcxx_gl(glDeleteProgram, mObject);
}

std::string glcxx::program_base::prepend_header_to_program(std::string name, const char* declarations, const std::string& src)
{
   std::string& result = name;
   std::transform(result.begin(), result.end(), result.begin(), ::toupper);
   result.erase(0, result.find('_'));
   size_t start_pos = 0;
   while((start_pos = result.find('_', start_pos)) != std::string::npos)
      result.replace(start_pos, 1, "\n#define ");
   result += declarations + src;
   return result;
}

glcxx::program_base::program_base(const std::string& glsl_version, const std::string& src, bool has_geom_shader)
   : program_res_holder()
   , mVertexShader(glsl_version, src, mObject, GL_VERTEX_SHADER)
   , mFragmentShader(glsl_version, src, mObject, GL_FRAGMENT_SHADER)
   , mGeometryShader(has_geom_shader ? std::make_unique<shader>(glsl_version, src, mObject, GL_GEOMETRY_SHADER) : nullptr)
{
   glcxx_gl(glLinkProgram, mObject);
   GLint linked;
   glcxx_gl(glGetProgramiv, mObject, GL_LINK_STATUS, &linked);
   if (!linked)
   {
      GLint log_length = 0;
      glcxx_gl(glGetProgramiv, mObject, GL_INFO_LOG_LENGTH, &log_length);
      auto error_msg = std::make_unique<GLchar[]>(log_length);
      if (log_length > 0)
      {
         GLsizei len = 0;
         glcxx_gl(glGetProgramInfoLog, mObject, log_length, &len, error_msg.get());
      }
      throw shader_linking_error(std::string("program linking failed:\n") + error_msg.get());
   }
}
