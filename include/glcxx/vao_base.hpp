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

        /// @brief program id this vao is enabled for
        mutable GLuint _program_id = 0;

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

        friend void swap(vao_base& x, vao_base& y)
        {
            std::swap(x._id, y._id);
            std::swap(x._program_id, y._program_id);
        }

        /// @brief update program id this vao was bound to, return true if it
        /// was really changed
        bool program_id(const GLuint program_id) const
        {
            const bool ret = program_id != _program_id;
            _program_id = program_id;
            return ret;
        }

        /// @brief assignment
        vao_base& operator=(vao_base other) noexcept
        {
            swap(*this, other);
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
