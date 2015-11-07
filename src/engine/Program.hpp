/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "Shader.hpp"

class CProgram;

/// @brief program ptr
typedef std::unique_ptr<CProgram> TProgramPtr;

class CProgram
{
   public:
      /// @brief factory method returning program
      static TProgramPtr create(const std::string& vertexSrc, const std::string& fragmentSrc);

      /// @brief constuctor
      CProgram() = default;

      /// @brief destructor
      ~CProgram();

      /// @brief @returns true if program was created and compiled fine
      bool isValid() const;

      /// @brief make this program current
      void bind() const;

   private:
      /// @brief disabled stuff
      CProgram(const CProgram&) = delete;
      CProgram& operator=(const CProgram& other) = delete;

      /// @brief program object id
      GLuint mObject = 0;

      /// @brief shaders
      TShaderPtr mVertexShader;
      TShaderPtr mFragmentShader;
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
