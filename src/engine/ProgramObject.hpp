/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMOBJECT_HPP
#define ENGINE_PROGRAMOBJECT_HPP

#include "Shader.hpp"

/// @brief resource holder for opengl program object
class CProgramObject
{
   public:
      /// @brief constuctor
      CProgramObject(const std::string& vertexSrc, const std::string& fragmentSrc);

      /// @brief destructor
      ~CProgramObject();

      /// @brief make this program object current
      void bind() const;

   private:
      /// @brief disabled stuff
      CProgramObject(const CProgramObject&) = delete;
      CProgramObject& operator=(const CProgramObject& other) = delete;

      /// @brief program object id
      GLuint mObject = 0;

      /// @brief shaders
      CShader mVertexShader;
      CShader mFragmentShader;

      template<typename...> friend class TProgram;
};

inline void CProgramObject::bind() const
{
   gl(glUseProgram, mObject);
}

#endif // ENGINE_PROGRAMOBJECT_HPP
