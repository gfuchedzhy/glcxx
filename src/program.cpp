/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "program.hpp"

glcxx::program_res_holder::program_res_holder()
   : mObject(glcxx_gl(glCreateProgram))
{}

glcxx::program_res_holder::~program_res_holder()
{
   glcxx_gl(glDeleteProgram, mObject);
}

glcxx::program_base::program_base(const char* name, const std::string& glsl_version, const std::string& src, bool has_geom_shader)
   try : program_res_holder()
       , mVertexShader(glsl_version + "\n#define VERTEX\n" + src, mObject, GL_VERTEX_SHADER)
       , mFragmentShader(glsl_version + "\n#define FRAGMENT\n" + src, mObject, GL_FRAGMENT_SHADER)
       , mGeometryShader(has_geom_shader ? std::make_unique<shader>(glsl_version + "\n#define GEOMETRY\n" + src, mObject, GL_GEOMETRY_SHADER) : nullptr)
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
      throw program_creation_error(name, std::string("program linking failed:\n") + error_msg.get());
   }
}
catch(const shader_compile_error& e)
{
   throw program_creation_error(name, e.what());
}

glcxx::program_creation_error::program_creation_error(const char* name, const std::string& msg)
   : std::runtime_error(std::string(name) +  + " program creation failed:\n" + msg)
{}
