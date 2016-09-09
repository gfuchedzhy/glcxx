/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "glcxx/program.hpp"

#include <algorithm>
#include "glcxx/except.hpp"

glcxx::program_res_holder::program_res_holder()
   : _object(glcxx_gl(glCreateProgram))
{}

glcxx::program_res_holder::~program_res_holder()
{
   glcxx_gl(glDeleteProgram, _object);
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
   , _vertex_shader(glsl_version, src, _object, GL_VERTEX_SHADER)
   , _fragment_shader(glsl_version, src, _object, GL_FRAGMENT_SHADER)
   , _geometry_shader(has_geom_shader ? std::make_unique<shader>(glsl_version, src, _object, GL_GEOMETRY_SHADER) : nullptr)
{
   glcxx_gl(glLinkProgram, _object);
   GLint linked;
   glcxx_gl(glGetProgramiv, _object, GL_LINK_STATUS, &linked);
   if (!linked)
   {
      GLint log_length = 0;
      glcxx_gl(glGetProgramiv, _object, GL_INFO_LOG_LENGTH, &log_length);
      auto error_msg = std::make_unique<GLchar[]>(log_length);
      if (log_length > 0)
      {
         GLsizei len = 0;
         glcxx_gl(glGetProgramInfoLog, _object, log_length, &len, error_msg.get());
      }
      throw shader_linking_error(std::string("program linking failed:\n") + error_msg.get());
   }
}
