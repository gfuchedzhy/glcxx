/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMOBJECT_HPP
#define ENGINE_PROGRAMOBJECT_HPP

#include "Shader.hpp"
#include <memory>

class CProgramObject;

/// @brief program ptr
using TProgramPtr = std::shared_ptr<CProgramObject>;

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

      /// @brief select program
      static void select(TProgramPtr p);

   protected:
      /// @brief make this program object current
      virtual void select();

      /// @brief returns true if current program selected
      bool isSelected() const;

      /// @brief current program ptr
      static TProgramPtr current;

      /// @brief program object id
      GLuint mObject = 0;

   private:
      /// @brief shaders
      CShader mVertexShader;
      CShader mFragmentShader;
};

inline void CProgramObject::select(TProgramPtr p)
{
   if (p != current)
   {
      current = p;
      if (current)
         current->select();
   }
}

inline void CProgramObject::select()
{
   gl(glUseProgram, mObject);
}

inline bool CProgramObject::isSelected() const
{
   return current.get() == this;
}

#endif // ENGINE_PROGRAMOBJECT_HPP
