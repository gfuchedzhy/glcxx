/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "ProgramObject.hpp"
#include <tuple>

template<typename... TProgramInput>
class TProgram : public CProgramObject
{
   public:
      /// @brief ctstring containing glsl declarations of all program input
      using tVertexShaderDeclaration = ct::string_cat<typename TProgramInput::tVertexShaderDeclaration...>;
      using tFragmentShaderDeclaration = ct::string_cat<typename TProgramInput::tFragmentShaderDeclaration...>;

      /// @brief tuple containing all program input
      using tProgramInputTuple = std::tuple<TProgramInput...>;

      /// @brief input number
      static constexpr size_t inputNum = sizeof...(TProgramInput);

      /// @brief constructor
      TProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
         : CProgramObject(tVertexShaderDeclaration::chars + vertexSrc, tFragmentShaderDeclaration::chars + fragmentSrc)
         , mProgramInput(TProgramInput(mObject)...)
      {
      }

      /// @brief sets program input to a program
      template<typename TName, int I = ct::TTupleTraits<tProgramInputTuple>::indexByName(TName{})>
      void set(typename std::tuple_element<I, tProgramInputTuple>::type::tValueType input)
      {
         std::get<I>(mProgramInput).set(input, mIsSelected);
      }

      /// @brief @see CProgramObject::select
      void select() override
      {
         CProgramObject::select();
         doSelect(std::make_index_sequence<inputNum>{});
      }

   private:
      /// @brief program input associated with program
      tProgramInputTuple mProgramInput;

   private: // impl
      /// @brief select impl
      template<size_t... I>
      void doSelect(std::index_sequence<I...>)
      {
         swallow(std::get<I>(mProgramInput).select());
      }
};

#endif // ENGINE_PROGRAM_HPP
