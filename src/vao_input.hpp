/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_VAO_INPUT_HPP
#define GLCXX_SRC_VAO_INPUT_HPP

#include "vao.hpp"

// todo namespace
// namespace glcxx
// {
   /// @brief attrib_input contains traits only, actual input class is
   /// vao_input, which combines all attribs in single vao, attrib_input is used
   /// for program declarations
   template<typename TName, typename TAttribTraits>
   struct attrib_input {};

   /// @brief holds state of program's vao
   template<typename TAttribInputTuple> class vao_input;

   /// @brief specialization for tuple
   template<typename... TAttribName, typename... TAttribTraits>
   class vao_input<std::tuple<attrib_input<TAttribName, TAttribTraits>...>>
   {
         /// @brief return true if Tuple doesn't contain T
         template<typename T, typename Tuple> struct doesnt_contain
         {
               static constexpr bool value = !ct::tuple_contains<Tuple, T>::value;
         };

      public:
         /// @brief vao holds VBOs, which are always vertex shader inputs
         using decl_tag = tag::vertex;

         /// @brief ctstring containing glsl declaration of attributes
         using declaration = ct::string_cat<typename TAttribTraits::template declaration<TAttribName>...>;

         /// @brief constructor
         vao_input(const GLuint program)
            : mLocations{{glsl::get_attrib_loc(program, TAttribName::chars)...}}
            , mProgramID(program)
         {}

         /// @brief called after program was selected, nothing to do as vao gets
         /// passed to draw function
         void select() const
         {}

         /// @brief draw using index buffer from vao
         template<typename... TName, typename...TData>
         void draw_elements(const indexed_vao<ct::named_type<TName, TData>...>& vao) const
         {
            using vao_tuple = std::tuple<ct::named_type<TName, TData>...>;
            using required_vao_tuple = std::tuple<ct::named_type<TAttribName, typename TAttribTraits::data>...>;
            static_assert(!ct::tuple_any_of<required_vao_tuple, doesnt_contain, vao_tuple>::value, "not all or not matching type inputs for program was provided by given vao");

            vao.bind();
            // if was attached to different program or wasn't attached at all
            if (mProgramID != vao.mProgramID)
            {
               Log::msg("attaching vao to a program id ", mProgramID);
               vao.mProgramID = mProgramID;
               vao.template bind_buffer<cts("indices")>();
               swallow(vao.template bind_buffer<TAttribName>() &&
                       (TAttribTraits::attach(mLocations[ct::tuple_find<std::tuple<TAttribName...>, TAttribName>::value]), true));
            }
            vao.draw_elements();
         }

         /// @brief draw arrays
         template<typename...TName, typename...TData>
         void draw_arrays(const vao<ct::named_type<TName, TData>...>& vao, GLsizei size, GLenum mode) const
         {
            using vao_tuple = std::tuple<ct::named_type<TName, TData>...>;
            using required_vao_tuple =  std::tuple<ct::named_type<TAttribName, typename TAttribTraits::data>...>;
            static_assert(!ct::tuple_any_of<required_vao_tuple, doesnt_contain,vao_tuple>::value, "not all or not matching type inputs for program was provided by given vao");

            vao.bind();
            // if was attached to different program or wasn't attached at all
            if (mProgramID != vao.mProgramID)
            {
               Log::msg("attaching vao to a program id ", mProgramID);
               vao.mProgramID = mProgramID;
               swallow(vao.template bind_buffer<TAttribName>() &&
                    (TAttribTraits::attach(mLocations[ct::tuple_find<std::tuple<TAttribName...>, TAttribName>::value]), true));
            }
            gl(glDrawArrays, mode, 0, size);
         }

      private:
         /// @brief locations for all VBOs inside vao
         using tLocations = std::array<GLint, sizeof...(TAttribName)>;
         tLocations mLocations;

         /// @brief program id to check in runtime if input vao is ready to be
         /// bound for this program or it should be reattached for this program
         GLuint mProgramID;
   };
// }

#endif
