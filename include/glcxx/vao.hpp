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
#include "glcxx/buffer.hpp"

namespace glcxx
{
    namespace detail
    {
        /// @brief vao holding specific attributes
        template<bool HasIndexBuffer, typename...NamedAttrib> class vao;

        /// @brief specialization
        template<bool HasIndexBuffer, typename... Name, typename... Data>
        class vao<HasIndexBuffer, std::pair<Name, Data>...> : public vao_base
        {
            /// @brief buffer tuple
            using buffers = ct::tuple_cat<std::tuple<buffer_ptr<Data>...>, // VBOs
                                          // optional index buffer
                                          typename std::conditional<HasIndexBuffer, std::tuple<index_buffer_ptr>, std::tuple<>>::type>;

            /// @brief buffer tuple
            buffers _buffers;

            /// @brief buffer offsets
            std::array<std::ptrdiff_t, std::tuple_size<buffers>::value> _buffer_offsets = {};

            /// @brief traits for searching buffers in buffer tuple
            template<typename BufferName>
            struct traits
            {
                static constexpr size_t index = ct::tuple_find<std::tuple<Name..., cts("indices")>, BufferName>::value;
                using buffer_ptr = typename std::tuple_element<index, buffers>::type;
            };

        public:
            /// @brief set vbo or index buffer into vao
            template<typename BufferName>
            void set(typename traits<BufferName>::buffer_ptr vbo)
            {
                constexpr size_t index = traits<BufferName>::index;
                std::get<index>(_buffers) = std::move(vbo);
                // reset program id to reattach buffers
                reset_to_program(0);
            }

            /// @brief set vbo for attributes into vao
            template<typename BufferName>
            void offset(const ptrdiff_t offset)
            {
                constexpr size_t index = traits<BufferName>::index;
                _buffer_offsets[index] = offset;
                // reset program id to reattach buffers
                reset_to_program(0);
            }

            /// @brief bind buffer
            template<typename BufferName>
            void bind_buffer() const
            {
                const auto& p = std::get<traits<BufferName>::index>(_buffers);
                assert(p);
                p->bind();
            }

            /// @brief attach vertex buffer
            template<typename AttribTraits, typename BufferName>
            void attach_buffer(const GLint location) const
            {
                bind_buffer<BufferName>();

                constexpr size_t index = traits<BufferName>::index;
                static_assert(std::is_same<typename traits<BufferName>::buffer_ptr::element_type::data,
                                           typename AttribTraits::data>::value,
                              "buffer incompatible with attribute trait");
                AttribTraits::attach(location, static_cast<const typename AttribTraits::data*>(nullptr) + _buffer_offsets[index]);
            }

            /// @brief draw using index buffer
            template<bool HasIndexBuffer_ = HasIndexBuffer> // for SFINAE
            typename std::enable_if<HasIndexBuffer_>::type draw_elements() const
            {
                const index_buffer_ptr& p = std::get<traits<cts("indices")>::index>(_buffers);
                assert(p);
                p->draw();
            }

            /// @brief draw using index buffer
            template<bool HasIndexBuffer_ = HasIndexBuffer> // for SFINAE
            typename std::enable_if<HasIndexBuffer_>::type
            draw_elements_instanced(const GLsizei instance_count) const
            {
                const index_buffer_ptr& p = std::get<traits<cts("indices")>::index>(_buffers);
                assert(p);
                p->draw_instanced(instance_count);
            }

            /// @brief upload data to vbo, if doesn't exist, create it
            template<typename BufferName>
            void upload(const typename traits<BufferName>::buffer_ptr::element_type::data* data, size_t size, GLenum usage = 0)
            {
                auto& ptr = std::get<traits<BufferName>::index>(_buffers);
                if (ptr)
                    ptr->upload(data, size, usage);
                else
                    ptr = make_buffer(data, size, usage ? usage : GL_STATIC_DRAW);
            }

            /// @brief upload data to index buffer, if doesn't exist, create it
            template<typename BufferName, typename T>
            typename std::enable_if<HasIndexBuffer && std::is_same<BufferName, cts("indices")>::value>::type
            upload(const T* data, size_t size, GLenum mode, GLenum usage = 0)
            {
                index_buffer_ptr& ptr = std::get<traits<cts("indices")>::index>(_buffers);
                if (ptr)
                    ptr->upload(data, size, mode, usage);
                else
                    ptr = make_index_buffer(data, size, mode, usage ? usage : GL_STATIC_DRAW);
            }
        };
    }

    /// @brief shortcut types
    template<typename... NamedAttrib>
    using vao = detail::vao<false, NamedAttrib...>;
    template<typename... NamedAttrib>
    using indexed_vao = detail::vao<true, NamedAttrib...>;

    /// @brief make new vao with given buffers, version for vao without index buffer
    template<typename...Name, typename... BufferPtr>
    inline auto make_vao(BufferPtr&&... buf)
    {
        vao<std::pair<Name, typename std::remove_reference<BufferPtr>::type::element_type::data>...> vao;
        glcxx_swallow(vao.template set<Name>(std::forward<BufferPtr>(buf)));
        return vao;
    }

    /// @brief make new vao with given buffers, version for vao with index buffer
    template<typename...Name, typename IndexBufferPtr, typename... BufferPtr>
    inline auto make_indexed_vao(IndexBufferPtr&& indices, BufferPtr&&... buf)
    {
        indexed_vao<std::pair<Name, typename std::remove_reference<BufferPtr>::type::element_type::data>...> vao;
        vao.template set<cts("indices")>(std::forward<IndexBufferPtr>(indices));
        glcxx_swallow(vao.template set<Name>(std::forward<BufferPtr>(buf)));
        return vao;
    }
}

#endif
