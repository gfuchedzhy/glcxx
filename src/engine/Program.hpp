/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "ProgramObject.hpp"
#include "ProgramInput.hpp"
#include <tuple>

template<typename... TProgramInput>
class TProgram : public CProgramObject, public TProgramInputs<TProgramInput...>, public CIndexBufferProgramInput
{
   public:
      //todo
      using TProgramInputs<TProgramInput...>::set;
      using CIndexBufferProgramInput::set;

      /// @brief ctstring containing glsl declarations of all program inputs
      using tVertexShaderDeclaration = ct::string_cat<typename TProgramInput::template tVertexShaderDeclaration<typename TProgramInput::tName>...>;
      using tFragmentShaderDeclaration = ct::string_cat<typename TProgramInput::template tFragmentShaderDeclaration<typename TProgramInput::tName>...>;

      /// @brief constructor
      TProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
         : CProgramObject(tVertexShaderDeclaration::chars + vertexSrc, tFragmentShaderDeclaration::chars + fragmentSrc)
         , TProgramInputs<TProgramInput...>(mObject)
      {}

      /// @brief @see CProgramObject::select
      void select() const
      {
         CProgramObject::select();
         swallow(TProgramInput::select());
         CIndexBufferProgramInput::select();
      }
};

#endif // ENGINE_PROGRAM_HPP
