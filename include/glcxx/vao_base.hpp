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

#ifndef GLCXX_VAO_BASE_HPP
#define GLCXX_VAO_BASE_HPP

#include "glcxx/gl.hpp"
#include <algorithm>

namespace glcxx
{
    /// @brief base vao class, resource holder
    class vao_base
    {
        /// @brief non copyable
        vao_base(const vao_base&) = delete;

        /// @brief vao id
        GLuint _id = 0;

        /// @brief program id this vao is attached to
        mutable GLuint _program_id = 0;

        // only vao input is allowed to change program
        template<typename AttribInputTuple>
        friend class vao_input_impl;

    protected:
        /// @brief return which program vao is currently attached to
        GLuint program() const
        {
            return _program_id;
        }

        /// @brief set program this vao is attached to
        void program(const GLuint program_id) const
        {
            _program_id = program_id;
        }

    public:
        /// @brief constructor
        vao_base()
        {
            glGenVertexArrays(1, &_id);
        }

        /// @brief move constructor
        vao_base(vao_base&& other) noexcept
            : _id(other._id)
            , _program_id(other._program_id)
        {
            other._program_id = 0;
            other._id = 0;
        }

        /// @brief destructor
        ~vao_base()
        {
            if (_id)
                glDeleteVertexArrays(1, &_id);
        }

        /// @brief assignment
        vao_base& operator=(vao_base other) noexcept {
            std::swap(_id, other._id);
            std::swap(_program_id, other._program_id);
            return *this;
        }

        /// @brief binds vao
        void bind() const
        {
            glBindVertexArray(_id);
        }

        /// @brief unbinds vao
        static void unbind()
        {
            glBindVertexArray(0);
        }
    };
}

#endif
