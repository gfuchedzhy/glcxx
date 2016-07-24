/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_RENDERER_HPP
#define GLCXX_SRC_RENDERER_HPP

#include "program.hpp"

//todo namespace
// namespace glcxx
// {
   /// @brief renderer, implements program creation and program selection
   /// @tparam list of program names
   template<typename... TProgramName>
   class renderer
   {
         /// @brief disabled stuff
         renderer(const renderer&) = delete;
         renderer& operator=(const renderer& other) = delete;

         /// @brief base program name, e.g. shaded-tex-nmap -> shaded
         template<typename TFullName>
         using base_name = ct::string_sub<TFullName, 0, ct::string_find<TFullName, cts("-")>::value>;

         /// @brief program type by its name
         template<typename TName>
         using program_type = program<TName, decltype(progInputDef(TName{}))>;

      public:
         /// @brief initializes all programs of this renderer
         renderer()
         {
            mPrograms = std::make_tuple(std::make_unique<program_type<TProgramName>>(programSrc<base_name<TProgramName>>())...);
         }

         /// @brief searches given program by name in compile time, deselects
         /// previous program, selects requested one and returns it
         template<typename TName>
         auto& get_program()
         {
            constexpr size_t index = ct::tuple_find<std::tuple<TProgramName...>, TName>::value;
            static_assert(sizeof...(TProgramName) != index, "program name not found");
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
         std::tuple<std::unique_ptr<program_type<TProgramName>>...> mPrograms;

         /// @brief current program index, points after last program by default
         size_t mCurrentProgramIndex = sizeof...(TProgramName);
   };
// }
#endif
