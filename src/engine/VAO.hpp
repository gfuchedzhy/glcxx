/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_VAO_HPP
#define ENGINE_VAO_HPP

#include "BufferObject.hpp"
#include <bitset>

/// @brief base VAO class, resource holder
class CVertexArrayObjectBase
{
      /// @brief friend declaration
      template<typename> friend class TVertexArrayObjectProgramInput;

      /// @brief non copyable
      CVertexArrayObjectBase(const CVertexArrayObjectBase&) = delete;

      /// @brief vao id
      GLuint mID = 0;

      /// @brief program id this vao is enabled for
      mutable GLuint mProgramID = 0;

   public:
      /// @brief constructor
      CVertexArrayObjectBase()
      {
         gl(glGenVertexArrays, 1, &mID);
      }

      /// @brief move constructor
      CVertexArrayObjectBase(CVertexArrayObjectBase&& other) noexcept
         : mID(other.mID)
         , mProgramID(other.mProgramID)
      {
         other.mProgramID = 0;
         other.mID = 0;
      }

      /// @brief destructor
      ~CVertexArrayObjectBase()
      {
         if (mID)
            gl(glDeleteVertexArrays, 1, &mID);
      }

      friend void swap(CVertexArrayObjectBase& x, CVertexArrayObjectBase& y)
      {
         std::swap(x.mID, y.mID);
         std::swap(x.mProgramID, y.mProgramID);
      }

      /// @brief assignment
      CVertexArrayObjectBase& operator=(CVertexArrayObjectBase other) noexcept
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
      static void unBind()
      {
         gl(glBindVertexArray, 0);
      }
};

/// @brief vao holding specific attributes
template<bool hasIndexBuffer, typename...TNamedAttrib> class TVertexArrayObject;

/// @brief shortcut types
template<typename...TNamedAttrib>
using tVAO = TVertexArrayObject<false, TNamedAttrib...>;
template<typename...TNamedAttrib>
using tIndexedVAO = TVertexArrayObject<true, TNamedAttrib...>;

/// @brief specialization
template<bool hasIndexBuffer, typename... TName, typename... TData>
class TVertexArrayObject<hasIndexBuffer, ct::named_type<TName, TData>...> : public CVertexArrayObjectBase
{
      /// @brief buffer tuple
      using tBuffers = ct::tuple_cat<std::tuple<tBufferPtr<TData>...>, // VBOs
                                     // optional index buffer
                                     typename std::conditional<hasIndexBuffer, std::tuple<tIndexBufferPtr>, std::tuple<>>::type>;

      /// @brief buffer tuple
      tBuffers mBuffers;

      /// @brief traits for searching buffers in buffer tuple
      template<typename Name>
      struct traits
      {
            static constexpr size_t index = ct::tuple_find<std::tuple<TName..., cts("indices")>, Name>::value;
            using tBufferPtr = typename std::tuple_element<index, tBuffers>::type;
      };

   public:
      /// @brief set vbo or index buffer into vao
      template<typename Name>
      void set(typename traits<Name>::tBufferPtr vbo)
      {
         std::get<traits<Name>::index>(mBuffers) = std::move(vbo);
      }

      /// @brief bind buffer return true if bound
      template<typename Name>
      bool bindBuffer() const
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
      template<bool _hasIndexBuffer = hasIndexBuffer> // for SFINAE
      typename std::enable_if<_hasIndexBuffer>::type drawElements() const
      {
         const tIndexBufferPtr& p = std::get<traits<cts("indices")>::index>(mBuffers);
         assert(p);
         p->draw();
      }

      /// @brief upload data to vbo, if doesn't exist, create it
      template<typename Name, size_t I = ct::tuple_find<std::tuple<TName...>, Name>::value>
      typename std::enable_if<!hasIndexBuffer || !std::is_same<Name, cts("indices")>::value>::type
      upload(const typename std::tuple_element<I, std::tuple<TData...>>::type* data, size_t size, GLenum usage = 0)
      {
         auto& ptr = std::get<I>(mBuffers);
         if (ptr)
            ptr->upload(data, size, usage);
         else
            ptr = make_buffer(data, size, usage ? usage : GL_STATIC_DRAW);
      }

      /// @brief upload data to index buffer, if doesn't exist, create it
      template<typename Name, typename T>
      typename std::enable_if<hasIndexBuffer && std::is_same<Name, cts("indices")>::value>::type
      upload(const T* data, size_t size, GLenum mode, GLenum usage = 0)
      {
         tIndexBufferPtr& ptr = std::get<traits<cts("indices")>::index>(mBuffers);
         if (ptr)
            ptr->upload(data, size, mode, usage);
         else
            ptr = make_indexBuffer(data, size, mode, usage ? usage : GL_STATIC_DRAW);
      }
};

/// @brief make new vao with given buffers, version for vao without index buffer
template<typename...TName, typename... TBufferPtr>
inline auto make_vao(TBufferPtr&&... buf)
{
   tVAO<ct::named_type<TName, typename std::remove_reference<TBufferPtr>::type::element_type::tData>...> vao;
   swallow(vao.template set<TName>(std::forward<TBufferPtr>(buf)));
   return vao;
}

/// @brief make new vao with given buffers, version for vao with index buffer
template<typename...TName, typename TIndexBufferPtr, typename... TBufferPtr>
inline auto make_vao(TIndexBufferPtr&& indices, TBufferPtr&&... buf)
{
   tIndexedVAO<ct::named_type<TName, typename std::remove_reference<TBufferPtr>::type::element_type::tData>...> vao;
   vao.template set<cts("indices")>(std::forward<TIndexBufferPtr>(indices));
   swallow(vao.template set<TName>(std::forward<TBufferPtr>(buf)));
   return vao;
}

#endif
