// Copyright 2015, 2016 Grygoriy Fuchedzhy
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "glcxx/program.hpp"

#include <algorithm>
#include "glcxx/except.hpp"

glcxx::program_res_holder::program_res_holder()
   : _object(glCreateProgram())
{}

glcxx::program_res_holder::~program_res_holder()
{
   glDeleteProgram(_object);
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
   glLinkProgram(_object);
   GLint linked;
   glGetProgramiv(_object, GL_LINK_STATUS, &linked);
   if (!linked)
   {
      GLint log_length = 0;
      glGetProgramiv(_object, GL_INFO_LOG_LENGTH, &log_length);
      auto error_msg = std::make_unique<GLchar[]>(log_length);
      if (log_length > 0)
      {
         GLsizei len = 0;
         glGetProgramInfoLog(_object, log_length, &len, error_msg.get());
      }
      throw shader_linking_error(std::string("program linking failed:\n") + error_msg.get());
   }

   // select created program so that uniform inputs could attach default
   // constructed values
   select();
}
