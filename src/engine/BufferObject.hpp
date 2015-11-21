/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECT_HPP
#define ENGINE_BUFFEROBJECT_HPP

#include "Attribute.hpp"

/// @brief buffer object
template<typename TData>
class TBufferObject
{
   public:
      /// @brief creates buffer
      TBufferObject(const TData* data, size_t size)
      {
         gl(glGenBuffers, 1, &mID);
         bind();
         gl(glBufferData, GL_ARRAY_BUFFER, size*sizeof(TData), data, GL_STATIC_DRAW);
         unBind();
      }

      /// @brief frees buffer
      ~TBufferObject()
      {
         gl(glDeleteBuffers, 1, &mID);
      }

      /// @brief binds buffer
      void bind()
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
      }

      /// @brief unbinds buffer
      static void unBind()
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

   private:
      /// @brief disabled stuff
      TBufferObject(const TBufferObject&) = delete;
      TBufferObject& operator=(const TBufferObject& other) = delete;

   private:
      /// @brief buffer id
      GLuint mID;
};

#endif // ENGINE_BUFFEROBJECT_HPP
