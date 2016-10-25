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

#ifndef GLCXX_BUFFER_INPUT_HPP
#define GLCXX_BUFFER_INPUT_HPP

#include "glcxx/buffer.hpp"

namespace glcxx
{
    template<typename AttribTraits>
    class buffer_input_base
    {
        /// @brief attribute location
        GLint _location;

        /// @brief buffer input offset
        std::ptrdiff_t _offset = 0;

        /// @brief holds actual buffer
        using buffer = buffer_ptr<typename AttribTraits::data>;
        buffer _buffer;

    public:
        /// @brief constructor
        buffer_input_base(GLint location)
            : _location(location)
        {}

        /// @brief set new buffer as program input
        void set(const buffer& value, const std::ptrdiff_t offset = 0)
        {
            if (_buffer != value || _offset != offset)
            {
                // detach old input, attach new one
                if (_buffer)
                    AttribTraits::detach(_location);
                _buffer = value;
                _offset = offset;
                select();
            }
        }

        /// @brief called after program was selected, attach buffer
        void select() const
        {
            if (_buffer)
            {
                _buffer->bind();
                AttribTraits::attach(_location, static_cast<const typename AttribTraits::data*>(nullptr) + _offset);
                _buffer->unbind();
            }
        }
    };

    /// @brief holds state of program's uniform
    template<typename Name, typename AttribTraits>
    class buffer_input : public buffer_input_base<AttribTraits>
    {
        /// @brief base implementation class
        using base = buffer_input_base<AttribTraits>;

    public:
        /// @brief VBOs are always vertex shader inputs
        using decl_tag = tag::vertex;

        /// @brief ctstring containing glsl declaration of variable
        using declaration = typename AttribTraits::template declaration<Name>;

        /// @brief constructor
        buffer_input(const GLuint program)
            : base(get_attrib_loc(program, Name::chars))
        {}

        /// @brief named set method
        template<typename InputName>
        typename std::enable_if<std::is_same<InputName, Name>::value>::type
        set(const buffer_ptr<typename AttribTraits::data>& value)
        {
            base::set(value);
        }
    };
}

#endif
