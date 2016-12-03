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
    template<typename Attrib>
    class buffer_input_base
    {
        /// @brief attribute location
        GLint _location;

        /// @brief holds buffer and its layout
        attrib _attrib;

    public:
        /// @brief constructor
        buffer_input_base(GLint location)
            : _location(location)
        {}

        /// @brief set new buffer and its layout as program input
        /// @param offset buffer offset in units of T type
        template<typename T>
        void set(buffer_ptr<T> buf,
                 const GLuint divisor = 0u,
                 const GLsizei offset = 0,
                 const bool normalize = true)
        {
            static_assert(is_glsl_convertible<T, Attrib>::value, "types are not convertible");
            if (_attrib.set(std::move(buf), divisor, offset, normalize)) // changed?
                select();
        }

        /// @brief set new buffer and its layout as program input
        /// @param offset buffer offset in units of T type
        template<typename T, typename U> inline bool
        reset(buffer_ptr<T> buf,
              U T::*member,
              const GLuint divisor = 0u,
              const GLsizei offset = 0,
              const bool normalize = true)
        {
            static_assert(is_glsl_convertible<U, Attrib>::value, "types are not convertible");
            if (_attrib.set(std::move(buf), member, divisor, offset, normalize)) // changed?
                select();
        }

        /// @brief called after program was selected
        void select() const
        {
            _attrib.attach_unsafe<Attrib>(_location);
        }
    };

    /// @brief holds state of program's uniform
    template<typename Name, typename Attrib>
    class buffer_input : public buffer_input_base<Attrib>
    {
        /// @brief base implementation class
        using base = buffer_input_base<Attrib>;

    public:
        /// @brief name
        using name = Name;

        /// @brief VBOs are always vertex shader inputs
        using decl_tag = tag::vertex;

        /// @brief ctstring containing glsl declaration of variable
        using declaration = attrib::declaration<Attrib, Name>;

        /// @brief constructor
        buffer_input(const GLuint program)
            : base(get_attrib_loc(program, Name::chars))
        {}

        /// @brief named set method
        template<typename InputName, typename T>
        std::enable_if_t<std::is_same<InputName, Name>::value>
        set(const buffer_ptr<T>& value)
        {
            base::set(value);
        }
    };
}

#endif
