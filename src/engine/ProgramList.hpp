/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMLIST_HPP
#define ENGINE_PROGRAMLIST_HPP

#include "Program.hpp"

/// @brief program list, implements lazy program creation and program selection
/// on get method call
/// @tparam list of program names
template<typename... TProgramName>
class TProgramList
{
      /// @brief disabled stuff
      TProgramList(const TProgramList&) = delete;
      TProgramList& operator=(const TProgramList& other) = delete;
   public:
      TProgramList() = default;

      /// @brief searches given program by name in compile time, creates it if
      /// it is not created, deselects previously selected program, selecs new
      /// one
      template<typename TName>
      auto get()
      {
         constexpr int index = ct::TTypeIndexInPack<TName, TProgramName...>::value;
         static_assert(-1 != index, "program name not found");
         auto&& p = std::get<index>(mPrograms);
         if (!p)
            p = make_program(TName{});
         if (p != mCurrent)
         {
            Log::msg("selecting program: ", TName::chars);
            if(mCurrent)
               mCurrent->deselect();
            mCurrent = p;
            mCurrent->select();
         }
         return p;
      }

   private:
      /// @brief program list
      std::tuple<decltype(make_program(TProgramName{}))...> mPrograms;

      /// @brief current program ptr
      TProgramPtr mCurrent;
};

#endif // ENGINE_PROGRAMLIST_HPP
