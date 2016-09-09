/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_VAO_INPUT_HPP
#define GLCXX_VAO_INPUT_HPP

#include "vao.hpp"

namespace glcxx
{
   /// @brief attrib_input contains traits only, actual input class is
   /// vao_input, which combines all attribs in single vao, attrib_input is used
   /// for program declarations
   template<typename Name, typename AttribTraits>
   struct attrib_input {};

   /// @brief holds state of program's vao
   template<typename AttribInputTuple> class vao_input;

   /// @brief specialization for tuple
   template<typename... AttribName, typename... AttribTraits>
   class vao_input<std::tuple<attrib_input<AttribName, AttribTraits>...>>
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
         using declaration = ct::string_cat<typename AttribTraits::template declaration<AttribName>...>;

         /// @brief constructor
         vao_input(const GLuint program)
            : _locations{{get_attrib_loc(program, AttribName::chars)...}}
            , _program_id(program)
         {}

         /// @brief called after program was selected, nothing to do as vao gets
         /// passed to draw function
         void select() const
         {}

         /// @brief draw using index buffer from vao
         template<typename... Name, typename... Data>
         void draw_elements(const indexed_vao<std::pair<Name, Data>...>& vao) const
         {
            using vao_tuple = std::tuple<std::pair<Name, Data>...>;
            using required_vao_tuple = std::tuple<std::pair<AttribName, typename AttribTraits::data>...>;
            static_assert(!ct::tuple_any_of<required_vao_tuple, doesnt_contain, vao_tuple>::value, "not all or not matching type inputs for program was provided by given vao");

            vao.bind();
            // if was attached to different program or wasn't attached at all
            if (_program_id != vao._program_id)
            {
               vao._program_id = _program_id;
               vao.template bind_buffer<cts("indices")>();
               glcxx_swallow(vao.template bind_buffer<AttribName>() &&
                             (AttribTraits::attach(_locations[ct::tuple_find<std::tuple<AttribName...>, AttribName>::value]), true));
            }
            vao.draw_elements();
         }

         /// @brief draw arrays
         template<typename... Name, typename... Data>
         void draw_arrays(const vao<std::pair<Name, Data>...>& vao, GLsizei size, GLenum mode) const
         {
            using vao_tuple = std::tuple<std::pair<Name, Data>...>;
            using required_vao_tuple =  std::tuple<std::pair<AttribName, typename AttribTraits::data>...>;
            static_assert(!ct::tuple_any_of<required_vao_tuple, doesnt_contain,vao_tuple>::value, "not all or not matching type inputs for program was provided by given vao");

            vao.bind();
            // if was attached to different program or wasn't attached at all
            if (_program_id != vao._program_id)
            {
               vao._program_id = _program_id;
               glcxx_swallow(vao.template bind_buffer<AttribName>() &&
                             (AttribTraits::attach(_locations[ct::tuple_find<std::tuple<AttribName...>, AttribName>::value]), true));
            }
            glcxx_gl(glDrawArrays, mode, 0, size);
         }

      private:
         /// @brief locations for all VBOs inside vao
         using tLocations = std::array<GLint, sizeof...(AttribName)>;
         tLocations _locations;

         /// @brief program id to check in runtime if input vao is ready to be
         /// bound for this program or it should be reattached for this program
         GLuint _program_id;
   };
}

#endif
