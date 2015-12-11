/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMOBJECT_HPP
#define ENGINE_PROGRAMOBJECT_HPP

#include "Shader.hpp"
#include <memory>

/// @brief resource holder for opengl program object
class CProgramObject
{
      /// @brief disabled stuff
      CProgramObject(const CProgramObject&) = delete;
      CProgramObject& operator=(const CProgramObject& other) = delete;
   public:
      /// @brief constuctor
      CProgramObject(const std::string& vertexSrc, const std::string& fragmentSrc);

      /// @brief destructor
      virtual ~CProgramObject();

      /// @brief selects program
      virtual void select() const;

   protected:
      /// @brief program object id
      GLuint mObject = 0;

   private:
      /// @brief shaders
      CShader mVertexShader;
      CShader mFragmentShader;
};

inline void CProgramObject::select() const
{
   gl(glUseProgram, mObject);
}

#endif // ENGINE_PROGRAMOBJECT_HPP
