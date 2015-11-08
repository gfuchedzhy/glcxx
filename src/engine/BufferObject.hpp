/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECT_HPP
#define ENGINE_BUFFEROBJECT_HPP

#include "Attribute.hpp"

/// @brief buffer object
template<typename TAttributeDataPack>
class TBufferObject
{
   public:
      using tData = TAttributeDataPack;

      /// @brief creates buffer
      TBufferObject()
      {
         gl(glGenBuffers, 1, &mID);
      }

      /// @brief frees buffer
      ~TBufferObject()
      {
         gl(glDeleteBuffers, 1, &mID);
      }

      /// @brief upload data to buffer
      void upload(const tData* attr, size_t size)
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
         gl(glBufferData, GL_ARRAY_BUFFER, size*sizeof(tData), attr, GL_STATIC_DRAW);
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

      /// @brief attaches attribute data to given locations
      void attach(const typename tData::tLocations& locations)
      {
         Log::msg("attach ", mID);
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
         TAttributeDataPack::attach(locations);
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

      /// @brief detaches attribute data
      void detach(const typename tData::tLocations& locations)
      {
         Log::msg("detach ", mID);
         TAttributeDataPack::detach(locations);
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
