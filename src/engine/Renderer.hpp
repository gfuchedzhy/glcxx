/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_RENDERER_HPP
#define ENGINE_RENDERER_HPP

#include "Program.hpp"
#include <SFML/Window.hpp>

/// @brief renderer, implements program creation and program selection
/// @tparam list of program names
template<typename... TProgramName>
class TRenderer
{
      /// @brief disabled stuff
      TRenderer(const TRenderer&) = delete;
      TRenderer& operator=(const TRenderer& other) = delete;

      /// @brief base program name, e.g. shaded-tex-nmap -> shaded
      template<typename TFullName>
      using base_name = ct::string_sub<TFullName, 0, ct::string_find<TFullName, cts("-")>::value>;

      /// @brief program type by its name
      template<typename TName>
      using program_type = TProgram<TName, decltype(progInputDef(TName{}))>;

   public:
      /// @brief initializes all programs of this rendererq
      TRenderer()
      {
         mPrograms = std::make_tuple(std::make_unique<program_type<TProgramName>>(programSrc<base_name<TProgramName>>())...);
      }

      /// @brief searches given program by name in compile time, creates it if
      /// it is not created, deselects previous program, selects requested one
      /// and returns it, @see @notes for TRenderer::get
      /// @note it is disallowed to cache returned value, as selection of
      /// another program will invalidate it, to discourage caching this
      /// function returns raw pointer
      template<typename TName>
      auto& get()
      {
         constexpr int index = ct::type_index_in_pack<TName, TProgramName...>::value;
         static_assert(-1 != index, "program name not found");
         auto p = std::get<index>(mPrograms).get();
         if (static_cast<CProgramBase*>(p) != mCurrent)
         {
            p->select();
            mCurrent = p;
         }
         return *p;
      }

   private:
      /// @brief context necessaty to allow early program creation
      sf::Context mContext;

      /// @brief program list
      std::tuple<std::unique_ptr<program_type<TProgramName>>...> mPrograms;

      /// @brief current program ptr
      CProgramBase* mCurrent;
};

#endif // ENGINE_RENDERER_HPP
