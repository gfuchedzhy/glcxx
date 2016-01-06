/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Program.hpp"

namespace
{
   // macros to declare and access in/out variables between shaders
   static const char glslHelperMacros[] = R"(
#ifdef VERTEX
#define DECL_VERT_GEOM(type, name) out type vg##name
#elif defined GEOMETRY
#define DECL_VERT_GEOM(type, name) in type vg##name[]
#else
#define DECL_VERT_GEOM(type, name) void
#endif

#ifdef GEOMETRY
#define DECL_GEOM_FRAG(type, name) out type gf##name
#elif defined FRAGMENT
#define DECL_GEOM_FRAG(type, name) in type gf##name
#else
#define DECL_GEOM_FRAG(type, name) void
#endif

#ifdef VERTEX
#define DECL_VERT_FRAG(type, name) out type vf##name
#elif defined FRAGMENT
#define DECL_VERT_FRAG(type, name) in type vf##name
#else
#define DECL_VERT_FRAG(type, name) void
#endif

#ifdef VERTEX
#define out(name) vg##name
#elif defined GEOMETRY
#define in(name) vg##name
#define out(name) gf##name
#elif defined FRAGMENT
#define in(name) gf##name
#endif
#line 0
)";
}

// concat declarations, helper code and sources, then pass them to delegate constructor
CProgramBase::CProgramBase(const char* declarations, const char* src, bool hasGeometryShader)
   : CProgramBase(std::string(declarations) + glslHelperMacros + src, hasGeometryShader)
{}

CProgramBase::CProgramBase(const std::string& src, bool hasGeometryShader)
   : mVertexShader("#define VERTEX\n" + src, GL_VERTEX_SHADER)
   , mFragmentShader("#define FRAGMENT\n" + src, GL_FRAGMENT_SHADER)
   , mGeometryShader(hasGeometryShader ? std::make_unique<CShader>("#define GEOMETRY\n" + src, GL_GEOMETRY_SHADER) : nullptr)
{
   mObject = gl(glCreateProgram);
   Log::msg("creating program ", mObject);
   gl(glAttachShader, mObject, mVertexShader.mObject);
   gl(glAttachShader, mObject, mFragmentShader.mObject);
   if (mGeometryShader)
      gl(glAttachShader, mObject, mGeometryShader->mObject);
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
      this->~CProgramBase();
      throw std::runtime_error{"program linking failed"};
   }
}

CProgramBase::~CProgramBase()
{
   Log::msg("destroying program ", mObject);
   gl(glDetachShader, mObject, mVertexShader.mObject);
   gl(glDetachShader, mObject, mFragmentShader.mObject);
   if (mGeometryShader)
      gl(glDetachShader, mObject, mGeometryShader->mObject);
   gl(glDeleteProgram, mObject);
}
