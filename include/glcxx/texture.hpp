// Copyright 2015, 2016 Grygoriy Fuchedzhy
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

#ifndef GLCXX_TEXTURE_HPP
#define GLCXX_TEXTURE_HPP

#include <memory>
#include "glcxx/input_var.hpp"

namespace glcxx
{
    /// @brief texture resource holder
    class texture
    {
        /// @brief disabled stuff
        texture(const texture&) = delete;
        texture& operator=(const texture& other) = delete;

        /// @brief texture id
        GLuint _id;

    protected:
        /// @brief texture target
        GLenum _target = 0;

    public:
        /// @brief reads texture from file, uploads to gpu
        texture(GLenum target)
            : _target(target)
        {
            glGenTextures(1, &_id);
        }

        /// @brief frees texture
        ~texture()
        {
            glDeleteTextures(1, &_id);
        }

        /// @brief binds texture
        void bind() const
        {
            glBindTexture(_target, _id);
        }

        /// @brief unbinds texture
        void unbind()
        {
            glBindTexture(_target, 0);
        }
    };

    /// @brief texture ptr
    using texture_ptr = std::shared_ptr<texture>;
}

#endif
