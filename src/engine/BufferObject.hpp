/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECT_HPP
#define ENGINE_BUFFEROBJECT_HPP

#include "GLSLInputVariable.hpp"
#include <memory>

/// @brief base class for all types of buffer objects
class CBufferObjectBase
{
      /// @brief disabled stuff
      CBufferObjectBase(const CBufferObjectBase&) = delete;
      CBufferObjectBase& operator=(const CBufferObjectBase& other) = delete;

      /// @brief buffer id
      GLuint mID;

      /// @brief target
      GLenum mTarget;

      /// @brief usage
      GLenum mUsage;

   public:
      /// @brief constructor
      CBufferObjectBase(const void* data, size_t size, GLenum usage, GLenum target)
         : mTarget(target)
         , mUsage(usage)
      {
         gl(glGenBuffers, 1, &mID);
         upload(data, size);
      }

      /// @brief free buffer
      ~CBufferObjectBase()
      {
         gl(glDeleteBuffers, 1, &mID);
      }

      /// @brief binds buffer
      void bind() const
      {
         gl(glBindBuffer, mTarget, mID);
      }

      /// @brief unbinds buffer
      void unBind() const
      {
         gl(glBindBuffer, mTarget, 0);
      }

      /// @brief uploads new data to buffer, usage = 0 means do not change usage
      void upload(const void* data, size_t size, GLenum usage = 0)
      {
         if (usage)
            mUsage = usage;
         bind();
         gl(glBufferData, mTarget, size, data, mUsage);
         unBind();
      }
};

/// @brief typesafe buffer object
template<typename TData>
class TBufferObject : public CBufferObjectBase
{
   public:
      /// @brief underlying data type
      using tData = TData;

      /// @brief constructor
      TBufferObject(const TData* data, size_t size, GLenum usage = GL_STATIC_DRAW, GLenum target = GL_ARRAY_BUFFER)
         : CBufferObjectBase(data, size*sizeof(TData), usage, target)
      {}

      /// @brief uploads new data to buffer, usage = 0 means do not change usage
      void upload(const TData* data, size_t size, GLenum usage = 0)
      {
         CBufferObjectBase::upload(data, size*sizeof(TData), usage);
      }
};

/// @brief buffer ptr
template<typename TData>
using tBufferPtr = std::shared_ptr<TBufferObject<TData>>;

/// @brief make buffer
template<typename TData>
inline tBufferPtr<TData> make_buffer(const TData* data, size_t size, GLenum usage = GL_STATIC_DRAW)
{
   return std::make_shared<TBufferObject<TData>>(data, size, usage);
}

/// @brief index buffer object, this buffer accepts ubyte, ushort and uint
/// indices, also it saves mode, e.g. GL_TRIANGLE_STRIP
class CIndexBuffer : public CBufferObjectBase
{
      /// @brief size of buffer
      GLsizei mSize;

      /// @brief mode, e.g. GL_TRIANGLE_STRIP
      GLenum mMode;

      /// @brief type, e.g. GL_UNSIGNED_BYTE
      GLenum mType;

   public:
      /// @brief constructor
      template<typename T>
      CIndexBuffer(const T* data, size_t size, GLenum mode, GLenum usage = GL_STATIC_DRAW)
         : CBufferObjectBase(data, size*sizeof(T), usage,
                 // before binding index buffer for upload vao should be unbound
                 // to preserve vao's internal state, note that this in not the
                 // case for vertex buffers as their bindings are not part of
                 // vao's state
                 (gl(glBindVertexArray, 0), GL_ELEMENT_ARRAY_BUFFER))
         , mSize(size)
         , mMode(mode)
         , mType(glsl::TTypeID<T>::id)
      {
         constexpr GLenum id = glsl::TTypeID<T>::id;
         static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
      }

      /// @brief uploads new data to buffer
      template<typename T>
      void upload(const T* data, size_t size, GLenum mode, GLenum usage = 0)
      {
         constexpr GLenum id = glsl::TTypeID<T>::id;
         static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
         // before binding index buffer for upload vao should be unbound to
         // preserve vao's internal state, note that this in not the case for
         // vertex buffers as their bindings are not part of vao's state
         gl(glBindVertexArray, 0);
         CBufferObjectBase::upload(data, size*sizeof(T), usage);
         mSize = size;
         mMode = mode;
         mType = id;
      }

      /// @brief draw with this index buffer
      void draw() const
      {
         gl(glDrawElements, mMode, mSize, mType, nullptr);
      }

      /// @brief unbind index buffer
      static void unBind()
      {
         gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
      }
};

/// @brief index buffer ptr
using tIndexBufferPtr = std::shared_ptr<CIndexBuffer>;

/// @brief make index buffer
template<typename... TArgs>
inline tIndexBufferPtr make_indexBuffer(TArgs&&... args)
{
   return std::make_shared<CIndexBuffer>(std::forward<TArgs>(args)...);
}

#endif
