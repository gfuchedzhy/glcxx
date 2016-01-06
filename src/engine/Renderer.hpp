/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
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

      /// @brief return true if program input requires geometry shader, either
      /// it has input for geometry shader, or marked explicitly with
      /// tag::geometry
      /// @note dummy type because full specializations are not allowed in class
      template<typename T, typename Dummy = void>
      struct is_geom_impl
      {
            static constexpr bool value = (0 != T::tGeometryShaderDeclaration::length);
      };
      template<typename Dummy> struct is_geom_impl<tag::geometry, Dummy> { static constexpr bool value = true; };
      template<typename T> struct is_geom : is_geom_impl<T> {};

      /// @brief program type by its name
      template<typename TName>
      using program_type = TProgram<TName,
                                    ct::tuple_any_of<is_geom, decltype(progInputDef(TName{}))>::value, // has geometry shader?
                                    ct::tuple_strip<tag::geometry, decltype(progInputDef(TName{}))>>; // remove tag

   public:
      /// @brief initializes all programs of this rendererq
      TRenderer()
      {
         Log::msg("openGL ", mContextSettings.majorVersion, '.', mContextSettings.minorVersion,  " version loaded");
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
         constexpr size_t index = ct::tuple_find<TName, std::tuple<TProgramName...>>::value;
         static_assert(sizeof...(TProgramName) != index, "program name not found");
         auto p = std::get<index>(mPrograms).get();
         if (static_cast<CProgramBase*>(p) != mCurrent)
         {
            p->select();
            mCurrent = p;
         }
         return *p;
      }

      /// @brief return context settings
      auto& contextSettings() const
      {
         return mContextSettings;
      }

   private:
      /// @brief context settings
      sf::ContextSettings mContextSettings{24, 0, 0, 3, 3, sf::ContextSettings::Core};

      /// @brief context necessety to allow early program creation
      sf::Context mContext{mContextSettings, 1, 1};

      /// @brief program list
      std::tuple<std::unique_ptr<program_type<TProgramName>>...> mPrograms;

      /// @brief current program ptr
      CProgramBase* mCurrent;
};

#endif // ENGINE_RENDERER_HPP
