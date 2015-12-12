/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "ProgramObject.hpp"
#include <tuple>

// todo
template<typename T, typename... TRest>
struct TProgramInputAggregator : public T, public TProgramInputAggregator<TRest...>
{
      using tBase = TProgramInputAggregator<TRest...>;
      TProgramInputAggregator(GLuint program)
         : T(program)
         , tBase(program)
      {}

      using T::set;
      using TProgramInputAggregator<TRest...>::set;
};

template<typename T>
struct TProgramInputAggregator<T> : public T
{
      using T::T;
      using T::set;
};

template<typename... TProgramInput>
class TProgram : public CProgramObject, public TProgramInputAggregator<TProgramInput...>
{
   public:
      /// @brief ctstring containing glsl declarations of all program input todo
      using tVertexShaderDeclaration = ct::string_cat<typename TProgramInput::template tVertexShaderDeclaration<typename TProgramInput::tName>...>;
      using tFragmentShaderDeclaration = ct::string_cat<typename TProgramInput::template tFragmentShaderDeclaration<typename TProgramInput::tName>...>;

      /// @brief constructor
      TProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
         : CProgramObject(tVertexShaderDeclaration::chars + vertexSrc, tFragmentShaderDeclaration::chars + fragmentSrc)
         , TProgramInputAggregator<TProgramInput...>(mObject)
      {}

      /// @brief @see CProgramObject::select
      void select() const
      {
         CProgramObject::select();
         swallow(TProgramInput::select());
      }
};

#endif // ENGINE_PROGRAM_HPP
