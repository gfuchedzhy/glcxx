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

#ifndef GLCXX_ATTRIB_HPP
#define GLCXX_ATTRIB_HPP

#include "glcxx/shader_type.hpp"
#include "glcxx/buffer.hpp"

namespace glcxx
{
    /// @brief get attribute location
    GLint get_attrib_loc(GLuint program, const char* name);

    /// @brief ctstring containing glsl declaration of attribute
    template<typename ShaderType, typename Name>
    using attrib_declaration = ct::string_cat<cts("in "), typename shader_type::traits<ShaderType>::name, cts(" "), Name, cts(";\n")>;

    /// @brief glVertexAttribPointer for float based shader type
    template<typename ShaderType>
    static inline auto gl_vertex_attrib_pointer(const GLint location,
                                                const GLenum type,
                                                const bool normalize,
                                                const GLsizei stride,
                                                const GLsizei byte_offset)
        -> std::enable_if_t<shader_type::traits<ShaderType>::id == GL_FLOAT>
    {
        glVertexAttribPointer(location, shader_type::traits<ShaderType>::components_num, type,
                              normalize, stride, reinterpret_cast<const void*>(byte_offset));
    }

    /// @brief glVertexAttribPointer for integer shader types
    template<typename ShaderType>
    static inline auto gl_vertex_attrib_pointer(const GLint location,
                                                const GLenum type,
                                                const bool /*normalize*/, // ignored
                                                const GLsizei stride,
                                                const GLsizei byte_offset)
        -> std::enable_if_t<shader_type::traits<ShaderType>::id == GL_BYTE ||
                            shader_type::traits<ShaderType>::id == GL_UNSIGNED_BYTE ||
                            shader_type::traits<ShaderType>::id == GL_SHORT ||
                            shader_type::traits<ShaderType>::id == GL_UNSIGNED_SHORT ||
                            shader_type::traits<ShaderType>::id == GL_INT ||
                            shader_type::traits<ShaderType>::id == GL_UNSIGNED_INT>
    {
        glVertexAttribIPointer(location, shader_type::traits<ShaderType>::components_num,
                               type, stride, reinterpret_cast<const void*>(byte_offset));
    }

#ifdef glVertexAttribLPointer
    /// @brief glVertexAttribPointer for double shader type
    template<typename ShaderType>
    static inline auto gl_vertex_attrib_pointer(const GLint location,
                                                const GLenum type,
                                                const bool /*normalize*/, // ignored
                                                const GLsizei stride,
                                                const GLsizei byte_offset)
        -> std::enable_if_t<shader_type::traits<ShaderType>::id == GL_DOUBLE>
    {
        glVertexAttribLPointer(location, shader_type::traits<ShaderType>::components_num,
                               type, stride, reinterpret_cast<const void*>(byte_offset));
    }
#endif

    /// @brief holds buffer and its layout
    class attrib
    {
        buffer_base_ptr _buffer;
        GLenum _type;
        GLuint _divisor = 0;
        GLsizei _stride;
        bool _normalize = true;
        GLsizei _basic_type_size;
        GLsizei _byte_offset = 0;

    public:
        /// @return true if was changed
        template<typename T> inline bool
        set(buffer_ptr<T> buf,
              const GLuint divisor,
              const GLsizei offset,
              const bool normalize)
        {
            const GLsizei byte_offset = offset*sizeof(T);
            if (_buffer != buf ||
                _divisor != divisor ||
                _byte_offset != byte_offset ||
                _normalize != normalize)
            {
                _buffer = std::move(buf);
                _type = shader_type::traits<T>::id;
                _divisor = divisor;
                _stride = sizeof(T);
                _normalize = normalize;
                _basic_type_size = sizeof(typename shader_type::traits<T>::basic_type);
                _byte_offset = byte_offset;
                return true;
            }
            return false;
        }

        /// @return true if was changed
        template<typename T, typename U> inline bool
        set(buffer_ptr<T> buf,
            U T::*member,
            const GLuint divisor,
            const GLsizei offset,
            const bool normalize)
        {
            const GLsizei byte_offset = member_offset(member) + offset*sizeof(T);
            if (_buffer != buf ||
                _divisor != divisor ||
                _byte_offset != byte_offset ||
                _normalize != normalize)
            {
                _buffer = std::move(buf);
                _type = shader_type::traits<U>::id;
                _divisor = divisor;
                _stride = sizeof(T);
                _normalize = normalize;
                _basic_type_size = sizeof(typename shader_type::traits<U>::basic_type);
                _byte_offset = byte_offset;
                return true;
            }
            return false;
        }

        /// @return true if attribute should be reattached after upload
        template<typename T>
        bool upload(const T* data, size_t size, GLenum usage = 0)
        {
            // buffer exists and no one else is using it
            if (_buffer && 1 == _buffer.use_count())
            {
                _buffer->upload(data, size*sizeof(T), usage);

                // need to reattach
                if (_type != shader_type::traits<T>::id ||
                    _stride != sizeof(T) ||
                    _basic_type_size != sizeof(typename shader_type::traits<T>::basic_type) ||
                    _byte_offset != 0)
                {
                    _type = shader_type::traits<T>::id;
                    _stride = sizeof(T);
                    _basic_type_size = sizeof(typename shader_type::traits<T>::basic_type);
                    _byte_offset = 0;
                    return true;
                }
            }
            else
                return set(make_buffer(data, size, usage ? usage : GL_STATIC_DRAW),
                           _divisor, 0, _normalize);
            return false;
        }

        /// @brief attach attrib, type unsafe, ensure that current attribute
        /// is_glsl_convertible to ShaderType
        template<typename ShaderType>
        void attach_unsafe(const GLint location) const
        {
            using namespace shader_type;
            constexpr size_t locations_num  = traits<ShaderType>::locations_num;
            constexpr size_t components_num = traits<ShaderType>::components_num;

            if (_buffer)
            {
                _buffer->bind();
                for (size_t i = 0; i < locations_num; ++i)
                {
                    glEnableVertexAttribArray(location + i);

                    gl_vertex_attrib_pointer<ShaderType>(
                        location + i, _type, _normalize, _stride,
                        _byte_offset + i*components_num*_basic_type_size);

                    glVertexAttribDivisor(location + i, _divisor);
                }
                _buffer->unbind();
            }
            else
            {
                for (GLint i = 0; i < shader_type::traits<ShaderType>::locations_num; ++i)
                    glDisableVertexAttribArray(location + i);
            }
        }
    };
}

#endif
