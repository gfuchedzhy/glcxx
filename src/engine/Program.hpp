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
      using tDeclaration = ct::string_cat<typename TProgramInput::tData::tDeclaration...>;

      /// @brief tuple containing all program input, needed for type indexing
      using tProgramInputTypesTuple = std::tuple<TProgramInput...>;

      /// @brief tuple containing ptrs of all program input
      using tProgramInputTuple = std::tuple<std::shared_ptr<TProgramInput>...>;

      /// @brief input number
      static constexpr size_t inputNum = sizeof...(TProgramInput);

      /// @brief constructor
      TProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
         : CProgramObject(tDeclaration::chars + vertexSrc, fragmentSrc)
      {
         retrieveLocations(std::make_index_sequence<inputNum>{});
      }

      /// @brief queries and saves locations for all program input
      template<size_t... I>
      void retrieveLocations(std::index_sequence<I...>)
      {
         swallow(std::get<I>(mLocations) = TProgramInput::tData::getLocations(mObject));
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
         auto& current = std::get<I>(mProgramInput);
         if (input != current)
         {
            if (isSelected())
            {
               // detach old input, attach new one
               attach<I>(false);
               current = input;
               attach<I>(true);
            }
            else
            {
               // if attached and detach is not scheduled yet - schedule detach
               auto& forDetach = std::get<I>(mProgramInputDetach);
               if (mAttached[I] && !forDetach)
                  forDetach = current;
               current = input;
            }
         }
      }

      /// @brief @see CProgramObject::select
      void select() override
      {
         CProgramObject::select();
         doSelect(std::make_index_sequence<inputNum>{});
      }

      /// @brief select impl
      template<size_t... I>
      void doSelect(std::index_sequence<I...>)
      {
         swallow(doSelectSingle<I>());
      }

      /// @brief select impl for single program input
      template<size_t I>
      void doSelectSingle()
      {
         if (auto forDetach = std::get<I>(mProgramInputDetach))
         {
            forDetach->detach(std::get<I>(mLocations));
            forDetach = nullptr;
         }
         if (auto input = std::get<I>(mProgramInput))
         {
            input->attach(std::get<I>(mLocations));
            mAttached[I] = true;
         }
      }

   private:
      /// @brief if @param state true attach, otherwise detach
      template<size_t I>
      void attach(const bool state)
      {
         if (auto& input = std::get<I>(mProgramInput))
         {
            const auto& locations = std::get<I>(mLocations);
            state ? input->attach(locations) : input->detach(locations);
            mAttached[I] = state;
         }
      }

      /// @brief locations for all program input
      std::tuple<typename TProgramInput::tData::tLocations...> mLocations;

      /// @brief program input associated with program
      tProgramInputTuple mProgramInput;

      /// @brief program input scheduled for detach
      tProgramInputTuple mProgramInputDetach;

      /// @brief true if program input is currently attached
      std::array<bool, inputNum> mAttached = {}; // false
};

#endif // ENGINE_PROGRAM_HPP
