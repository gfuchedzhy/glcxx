// Copyright 2015, 2016 Grygorii Fuchedzhy <gfuchedzhy@gmail.com>
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

#ifndef GLCXX_UNIFORM_INPUT_HPP
#define GLCXX_UNIFORM_INPUT_HPP

#include "glcxx/input_var.hpp"

namespace glcxx
{
    /// @brief base implementation for uniform program input
    template<typename UniformTraits>
    class uniform_input_base
    {
        /// @brief location of program input inside program
        GLint _location;

        /// @brief holds actual uniform
        typename UniformTraits::data _uniform_data;

    public:
        /// @brief constructor
        uniform_input_base(GLint location)
            : _location(location)
        {
            attach_uniform(_location, glsl_cast<typename UniformTraits::glsl_data>(_uniform_data));
        }

        /// @brief set uniform as program input
        void set(const typename UniformTraits::data& value)
        {
            if (_uniform_data != value)
            {
                _uniform_data = value;
                attach_uniform(_location, glsl_cast<typename UniformTraits::glsl_data>(_uniform_data));
            }
        }

        /// @brief called after program was selected, nothing to do as uniforms
        /// remains attached during program selection change
        void select() const
        {}
    };

    /// @brief holds state of program's uniform
    template<typename Name, typename UniformTraits, typename DeclTag = tag::vertex>
    class uniform_input : public uniform_input_base<UniformTraits>
    {
        /// @brief base implementation class
        using base = uniform_input_base<UniformTraits>;

    public:
        /// @brief declaration tag
        using decl_tag = DeclTag;

        /// @brief ctstring containing glsl declaration of variable
        using declaration = typename UniformTraits::template declaration<Name>;

        /// @brief constructor
        uniform_input(const GLuint program)
            : base(get_uniform_loc(program, Name::chars))
        {}

        /// @brief named set method
        template<typename InputName>
        typename std::enable_if<std::is_same<InputName, Name>::value>::type
        set(const typename UniformTraits::data& value)
        {
            base::set(value);
        }
    };
}

#endif
