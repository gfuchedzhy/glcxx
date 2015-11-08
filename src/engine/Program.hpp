/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "ProgramObject.hpp"
#include <tuple>
#include <memory>

template<typename... TProgramInput>
class TProgram
{
   public:
      /// @brief ctstring containing glsl declaration of all program input
      using tDeclaration = ct::string_cat<typename TProgramInput::tData::tDeclaration...>;

      /// @brief tuple containing all program input, needed for type indexing
      using tProgramInputTypesTuple = std::tuple<TProgramInput...>;

      /// @brief tuple containing ptrs of all program input
      using tProgramInputTuple = std::tuple<std::shared_ptr<TProgramInput>...>;

      /// @brief input number
      static constexpr size_t inputNum = sizeof...(TProgramInput);

      /// @brief constructor
      TProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
         : mObject(tDeclaration::chars + vertexSrc, fragmentSrc)
      {
         retrieveLocations(std::make_index_sequence<inputNum>{});
         Log::msg(mLocations);
      }

      /// @brief queries and saves locations for all program input
      template<size_t... I>
      void retrieveLocations(std::index_sequence<I...>)
      {
         swallow(std::get<I>(mLocations) = TProgramInput::tData::getLocations(mObject.mObject));
      }

      /// @brief get index of named program input at compile time
      template<typename TName, int index = sizeof...(TProgramInput) - 1>
         static constexpr int indexByName(TName, std::integral_constant<int, index> = std::integral_constant<int, index>{})
      {
         return std::is_same<TName, typename std::tuple_element<index, tProgramInputTypesTuple>::type::tData::tName>::value ? index :
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
      void set(typename std::tuple_element<I, tProgramInputTuple>::type input)
      {
         std::get<I>(mProgramInput) = input;
      }

   private:
      /// @brief program object
      CProgramObject mObject;

      /// @brief locations for all program input
      std::tuple<typename TProgramInput::tData::tLocations...> mLocations;

      /// @brief program input associated with program
      tProgramInputTuple mProgramInput;
};

#endif // ENGINE_PROGRAM_HPP
