// Copyright 2016 Grygorii Fuchedzhy <gfuchedzhy@gmail.com>
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
    /// @brief vao_input is basically a tag type, actual input class is
    /// vao_input_impl, which combines all attributes in single vao, vao_input
    /// is used for program declarations only
    template<typename Name, typename Type>
    struct vao_input {
        /// @brief name
        using name = Name;

        /// @brief vao holds VBOs, which are always vertex shader inputs
        using decl_tag = tag::vertex;
    };

    /// @brief holds state of program's vao
    template<typename AttribInputTuple> class vao_input_impl;

    /// @brief specialization for tuple
    template<typename... AttribName, typename... Attrib>
    class vao_input_impl<std::tuple<vao_input<AttribName, Attrib>...>>
    {
        /// @brief return true if Tuple doesn't contain T
        template<typename T, typename Tuple> struct doesnt_contain
        {
            static constexpr bool value = !ct::tuple_contains<Tuple, T>::value;
        };

        /// @brief locations for all VBOs inside vao
        std::array<GLint, sizeof...(AttribName)> _locations;

        /// @brief program id to check in runtime if input vao is ready to be
        /// bound for this program or it should be reattached for this program
        GLuint _program_id;

     public:
        /// @brief vao holds VBOs, which are always vertex shader inputs
        using decl_tag = tag::vertex;

        /// @brief ctstring containing glsl declaration of attributes
        using declaration = ct::string_cat<attrib_declaration<Attrib, AttribName>...>;

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
        void draw_elements(const vao<T...>& vao) const
        {
            if (0 != vao.instance_count())
            {
                bind(vao);
                vao.draw_elements();
                vao_base::unbind();
            }
        }

        /// @brief draw arrays
        template<typename... T>
        void draw_arrays(const vao<T...>& vao,
                         const GLenum mode,
                         const GLint first,
                         const GLsizei size) const
        {
            const auto instance_count = vao.instance_count();
            if (0 != instance_count)
            {
                bind(vao);
                if (-1 == instance_count)
                    glDrawArrays(mode, first, size);
                else
                    glDrawArraysInstanced(mode, first, size, instance_count);
                vao_base::unbind();
            }
        }

     private:
        /// @brief bind vao to current program
        template<typename... Name, typename... Data>
        void bind(const vao<std::pair<Name, Data>...>& vao) const
        {
            using vao_tuple = std::tuple<std::pair<Name, Data>...>;
            using required_vao_tuple = std::tuple<std::pair<AttribName, Attrib>...>;
            static_assert(!ct::tuple_any_of<required_vao_tuple, doesnt_contain, vao_tuple>::value, "not all or not matching type inputs for program was provided by given vao");

            vao.bind();
            // if was attached to different program or wasn't attached at all
            if (vao.program() != _program_id)
            {
                vao.program(_program_id);
                vao.attach_indices();
                glcxx_swallow(vao.template attach_attrib<Attrib, AttribName>(_locations[ct::tuple_find<std::tuple<AttribName...>, AttribName>::value]));
            }
        }
    };
}

#endif
