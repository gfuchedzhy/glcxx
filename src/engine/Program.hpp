/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "Shader.hpp"

class CProgram
{
   public:
      /// @brief factory method returning program
      static CProgram create(const std::string& vertexSrc, const std::string& fragmentSrc);

      /// @brief move constructor
      CProgram(CProgram&& other);

      /// @brief move assignment
      CProgram& operator=(CProgram&& other);

      /// @brief destructor
      ~CProgram();

      /// @brief @returns true if program was created and compiled fine
      bool isValid() const;

      /// @brief make this program current
      void bind() const;

   private:
      /// @brief disabled stuff
      CProgram() = default;
      CProgram(const CProgram&) = default;
      CProgram& operator=(const CProgram& other) = default;

      /// @brief release program associated with this object
      void destroy();

      GLuint mObject = 0;
      CShader mVertexShader;
      CShader mFragmentShader;
};

inline bool CProgram::isValid() const
{
   return mObject;
}

inline void CProgram::bind() const
{
   gl(glUseProgram, mObject);
}

#endif // ENGINE_PROGRAM_HPP
