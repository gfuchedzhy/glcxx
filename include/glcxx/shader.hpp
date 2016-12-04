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

#ifndef GLCXX_SHADER_HPP
#define GLCXX_SHADER_HPP

#include <string>
#include "glcxx/gl.hpp"

namespace glcxx
{
    /// @brief shader resource holder
    class shader_base
    {
        /// @brief disabled stuff
        shader_base(const shader_base&) = delete;
        shader_base& operator=(const shader_base& other) = delete;

    protected:
        /// @brief shader object id
        GLuint _object = 0;

    public:
        /// @brief constructor
        shader_base(GLenum shader_type);

        /// @brief destructor
        ~shader_base();
    };

    class shader : private shader_base
    {
        /// @brief program this shader is attached to
        GLuint _program_object;

    public:
        /// @brief constructor
        shader(const std::string& glsl_version, std::string src, GLuint program, GLenum shader_type);

        /// @brief destructor
        ~shader();
    };
}

#endif
