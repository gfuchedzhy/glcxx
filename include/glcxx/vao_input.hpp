// Copyright 2016 Grygoriy Fuchedzhy
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GLCXX_VAO_INPUT_HPP
#define GLCXX_VAO_INPUT_HPP

#include "glcxx/vao.hpp"

namespace glcxx
{
   /// @brief vao_input contains traits only, actual input class is
   /// vao_input_impl, which combines all attributes in single vao, vao_input is
   /// used for program declarations only
   template<typename Name, typename AttribTraits>
   struct vao_input {};

   /// @brief holds state of program's vao
   template<typename AttribInputTuple> class vao_input_impl;

   /// @brief specialization for tuple
   template<typename... AttribName, typename... AttribTraits>
   class vao_input_impl<std::tuple<vao_input<AttribName, AttribTraits>...>>
   {
         /// @brief true if no inputs for vao
         static constexpr bool is_empty = 0 == sizeof...(AttribTraits);

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
         vao_input_impl(const GLuint program)
            : _locations{{get_attrib_loc(program, AttribName::chars)...}}
            , _program_id(program)
         {}

         /// @brief called after program was selected, nothing to do as vao gets
         /// passed to draw function
         void select() const
         {}

         /// @brief draw using index buffer from vao
         template<typename... T>
         void draw_elements(const indexed_vao<T...>& vao) const
         {
            bind(vao);
            vao.draw_elements();
            vao_base::unbind();
         }

         /// @brief draw arrays
         template<typename... T>
         void draw_arrays(const vao<T...>& vao, GLsizei size, GLenum mode) const
         {
            bind(vao);
            glDrawArrays(mode, 0, size);
            vao_base::unbind();
         }

         /// @brief if vao_input is empty, allow drawing without vao, with just
         /// index buffer
         template<typename Dummy = int> // to enable sfinae
         void draw_elements(const index_buffer& ib,
                            typename std::enable_if<is_empty, Dummy>::type dummy = 0) const
         {
            ib.bind();
            ib.draw();
         }

         /// @brief if vao_input is empty, allow drawing without vao at all
         template<typename Dummy = int> // to enable sfinae
         void draw_arrays(GLsizei size, GLenum mode,
                          typename std::enable_if<is_empty, Dummy>::type dummy = 0) const
         {
            glDrawArrays(mode, 0, size);
         }

      private:
         /// @brief bind indexed vao to current program
         template<typename... Name, typename... Data>
         void bind(const indexed_vao<std::pair<Name, Data>...>& vao) const
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
         }

         /// @brief bind non indexed vao to current program
         template<typename... Name, typename... Data>
         void bind(const vao<std::pair<Name, Data>...>& vao) const
         {
            using vao_tuple = std::tuple<std::pair<Name, Data>...>;
            using required_vao_tuple = std::tuple<std::pair<AttribName, typename AttribTraits::data>...>;
            static_assert(!ct::tuple_any_of<required_vao_tuple, doesnt_contain, vao_tuple>::value, "not all or not matching type inputs for program was provided by given vao");

            vao.bind();
            // if was attached to different program or wasn't attached at all
            if (_program_id != vao._program_id)
            {
               vao._program_id = _program_id;
               glcxx_swallow(vao.template bind_buffer<AttribName>() &&
                             (AttribTraits::attach(_locations[ct::tuple_find<std::tuple<AttribName...>, AttribName>::value]), true));
            }
         }

      private:
         /// @brief locations for all VBOs inside vao
         std::array<GLint, sizeof...(AttribName)> _locations;

         /// @brief program id to check in runtime if input vao is ready to be
         /// bound for this program or it should be reattached for this program
         GLuint _program_id;
   };
}

#endif
