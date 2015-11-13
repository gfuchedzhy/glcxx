/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_RENDERER_HPP
#define ENGINE_RENDERER_HPP

#include "Program.hpp"

/// @brief renderer, implements lazy program creation and program selection
/// @tparam list of program names
template<typename... TProgramName>
class TRenderer
{
      /// @brief disabled stuff
      TRenderer(const TRenderer&) = delete;
      TRenderer& operator=(const TRenderer& other) = delete;
   public:
      TRenderer() = default;

      /// @brief searches given program by name in compile time, creates it if
      /// it is not created
      /// @note it is disallowed to cache returned value, use it locally only,
      /// to discourage caching this function returns raw pointer
      /// @note you can use result of this call only to update program input, to
      /// actually draw something you will have to select program
      template<typename TName>
      inline auto get()
      {
         constexpr int index = ct::TTypeIndexInPack<TName, TProgramName...>::value;
         static_assert(-1 != index, "program name not found");
         auto&& p = std::get<index>(mPrograms);
         if (!p)
            p = make_program(TName{});
         return p.get();
      }

      /// @brief searches given program by name in compile time, creates it if
      /// it is not created, deselects previous program, selects requested one
      /// and returns it, @see @notes for TRenderer::get
      template<typename TName>
      auto getAndSelect()
      {
         auto p = get<TName>();
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
      CProgramObject* mCurrent;
};

#endif // ENGINE_RENDERER_HPP
