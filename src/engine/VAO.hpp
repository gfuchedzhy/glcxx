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
      /// @brief disabled stuff
      CVertexArrayObjectBase(const CVertexArrayObjectBase&) = delete;
      CVertexArrayObjectBase& operator=(const CVertexArrayObjectBase& other) = delete;

      /// @brief vao id
      GLuint mID;

   public:
      /// @brief constructor
      CVertexArrayObjectBase()
      {
         gl(glGenVertexArrays, 1, &mID);
      }

      /// @brief destructor
      ~CVertexArrayObjectBase()
      {
         gl(glDeleteVertexArrays, 1, &mID);
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

/// @brief vao that has index buffer
class CIndexedVertexArrayObject : public CVertexArrayObjectBase
{
      /// @brief index buffer
      tIndexBufferPtr mIndexBuffer;

      /// @brief true if index buffer was changed and it should be rebound on
      /// attach
      mutable bool mIndexBufferDirty = false;

   public:
      /// @brief set index buffer
      void indexBuffer(const tIndexBufferPtr& value)
      {
         if (mIndexBuffer != value)
         {
            mIndexBufferDirty = true;
            mIndexBuffer = value;
         }
      }

      /// @brief draw using index buffer
      void drawElements() const
      {
         assert(mIndexBuffer);
         mIndexBuffer->draw();
      }

      /// @brief attach index buffer if it exists
      void attach() const
      {
         bind();
         if (mIndexBufferDirty)
         {
            mIndexBufferDirty = false;
            CIndexBuffer::unBind();
            if (mIndexBuffer)
               mIndexBuffer->bind();
         }
      }
};

/// @brief vao holding specific attributes
template<typename TAttribTuple> class TVertexArrayObject;

/// @brief specialization for tuple
template<typename... TNamedAttribs>
class TVertexArrayObject<std::tuple<TNamedAttribs...>> : public CIndexedVertexArrayObject
{
   public:
      /// @brief base class
      using tBase = CIndexedVertexArrayObject;

      /// @brief number of inputs
      static constexpr size_t size = sizeof...(TNamedAttribs);

      /// @brief locations for all VBOs inside vao
      using tLocations = std::array<GLint, size>;

      /// @brief return true if T is type named with TName
      template<typename T, typename TName>
      struct name_equals : std::integral_constant<bool, std::is_same<typename T::tName, TName>::value>
      {};

   private:
      /// @brief VBO tuple
      using tVBOs = std::tuple<tBufferPtr<typename TNamedAttribs::tAttribTraits::tData>...>;

      /// @brief tuple with attribute traits
      using tAttribTraits = std::tuple<typename TNamedAttribs::tAttribTraits...>;

      /// @brief VBOs
      tVBOs mVBOs;

      /// @brief bitset 
      mutable std::bitset<size> mDirty;

   public:
      /// @brief named set indices method
      template<typename TName>
      typename std::enable_if<std::is_same<TName, cts("indices")>::value>::type
      set(const tIndexBufferPtr& value)
      {
         tBase::indexBuffer(value);
      }

      /// @brief set vbo into vao
      template<typename TName, int I = ct::tuple_find_if<std::tuple<TNamedAttribs...>, name_equals, TName>::value>
      typename std::enable_if<I<size>::type
      set(const typename std::tuple_element<I, tVBOs>::type& vbo)
      {
         std::get<I>(mVBOs) = vbo;
         mDirty.set(I);
      }

      /// @brief upload data to current vbo, if none is current, create one,
      /// mark dirty
      template<typename TName,
               // vbo index
               size_t I = ct::tuple_find_if<std::tuple<TNamedAttribs...>, name_equals, TName>::value,
               // data type for current input, also used for SFINAE
               typename TData = typename std::tuple_element<I, tAttribTraits>::type::tData>
      void upload(const TData* data, size_t size, GLenum usage = 0)
      {
         auto& ptr = std::get<I>(mVBOs);
         if (ptr)
            ptr->upload(data, size, usage);
         else
         {
            ptr = make_buffer<TData>(data, size, usage ? usage : GL_STATIC_DRAW);
            mDirty.set(I);
         }
      }

      /// @brief attach vao, reattach VBOs if necessary
      void attach(const tLocations& locations) const
      {
         CIndexedVertexArrayObject::attach();
         if (mDirty.any())
            doAttach(locations, std::make_index_sequence<size>{});
      }

   private: // impl
      template<size_t...I>
      void doAttach(const tLocations& locations, std::index_sequence<I...>) const
      {
         swallow(doAttachSingle<I>(locations[I]));
      }

      template<size_t I>
      void doAttachSingle(GLint location) const
      {
         if (mDirty.test(I))
         {
            mDirty.reset(I);
            const auto& vbo = std::get<I>(mVBOs);
            assert(vbo);
            vbo->bind();
            std::tuple_element<I, std::tuple<typename TNamedAttribs::tAttribTraits...>>::type::attach(location);
            vbo->unBind();
         }
      }
};

#endif
