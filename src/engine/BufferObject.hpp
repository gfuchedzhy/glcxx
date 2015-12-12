/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECT_HPP
#define ENGINE_BUFFEROBJECT_HPP

#include "GLSLInputVariable.hpp"

/// @brief buffer object
template<typename TData>
class TBufferObject
{
      /// @brief disabled stuff
      TBufferObject(const TBufferObject&) = delete;
      TBufferObject& operator=(const TBufferObject& other) = delete;

      /// @brief buffer id
      GLuint mID;

      /// @brief target
      GLenum mTarget;

   public:
      /// @brief constructor
      TBufferObject(const TData* data, size_t size, GLenum target = GL_ARRAY_BUFFER)
         : mTarget(target)
      {
         gl(glGenBuffers, 1, &mID);
         bind();
         gl(glBufferData, mTarget, size*sizeof(TData), data, GL_STATIC_DRAW);
         unBind();
      }

      /// @brief free buffer
      ~TBufferObject()
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
};

/// @brief index buffer object, though underlying implementation is buffer of
/// unsigned chars, this buffer accepts ubyte, ushort and uint indices, also it
/// saves mode, e.g. GL_TRIANGLE_STRIP
class CIndexBuffer : public TBufferObject<unsigned char>
{
      /// @brief base type
      using tBase = TBufferObject<unsigned char>;

      /// @brief size of buffer
      GLsizei mSize;

      /// @brief mode, e.g. GL_TRIANGLE_STRIP
      GLenum mMode;

      /// @brief type, e.g. GL_UNSIGNED_BYTE
      GLenum mType;

   public:
      /// @brief constructor
      template<typename T>
      CIndexBuffer(const T* data, size_t size, GLenum mode)
         : tBase(reinterpret_cast<const unsigned char*>(data), size*sizeof(T), GL_ELEMENT_ARRAY_BUFFER)
         , mSize(size)
         , mMode(mode)
         , mType(glsl::TTypeID<T>::id)
      {
         constexpr GLenum id = glsl::TTypeID<T>::id;
         static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
         mSize = size;
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

#endif
