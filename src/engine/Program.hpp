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
      /// @brief ctstring containing glsl declaration of all program input
      using tDeclaration = ct::string_cat<typename TProgramInput::tTypeTraits::tDeclaration...>;

      /// @brief tuple containing all program input
      using tProgramInputTuple = std::tuple<TProgramInput...>;

      /// @brief input number
      static constexpr size_t inputNum = sizeof...(TProgramInput);

      /// @brief constructor
      TProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
         : CProgramObject(tDeclaration::chars + vertexSrc, fragmentSrc)
         , mProgramInput(TProgramInput(mObject)...)
      {
      }

      /// @brief get index of named program input at compile time
      template<typename TName, int index = sizeof...(TProgramInput) - 1>
         static constexpr int indexByName(TName, std::integral_constant<int, index> = std::integral_constant<int, index>{})
      {
         return std::is_same<TName, typename std::tuple_element<index, tProgramInputTuple>::type::tTypeTraits::tName>::value ? index :
            indexByName(TName{}, std::integral_constant<int, index-1>{});
      }

      /// @brief terminator for compile time program input index calculator
      template<typename TName>
      static constexpr int indexByName(TName, std::integral_constant<int, -1>)
      {
         return -1;
      }

      /// @brief sets program input to a program
      template<typename TName, int I = indexByName(TName{})>
      void set(typename std::tuple_element<I, tProgramInputTuple>::type::tValueType input)
      {
         std::get<I>(mProgramInput).set(input, isSelected());
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
