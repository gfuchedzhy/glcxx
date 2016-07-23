/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_VAO_HPP
#define GLCXX_SRC_VAO_HPP

#include <bitset>
#include "buffer.hpp"

// todo namespace
// namespace glcxx
// {
   /// @brief base vao class, resource holder
   class vao_base
   {
         /// @brief friend declaration
         template<typename> friend class TVertexArrayObjectProgramInput;

         /// @brief non copyable
         vao_base(const vao_base&) = delete;

         /// @brief vao id
         GLuint mID = 0;

         /// @brief program id this vao is enabled for
         mutable GLuint mProgramID = 0;

      public:
         /// @brief constructor
         vao_base()
         {
            gl(glGenVertexArrays, 1, &mID);
         }

         /// @brief move constructor
         vao_base(vao_base&& other) noexcept
            : mID(other.mID)
            , mProgramID(other.mProgramID)
         {
            other.mProgramID = 0;
            other.mID = 0;
         }

         /// @brief destructor
         ~vao_base()
         {
            if (mID)
               gl(glDeleteVertexArrays, 1, &mID);
         }

         friend void swap(vao_base& x, vao_base& y)
         {
            std::swap(x.mID, y.mID);
            std::swap(x.mProgramID, y.mProgramID);
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
            gl(glBindVertexArray, mID);
         }

         /// @brief unbinds vao
         static void unbind()
         {
            gl(glBindVertexArray, 0);
         }
   };

   namespace detail
   {
      /// @brief vao holding specific attributes
      template<bool HasIndexBuffer, typename...TNamedAttrib> class vao;

      /// @brief specialization
      template<bool HasIndexBuffer, typename... TName, typename... TData>
      class vao<HasIndexBuffer, ct::named_type<TName, TData>...> : public vao_base
      {
            /// @brief buffer tuple
            using buffers = ct::tuple_cat<std::tuple<buffer_ptr<TData>...>, // VBOs
                                          // optional index buffer
                                          typename std::conditional<HasIndexBuffer, std::tuple<index_buffer_ptr>, std::tuple<>>::type>;

            /// @brief buffer tuple
            buffers mBuffers;

            /// @brief traits for searching buffers in buffer tuple
            template<typename Name>
            struct traits
            {
                  static constexpr size_t index = ct::tuple_find<std::tuple<TName..., cts("indices")>, Name>::value;
                  using buffer_ptr = typename std::tuple_element<index, buffers>::type;
                  // using data = typename buffer_ptr::element_type::data;
            };

         public:
            /// @brief set vbo or index buffer into vao
            template<typename Name>
            void set(typename traits<Name>::buffer_ptr vbo)
            {
               std::get<traits<Name>::index>(mBuffers) = std::move(vbo);
            }

            /// @brief bind buffer return true if bound
            template<typename Name>
            bool bind_buffer() const
            {
               const auto& p = std::get<traits<Name>::index>(mBuffers);
               if (p)
               {
                  p->bind();
                  return true;
               }
               return false;
            }

            /// @brief draw using index buffer
            template<bool HasIndexBuffer_ = HasIndexBuffer> // for SFINAE
            typename std::enable_if<HasIndexBuffer_>::type draw_elements() const
            {
               const index_buffer_ptr& p = std::get<traits<cts("indices")>::index>(mBuffers);
               assert(p);
               p->draw();
            }

            /// @brief upload data to vbo, if doesn't exist, create it
            template<typename Name>
            void upload(const typename traits<Name>::buffer_ptr::element_type::data* data, size_t size, GLenum usage = 0)
            {
               auto& ptr = std::get<traits<Name>::index>(mBuffers);
               if (ptr)
                  ptr->upload(data, size, usage);
               else
                  ptr = make_buffer(data, size, usage ? usage : GL_STATIC_DRAW);
            }

            /// @brief upload data to index buffer, if doesn't exist, create it
            template<typename Name, typename T>
            typename std::enable_if<HasIndexBuffer && std::is_same<Name, cts("indices")>::value>::type
            upload(const T* data, size_t size, GLenum mode, GLenum usage = 0)
            {
               index_buffer_ptr& ptr = std::get<traits<cts("indices")>::index>(mBuffers);
               if (ptr)
                  ptr->upload(data, size, mode, usage);
               else
                  ptr = make_index_buffer(data, size, mode, usage ? usage : GL_STATIC_DRAW);
            }
      };
   }

   /// @brief shortcut types
   template<typename...TNamedAttrib>
   using vao = detail::vao<false, TNamedAttrib...>;
   template<typename...TNamedAttrib>
   using indexed_vao = detail::vao<true, TNamedAttrib...>;

   /// @brief make new vao with given buffers, version for vao without index buffer
   template<typename...TName, typename... TBufferPtr>
   inline auto make_vao(TBufferPtr&&... buf)
   {
      vao<ct::named_type<TName, typename std::remove_reference<TBufferPtr>::type::element_type::data>...> vao;
      swallow(vao.template set<TName>(std::forward<TBufferPtr>(buf)));
      return vao;
   }

   /// @brief make new vao with given buffers, version for vao with index buffer
   template<typename...TName, typename TIndexBufferPtr, typename... TBufferPtr>
   inline auto make_vao(TIndexBufferPtr&& indices, TBufferPtr&&... buf)
   {
      indexed_vao<ct::named_type<TName, typename std::remove_reference<TBufferPtr>::type::element_type::data>...> vao;
      vao.template set<cts("indices")>(std::forward<TIndexBufferPtr>(indices));
      swallow(vao.template set<TName>(std::forward<TBufferPtr>(buf)));
      return vao;
   }

// }

#endif
