/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_RENDERER_HPP
#define GLCXX_SRC_RENDERER_HPP

#include "program.hpp"

namespace glcxx
{
   /// @brief returns program src by name, all shaders of any program use same
   /// src as their base program, implementation differences between derived
   /// shaders should be implemented using preprocessor, so client should
   /// provide specializations of this function for all base programs
   template<typename TName>
   const char* get_program_src();

   /// @brief renderer, implements program creation and program selection
   /// @tparam std::pairs of compile time programs
   template<typename... TNamedPrograms> class renderer;

   template<typename... TName, typename... TProgram>
   class renderer<std::pair<TName, TProgram>...>
   {
         /// @brief disabled stuff
         renderer(const renderer&) = delete;
         renderer& operator=(const renderer& other) = delete;

         /// @brief base program name, e.g. shaded_tex_nmap -> shaded
         template<typename TFullName>
         using base_name = ct::string_sub<TFullName, 0, ct::string_find<TFullName, cts("_")>::value>;

      public:
         /// @brief initializes all programs of this renderer
         renderer(const std::string& glsl_version, const std::string& common_decl)
         {
            mPrograms = std::make_tuple(
               std::make_unique<TProgram>(
                  TName::chars, glsl_version, common_decl + get_program_src<base_name<TName>>())...);
         }

         /// @brief searches given program by name in compile time, selects
         /// requested one and returns it
         template<typename TProgramName>
         auto& get_program()
         {
            constexpr size_t index = ct::tuple_find<std::tuple<TName...>, TProgramName>::value;
            static_assert(sizeof...(TProgram) != index, "program name not found");
            auto p = std::get<index>(mPrograms).get();
            if (index != mCurrentProgramIndex)
            {
               mCurrentProgramIndex = index;
               p->select();
            }
            return *p;
         }

      private:
         /// @brief program list, @todo unique_ptr can be removed without providing
         /// movability or copyability to programs, when emplace style tuple
         /// constructor would be available
         std::tuple<std::unique_ptr<TProgram>...> mPrograms;

         /// @brief current program index, points after last program by default
         size_t mCurrentProgramIndex = sizeof...(TProgram);
   };
}
#endif
