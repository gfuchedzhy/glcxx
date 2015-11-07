/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECT_HPP
#define ENGINE_BUFFEROBJECT_HPP

#include "Attribute.hpp"

/// @brief forward declaration
template<typename TAttributeDataPack>
class TBufferObject;

/// @brief buffer ptr
template<typename TAttributeDataPack>
using TBufferObjectPtr = std::shared_ptr<TBufferObject<TAttributeDataPack>>;

/// @brief buffer object
template<typename TAttributeDataPack>
class TBufferObject
{
   public:
      using tData = TAttributeDataPack;

      /// @brief factory method returning buffer objects
      static TBufferObjectPtr<tData> create()
      {
         return std::make_shared<TBufferObject>();
      }

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
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
         doAttach(locations, std::make_index_sequence<tData::attributeNum>{});
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

      /// @brief detaches attribute data
      void detach(const typename tData::tLocations& locations)
      {
         doDetach(locations, std::make_index_sequence<tData::attributeNum>{});
      }

   private:
      /// @brief enables every attribute, then calls attrib pointer for each one
      template<size_t... I>
      void doAttach(const typename tData::tLocations& locations, std::index_sequence<I...>)
      {
         swallow(gl(glEnableVertexAttribArray, I));
         swallow(gl(glVertexAttribPointer,
                    locations[I],
                    std::tuple_element<I, typename tData::tBaseTuple>::type::size,
                    std::tuple_element<I, typename tData::tBaseTuple>::type::glTypeID,
                    GL_FALSE, // not normalized
                    sizeof(tData), // stride
                    tData::template offsetof<I>::value // offset
                    ));
      }

      /// @brief disables every attribute
      template<size_t... I>
      void doDetach(const typename tData::tLocations& locations, std::index_sequence<I...>)
      {
         swallow(gl(glDisableVertexAttribArray, I));
      }

      /// @brief disabled stuff
      TBufferObject(const TBufferObject&) = delete;
      TBufferObject& operator=(const TBufferObject& other) = delete;

   private:
      /// @brief buffer id
      GLuint mID;
};

#endif // ENGINE_BUFFEROBJECT_HPP
