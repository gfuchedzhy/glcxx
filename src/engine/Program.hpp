/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "ProgramObject.hpp"
#include <tuple>

template<typename... TProgramInput>
class TProgram
{
   public:
      /// @brief ctstring containing glsl declaration of all program input
      using tDeclaration = ct::string_cat<typename TProgramInput::tData::tDeclaration...>;

      /// @brief input number
      static constexpr size_t inputNum = sizeof...(TProgramInput);

      /// @brief constructor
      TProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
         : mObject(tDeclaration::chars + vertexSrc, fragmentSrc)
      {
         retrieveLocations(std::make_index_sequence<inputNum>{});
         Log::msg(mLocations);
      }

      template<size_t... I>
      void retrieveLocations(std::index_sequence<I...>)
      {
         swallow(std::get<I>(mLocations) = TProgramInput::tData::getLocations(mObject.mObject));
      }

   private:
      /// @brief program object
      CProgramObject mObject;

      /// @brief locations for all program input
      std::tuple<typename TProgramInput::tData::tLocations...> mLocations;
};

#endif // ENGINE_PROGRAM_HPP
