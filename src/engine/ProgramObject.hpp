/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMOBJECT_HPP
#define ENGINE_PROGRAMOBJECT_HPP

#include "Shader.hpp"

class CProgramObject;

/// @brief program object ptr
typedef std::unique_ptr<CProgramObject> TProgramObjectPtr;

class CProgramObject
{
   public:
      /// @brief factory method returning program object
      static TProgramObjectPtr create(const std::string& vertexSrc, const std::string& fragmentSrc);

      /// @brief constuctor
      CProgramObject() = default;

      /// @brief destructor
      ~CProgramObject();

      /// @brief @returns true if program object was created and compiled fine
      bool isValid() const;

      /// @brief make this program object current
      void bind() const;

   private:
      /// @brief disabled stuff
      CProgramObject(const CProgramObject&) = delete;
      CProgramObject& operator=(const CProgramObject& other) = delete;

      /// @brief program object id
      GLuint mObject = 0;

      /// @brief shaders
      TShaderPtr mVertexShader;
      TShaderPtr mFragmentShader;
};

inline bool CProgramObject::isValid() const
{
   return mObject;
}

inline void CProgramObject::bind() const
{
   gl(glUseProgram, mObject);
}

#endif // ENGINE_PROGRAMOBJECT_HPP
