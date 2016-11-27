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

#ifndef GLCXX_VAO_HPP
#define GLCXX_VAO_HPP

#include "glcxx/vao_base.hpp"
#include "glcxx/attrib.hpp"
#include <array>

namespace glcxx
{
    /// @brief vao holding specific attributes
    template<typename...NamedAttrib> class vao;

    /// @brief specialization
    template<typename... Name, typename... Data>
    class vao<std::pair<Name, Data>...> : public vao_base
    {
        /// @brief index buffer
        index_buffer_ptr _indices;

        /// @brief number of attributes provided by this vao
        static constexpr size_t attrib_num = sizeof...(Data);

        /// @brief attributes
        std::array<attrib, attrib_num> _attribs;

        /// @brief instance count
        GLsizei _instance_count = -1;

        /// @return attribute index by name
        template<typename AttribName>
        struct attrib_index {
            static constexpr size_t value = ct::tuple_find<std::tuple<Name...>, AttribName>::value;
        };

        /// @brief shader type of attribute by name
        template<typename AttribName>
        using attrib_shader_type = typename std::tuple_element<attrib_index<AttribName>::value, std::tuple<Data...>>::type;

    public:
        /// @brief get/set instance count
        auto instance_count() const { return _instance_count; }
        void instance_count(GLsizei v) { _instance_count = v; }

        /// @brief set index buffer
        void indices(index_buffer_ptr buf)
        {
            if (_indices != buf)
            {
                _indices = std::move(buf);
                program(0);
            }
        }

        /// @brief set attrib
        template<typename AttribName, typename T>
        void set(buffer_ptr<T> buf,
                 const GLuint divisor = 0u,
                 const GLsizei offset = 0,
                 const bool normalize = true)
        {
            constexpr size_t index = attrib_index<AttribName>::value;
            static_assert(index < attrib_num, "attribute with given name wasn't found");
            static_assert(is_glsl_convertible<T, attrib_shader_type<AttribName>>::value, "types are not convertible");

            if (_attribs[index].set(std::move(buf), divisor, offset, normalize)) // changed?
                program(0);
        }

        /// @brief set attrib
        template<typename AttribName, typename T, typename U>
        void set(buffer_ptr<T> buf,
                 U T::*member,
                 const GLuint divisor = 0u,
                 const GLsizei offset = 0,
                 const bool normalize = true)
        {
            constexpr size_t index = attrib_index<AttribName>::value;
            static_assert(index < attrib_num, "attribute with given name wasn't found");
            static_assert(is_glsl_convertible<U, attrib_shader_type<AttribName>>::value, "types are not convertible");

            if (_attribs[index].set(std::move(buf), member, divisor, offset, normalize)) // changed?
                program(0);
        }

        /// @brief bind index buffer if exists, otherwise unbind previously
        /// bound index buffer
        void attach_indices() const
        {
            if (_indices)
                _indices->bind();
            else
                index_buffer::unbind();
        }

        /// @brief attach vertex buffer
        template<typename ShaderType, typename AttribName>
        void attach_attrib(const GLint location) const
        {
            constexpr size_t index = attrib_index<AttribName>::value;
            static_assert(index < attrib_num, "attribute with given name wasn't found");
            static_assert(std::is_same<ShaderType, attrib_shader_type<AttribName>>::value,
                          "attrib is incompatible with provided shader type");

            _attribs[index].template attach_unsafe<ShaderType>(location);
        }

        /// @brief draw using index buffer
        void draw_elements() const
        {
            assert(_indices);
            _indices->draw(_instance_count);
        }

        /// @brief upload data to attribute vbo, if doesn't exist, create it
        template<typename AttribName, typename T>
        void upload(const T* data, size_t size, GLenum usage = 0)
        {
            constexpr size_t index = attrib_index<AttribName>::value;
            static_assert(index < attrib_num, "attribute with given name wasn't found");
            static_assert(is_glsl_convertible<T, attrib_shader_type<AttribName>>::value, "types are not convertible");
            if (_attribs[index].upload(data, size, usage))
                program(0);
        }

        /// @brief upload data to attribute vbo, if doesn't exist, create it
        template<typename AttribName, typename T>
        void upload(const std::vector<T>& v, GLenum usage = 0) {
            upload<AttribName>(v.data(), v.size(), usage);
        }

        /// @brief upload data to attribute vbo, if doesn't exist, create it
        template<typename AttribName, typename T, size_t N>
        void upload(const T (&arr)[N], GLenum usage = 0) {
            upload<AttribName>(arr, N, usage);
        }

        /// @brief upload data to index buffer, if doesn't exist, create it
        template<typename T>
        void upload_indices(const T* data, size_t size, GLenum mode, GLenum usage = 0)
        {
            if (_indices)
                _indices->upload(data, size, mode, usage);
            else
                _indices = make_index_buffer(data, size, mode, usage ? usage : GL_STATIC_DRAW);
        }

        /// @brief upload data to index buffer, if doesn't exist, create it
        template<typename T>
        void upload_indices(const std::vector<T>& v, GLenum mode, GLenum usage = 0) {
            upload_indices(v.data(), v.size(), mode, usage);
        }

        /// @brief upload data to index buffer, if doesn't exist, create it
        template<typename T, size_t N>
        void upload_indices(const T (&arr)[N], GLenum mode, GLenum usage = 0) {
            upload_indices(arr, N, mode, usage);
        }
    };

    /// @brief make new vao with given buffers, version for vao with index buffer
    template<typename...Name, typename IndexBufferPtr, typename... BufferPtr>
    inline auto make_indexed_vao(IndexBufferPtr&& indices, BufferPtr&&... buf)
    {
        vao<std::pair<Name, typename std::remove_reference<BufferPtr>::type::element_type::data>...> vao;
        vao.indices(std::forward<IndexBufferPtr>(indices));
        glcxx_swallow(vao.template set<Name>(std::forward<BufferPtr>(buf)));
        return vao;
    }
}

#endif
