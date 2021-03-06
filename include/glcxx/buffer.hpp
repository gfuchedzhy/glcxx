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

#ifndef GLCXX_BUFFER_HPP
#define GLCXX_BUFFER_HPP

#include "glcxx/vao_base.hpp"
#include "glcxx/shader_type.hpp"
#include <memory>
#include <vector>

namespace glcxx
{
    /// @brief base class for all types of buffers
    class buffer_base
    {
        /// @brief disabled stuff
        buffer_base(const buffer_base&) = delete;
        buffer_base& operator=(const buffer_base& other) = delete;

        /// @brief buffer id
        GLuint _id;

        /// @brief target
        GLenum _target;

        /// @brief usage
        GLenum _usage;

    public:
        /// @brief constructor
        buffer_base(const void* data, size_t size, GLenum usage, GLenum target);

        /// @brief free buffer
        ~buffer_base()
        {
            glDeleteBuffers(1, &_id);
        }

        /// @brief binds buffer
        void bind() const
        {
            glBindBuffer(_target, _id);
        }

        /// @brief unbinds buffer
        void unbind() const
        {
            glBindBuffer(_target, 0);
        }

        /// @brief uploads new data to buffer, usage = 0 means do not change usage
        void upload(const void* data, size_t size, GLenum usage = 0);
    };

    /// @brief typesafe buffer object
    template<typename Data>
    class buffer : public buffer_base
    {
        /// @brief buffer element number
        GLsizei _size;

    public:
        /// @brief underlying data type
        using data = Data;

        /// @brief constructor
        buffer(const Data* data, size_t size, GLenum usage = GL_STATIC_DRAW, GLenum target = GL_ARRAY_BUFFER)
            : buffer_base(data, size*sizeof(Data), usage, target)
            , _size(size)
        {}

        /// @brief constructor
        buffer(const std::vector<Data>& v, GLenum usage = GL_STATIC_DRAW, GLenum target = GL_ARRAY_BUFFER)
            : buffer(v.data(), v.size(), usage, target)
        {}

        /// @brief constructor
        template<size_t N>
        buffer(const Data (&arr)[N], GLenum usage = GL_STATIC_DRAW, GLenum target = GL_ARRAY_BUFFER)
            : buffer(arr, N, usage, target)
        {}

        /// @brief uploads new data to buffer, usage = 0 means do not change usage
        void upload(const Data* data, size_t size, GLenum usage = 0)
        {
            buffer_base::upload(data, size*sizeof(Data), usage);
            _size = size;
        }

        /// @brief uploads new data to buffer, usage = 0 means do not change usage
        void upload(const std::vector<Data>& v, GLenum usage = 0) {
            upload(v.data(), v.size(), usage);
        }

        /// @brief uploads new data to buffer, usage = 0 means do not change usage
        template<size_t N>
        void upload(const Data (&arr)[N], GLenum usage = 0) {
            upload(arr, N, usage);
        }

        /// @brief returns element number
        GLsizei size() const { return _size; }
    };

    /// @brief base buffer ptr
    using buffer_base_ptr = std::shared_ptr<buffer_base>;

    /// @brief buffer ptr
    template<typename Data>
    using buffer_ptr = std::shared_ptr<buffer<Data>>;

    /// @brief make buffer
    template<typename Data>
    inline buffer_ptr<Data> make_buffer(const Data* data, size_t size, GLenum usage = GL_STATIC_DRAW)
    {
        return std::make_shared<buffer<Data>>(data, size, usage);
    }

    /// @brief make buffer
    template<typename Data>
    inline buffer_ptr<Data> make_buffer(const std::vector<Data>& v, GLenum usage = GL_STATIC_DRAW)
    {
        return std::make_shared<buffer<Data>>(v, usage);
    }

    /// @brief make buffer
    template<typename Data, size_t N>
    inline buffer_ptr<Data> make_buffer(const Data (&arr)[N], GLenum usage = GL_STATIC_DRAW)
    {
        return std::make_shared<buffer<Data>>(arr, N, usage);
    }

    /// @brief index buffer object, this buffer accepts ubyte, ushort and uint
    /// indices, also it saves mode, e.g. GL_TRIANGLE_STRIP
    class index_buffer : private buffer_base
    {
        /// @brief buffer element number
        GLsizei _size;

        /// @brief mode, e.g. GL_TRIANGLE_STRIP
        GLenum _mode;

        /// @brief type, e.g. GL_UNSIGNED_BYTE
        GLenum _type;

    public:
        /// @brief constructor
        template<typename T>
        index_buffer(const T* data, size_t size, GLenum mode, GLenum usage = GL_STATIC_DRAW)
            : buffer_base(data, size*sizeof(T), usage, GL_ELEMENT_ARRAY_BUFFER)
            , _size(size)
            , _mode(mode)
            , _type(shader_type::id<T>::value)
        {
            constexpr GLenum id = shader_type::id<T>::value;
            static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
        }

        /// @brief constructor
        template<typename T>
        index_buffer(const std::vector<T>& v, GLenum mode, GLenum usage = GL_STATIC_DRAW)
            : index_buffer(v.data(), v.size(), mode, usage)
        {}

        /// @brief constructor
        template<typename T, size_t N>
        index_buffer(const T (&arr)[N], GLenum mode, GLenum usage = GL_STATIC_DRAW)
            : index_buffer(arr, N, mode, usage)
        {}

        /// @brief uploads new data to buffer
        template<typename T>
        void upload(const T* data, size_t size, GLenum mode, GLenum usage = 0)
        {
            constexpr GLenum id = shader_type::id<T>::value;
            static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
            buffer_base::upload(data, size*sizeof(T), usage);
            _size = size;
            _mode = mode;
            _type = id;
        }

        /// @brief uploads new data to buffer
        template<typename T>
        void upload(const std::vector<T>& v, GLenum mode, GLenum usage = 0) {
            upload(v.data(), v.size(), mode, usage);
        }

        /// @brief uploads new data to buffer
        template<typename T, size_t N>
        void upload(const T (&arr)[N], GLenum mode, GLenum usage = 0) {
            upload(arr, N, mode, usage);
        }

        /// @brief draw with this index buffer
        void draw(const GLsizei instance_count) const
        {
            if (-1 == instance_count)
                glDrawElements(_mode, _size, _type, nullptr);
            else
                glDrawElementsInstanced(_mode, _size, _type, nullptr, instance_count);
        }

        using buffer_base::bind;

        /// @brief unbind index buffer
        static void unbind()
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    };

    /// @brief index buffer ptr
    using index_buffer_ptr = std::shared_ptr<index_buffer>;

    /// @brief make index buffer
    template<typename... Args>
    inline index_buffer_ptr make_index_buffer(Args&&... args)
    {
        return std::make_shared<index_buffer>(std::forward<Args>(args)...);
    }
}

#endif
