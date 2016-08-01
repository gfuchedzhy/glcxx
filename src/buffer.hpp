/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_BUFFER_HPP
#define GLCXX_SRC_BUFFER_HPP

#include "input_var.hpp"
#include <memory>

namespace glcxx
{
   /// @brief base class for all types of buffers
   class buffer_base
   {
         /// @brief disabled stuff
         buffer_base(const buffer_base&) = delete;
         buffer_base& operator=(const buffer_base& other) = delete;

         /// @brief buffer id
         GLuint mID;

         /// @brief target
         GLenum mTarget;

         /// @brief usage
         GLenum mUsage;

      public:
         /// @brief constructor
         buffer_base(const void* data, size_t size, GLenum usage, GLenum target)
            : mTarget(target)
            , mUsage(usage)
         {
            glcxx_gl(glGenBuffers, 1, &mID);
            upload(data, size);
         }

         /// @brief free buffer
         ~buffer_base()
         {
            glcxx_gl(glDeleteBuffers, 1, &mID);
         }

         /// @brief binds buffer
         void bind() const
         {
            glcxx_gl(glBindBuffer, mTarget, mID);
         }

         /// @brief unbinds buffer
         void unbind() const
         {
            glcxx_gl(glBindBuffer, mTarget, 0);
         }

         /// @brief uploads new data to buffer, usage = 0 means do not change usage
         void upload(const void* data, size_t size, GLenum usage = 0)
         {
            if (usage)
               mUsage = usage;
            bind();
            glcxx_gl(glBufferData, mTarget, size, data, mUsage);
            unbind();
         }
   };

   /// @brief typesafe buffer object
   template<typename TData>
   class buffer : public buffer_base
   {
      public:
         /// @brief underlying data type
         using data = TData;

         /// @brief constructor
         buffer(const TData* data, size_t size, GLenum usage = GL_STATIC_DRAW, GLenum target = GL_ARRAY_BUFFER)
            : buffer_base(data, size*sizeof(TData), usage, target)
         {}

         /// @brief uploads new data to buffer, usage = 0 means do not change usage
         void upload(const TData* data, size_t size, GLenum usage = 0)
         {
            buffer_base::upload(data, size*sizeof(TData), usage);
         }
   };

   /// @brief buffer ptr
   template<typename TData>
   using buffer_ptr = std::shared_ptr<buffer<TData>>;

   /// @brief make buffer
   template<typename TData>
   inline buffer_ptr<TData> make_buffer(const TData* data, size_t size, GLenum usage = GL_STATIC_DRAW)
   {
      return std::make_shared<buffer<TData>>(data, size, usage);
   }

   /// @brief index buffer object, this buffer accepts ubyte, ushort and uint
   /// indices, also it saves mode, e.g. GL_TRIANGLE_STRIP
   class index_buffer : public buffer_base
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
         index_buffer(const T* data, size_t size, GLenum mode, GLenum usage = GL_STATIC_DRAW)
            : buffer_base(data, size*sizeof(T), usage,
                          // before binding index buffer for upload vao should be unbound
                          // to preserve vao's internal state, note that this in not the
                          // case for vertex buffers as their bindings are not part of
                          // vao's state
                          (glcxx_gl(glBindVertexArray, 0), GL_ELEMENT_ARRAY_BUFFER))
            , mSize(size)
            , mMode(mode)
            , mType(glsl::type_id<T>::value)
         {
            constexpr GLenum id = glsl::type_id<T>::value;
            static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
         }

         /// @brief uploads new data to buffer
         template<typename T>
         void upload(const T* data, size_t size, GLenum mode, GLenum usage = 0)
         {
            constexpr GLenum id = glsl::type_id<T>::value;
            static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
            // before binding index buffer for upload vao should be unbound to
            // preserve vao's internal state, note that this in not the case for
            // vertex buffers as their bindings are not part of vao's state
            glcxx_gl(glBindVertexArray, 0);
            buffer_base::upload(data, size*sizeof(T), usage);
            mSize = size;
            mMode = mode;
            mType = id;
         }

         /// @brief draw with this index buffer
         void draw() const
         {
            glcxx_gl(glDrawElements, mMode, mSize, mType, nullptr);
         }

         /// @brief unbind index buffer
         static void unbind()
         {
            glcxx_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
         }
   };

   /// @brief index buffer ptr
   using index_buffer_ptr = std::shared_ptr<index_buffer>;

   /// @brief make index buffer
   template<typename... TArgs>
   inline index_buffer_ptr make_index_buffer(TArgs&&... args)
   {
      return std::make_shared<index_buffer>(std::forward<TArgs>(args)...);
   }
}

#endif
